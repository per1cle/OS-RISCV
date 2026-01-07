# OS-RISCV - Documentație Tehnică

## Cuprins
1. [Arhitectura Sistemului](#arhitectura-sistemului)
2. [Boot Flow](#boot-flow)
3. [Scheduler & Syscalls](#scheduler--syscalls)
4. [Memory Management](#memory-management)
5. [Device I/O](#device-io)
6. [Instrucțiuni de Rulare](#instrucțiuni-de-rulare)
7. [Demo Minim Obligatoriu](#demo-minim-obligatoriu)
8. [Limitări și Direcții de Extindere](#limitări-și-direcții-de-extindere)

---

## Arhitectura Sistemului

### Straturi

```
┌─────────────────────────────────────────┐
│         User Programs (user1, user2)    │  ← User Space
├─────────────────────────────────────────┤
│         System Call Interface           │  ← ABI Layer
│    (write, yield, gettime)              │
├─────────────────────────────────────────┤
│         Kernel Core                     │
│  ┌────────────┬──────────────────────┐  │
│  │  Scheduler │   Memory Manager     │  │  ← Kernel Space
│  │ (RR, Time  │   (List Allocator)   │  │
│  │  Slice)    │                      │  │
│  ├────────────┼──────────────────────┤  │
│  │ Interrupt  │   Process Manager    │  │
│  │  Handler   │   (PCB, Context)     │  │
│  └────────────┴──────────────────────┘  │
├─────────────────────────────────────────┤
│         Device Drivers                  │
│  ┌────────────┬──────────────────────┐  │
│  │    UART    │   CLINT Timer        │  │  ← Hardware Drivers
│  │  (16550)   │   (Machine Mode)     │  │
│  └────────────┴──────────────────────┘  │
├─────────────────────────────────────────┤
│         Hardware (QEMU virt)            │  ← Physical Layer
│    RISC-V RV32IMA + Zicsr              │
└─────────────────────────────────────────┘
```

### Componente Principale

#### 1. **Kernel Core**
- `src/kernel/main.c` - Entry point, inițializare sistem
- `src/kernel/interrupt.c` - Trap handler, interrupt dispatch
- `src/kernel/timer.c` - CLINT timer, tick management

#### 2. **Process Management**
- `src/kernel/proc.c` - Scheduler, process table, context switch
- `src/kernel/comutare.S` - Assembly context switch routine
- `src/boot/start.S` - Boot assembly, trap entry point

#### 3. **Memory Management**
- `src/kernel/mm.c` - Heap allocator (list-based, coalescing)
- Headers: `headere/mm.h`

#### 4. **System Calls**
- `src/kernel/syscall.c` - Syscall dispatcher
- `headere/syscall.h` - Syscall numbers (64, 124, 169)

#### 5. **Device Drivers**
- `src/kernel_lib/kernel_lib.c` - UART driver (polled mode)
- Timer: CLINT (Core Local Interruptor)

#### 6. **User Programs**
- `user/user1.c`, `user/user2.c` - Test programs
- Linked at fixed addresses: 0x80400000, 0x80500000

#### 7. **Shell/Monitor**
- `src/kernel/shell.c` - Interactive shell
- Commands: help, ps, uptime, mem, kill, exec

---

## Boot Flow

### Secvența de Inițializare

```
 1. QEMU Firmware
      ↓
 2. _start (start.S)              ← Entry point @ 0x80000000
      ├─ Setup stack pointer
      ├─ Setup trap vector (mtvec)
      └─ Jump to kmain()
      ↓
 3. kmain() (main.c)
      ├─ Print boot banner
      ├─ mm_init()                ← Initialize heap allocator
      │    └─ Create free list
      ├─ init_timer()             ← Configure CLINT
      │    ├─ Set MTIMECMP
      │    └─ Enable MIE_MTIE
      ├─ init_ramfs()             ← Embed user programs
      │    └─ List available binaries
      ├─ init_proc()              ← Initialize process table
      │    └─ Set all to UNUSED
      ├─ shell_init()             ← Display shell banner
      └─ scheduler()              ← Start scheduling loop
           ↓
 4. Scheduler Loop
      ├─ shell_poll()            ← Check UART input
      ├─ Select next process
      └─ comutare()              ← Context switch
           ↓
 5. User Process Execution
      ├─ syscall (ecall)
      └─ timer interrupt → preemption
```

### Detalii Boot

**1. Hardware Initialization** (start.S)
```assembly
_start:
    la sp, _stack_end          # Setup stack
    la t0, handler             # Trap handler address
    csrw mtvec, t0            # Set trap vector
    j kmain                    # Jump to C code
```

**2. Kernel Initialization** (main.c)
- **UART**: Already initialized by firmware
- **Heap**: mm_init() creates initial free block
- **Timer**: CLINT configured for 100Hz ticks
- **Processes**: Empty process table
- **Shell**: Polling-based input

**3. First Schedule**
- Idle process (PID 0) starts
- Shell polling in idle loop
- User processes loaded on demand via `exec`

---

## Scheduler & Syscalls

### TRUE Preemptive Round-Robin Scheduler

**Design Philosophy**: Preemptive multitasking with timer-enforced context switching.

#### Scheduling Model
- **Primary mechanism**: Timer interrupt triggers preemption
- **Timer role**: Enforces time slice and fair CPU distribution
- **Round-robin**: Processes scheduled in circular order
- **Time slice**: Fixed ~10ms quantum per process
- **Preemption**: FORCED at every timer tick

#### Algorithm
```c
void tick() {
    counter_tick++;
    
    // Update CPU time tracking
    if(proc_actual > 0) {
        proc_table[proc_actual].cpu_time++;
    }
    
    // PREEMPTION: Force context switch every tick
    preempt();  // ← KEY: Forced preemption!
}

void preempt() {
    if(proc_actual > 0) {
        proc_table[proc_actual].state = READY;
        comutare(&proc_table[proc_actual].context, 
                 &proc_table[0].context);  // Switch to scheduler
    }
}

// Scheduler loop (runs as PID 0)
while (1) {
    shell_poll();              // Check for shell input
    
    next = find_next_ready();  // Round-robin selection
    if (next == -1) continue;  // No process ready
    
    proc_actual = next;
    comutare(idle, next);      // Context switch to process
    
    // Process returns here AFTER being preempted by timer
    proc_actual = 0;           // Back to idle
}
```

#### Time Slice Enforcement
- **Timer frequency**: 100Hz (tick every ~10ms)
- **Preemption**: Automatic at every tick
- **Fair distribution**: Each process gets equal quantum
- **No cooperation needed**: Processes are INTERRUPTED, not yielding

#### Why TRUE Preemptive?
1. **Timer enforces fairness**: No process can monopolize CPU
2. **Forced context switches**: Happens at every tick, regardless of process behavior
3. **Time slice guarantee**: Each process gets ~10ms before being preempted
4. **Real preemption**: Not dependent on cooperative yields

#### Process States
```c
enum states {
    READY,    // Runnable, waiting for CPU
    RUNNING,  // Currently executing (tracked for debugging)
    UNUSED    // Free slot in process table
};
```

#### Context Structure
```c
struct context {
    long ra, sp;              // Return address, stack pointer
    long s0-s11;              // Callee-saved registers only
    // Note: Caller-saved (a0-a7, t0-t6) saved by trap handler
};
```
};
```

#### Context Switch (comutare.S)
```assembly
comutare:
    # Save old context
    sw ra, 0(a0)
    sw sp, 4(a0)
    sw s0, 8(a0)
    # ... s1-s11
    
    # Load new context
    lw ra, 0(a1)
    lw sp, 4(a1)
    lw s0, 8(a1)
    # ... s1-s11
    
    ret                       # Jump to ra (new process)
```

### System Call ABI

#### Syscall Numbers (Linux RISC-V Standard)
```c
#define SYS_WRITE   64        // write(fd, buf, len)
#define SYS_YIELD   124       // yield()
#define SYS_GETTIME 169       // gettime()
```

#### Syscall Flow
```
User Space                    Kernel Space
─────────                     ────────────

syscall(64, 1, buf, len)
    ↓
  ecall                   ──→  handler (start.S)
                                  ├─ Save all registers
                                  ├─ Load a0-a7 from stack
                                  └─ Call handler_c()
                                       ↓
                                  handler_c() (interrupt.c)
                                       ├─ Check mcause
                                       └─ Call syscall_handler()
                                            ↓
                                  syscall_handler() (syscall.c)
                                       ├─ Switch(a7) {
                                       │    case 64: do_sys_write()
                                       │    case 124: do_sys_yield()
                                       │    case 169: do_sys_gettime()
                                       │  }
                                       └─ Return value in a0
                                  ←─── Return to user
    ↓
  return a0
```

#### Implementation Details

**write(fd, buf, len)**
```c
long do_sys_write(int fd, const char *buf, long len) {
    if (fd == 1) {  // stdout
        for (long i = 0; i < len; i++) {
            kernel_putchar(buf[i]);
        }
        return len;
    }
    return -1;
}
```

**yield()**
```c
long do_sys_yield(void) {
    yield();  // Context switch to scheduler
    return 0;
}
```

**gettime()**
```c
long do_sys_gettime(void) {
    extern long counter_tick;
    return counter_tick;
}
```

---

## Memory Management

### Heap Allocator Design

**Type**: Simple linked-list allocator with coalescing
**Location**: Kernel heap @ `_heap_start` to `_heap_end`

#### Block Structure
```c
typedef struct block_header {
    size_t size;              // Total size (including header)
    int is_free;              // 1 = free, 0 = allocated
    struct block_header* next;
} block_header_t;
```

#### Memory Layout
```
┌──────────────────────────────────────────┐
│  _heap_start                             │
├──────────────────────────────────────────┤
│  [Header][Allocated Data...............] │  Block 1
├──────────────────────────────────────────┤
│  [Header][Free Space...................]  │  Block 2
├──────────────────────────────────────────┤
│  [Header][Allocated Data..]              │  Block 3
├──────────────────────────────────────────┤
│  [Header][Free Space...................]  │  Block 4
├──────────────────────────────────────────┤
│  _heap_end                               │
└──────────────────────────────────────────┘
```

#### kmalloc(size_t size)

**Algorithm**:
1. Search free list for first-fit block
2. If block > requested + threshold:
   - Split block
   - Return first part
   - Add remainder to free list
3. Else: return entire block
4. 8-byte alignment

**Complexity**: O(n) where n = number of blocks

#### kfree(void *ptr)

**Algorithm**:
1. Mark block as free
2. **Coalesce with next** if next is free
3. **Coalesce with prev** if prev is free
   - Requires list traversal from head

**Features**:
- Double-free detection
- Adjacent block merging (coalescing)
- Reduces fragmentation

#### Statistics
```c
void mm_stats(size_t *total, size_t *used, 
              size_t *free, int *blocks);
```

Traverses list to compute:
- Total heap size
- Used memory
- Free memory
- Number of blocks

---

## Device I/O

### UART Driver (16550 Compatible)

#### Registers (Memory-Mapped @ 0x10000000)
```c
#define UART_THR  0x10000000  // Transmit Holding Register
#define UART_RBR  0x10000000  // Receive Buffer Register
#define UART_LSR  0x10000005  // Line Status Register
```

#### Line Status Register (LSR)
- Bit 0: **Data Ready** (DR) - 1 if RBR has data
- Bit 5: **Transmitter Empty** (THRE) - 1 if THR ready

#### Polled Output
```c
void kernel_putchar(char c) {
    while ((*UART_LSR & (1 << 5)) == 0);  // Wait THRE
    *UART_THR = c;                         // Write char
}
```

#### Polled Input (Blocking)
```c
char kernel_getchar(void) {
    while ((*UART_LSR & 0x01) == 0);      // Wait DR
    return *UART_RBR;                      // Read char
}
```

#### Polled Input (Non-blocking)
```c
int kernel_getchar_nonblock(void) {
    if (*UART_LSR & 0x01) {
        return *UART_RBR;
    }
    return -1;  // No data
}
```

#### Shell Integration
- Uses **non-blocking** input
- Polled in idle loop (shell_poll())
- Doesn't block scheduler

### CLINT Timer

#### Registers
```c
#define CLINT_MTIME    0x0200BFF8  // Current time
#define CLINT_MTIMECMP 0x02004000  // Compare value
```

#### Timer Interrupt
```c
void tick() {
    counter_tick++;                        // Global tick counter
    proc_table[proc_actual].cpu_time++;   // Process CPU time
    
    if (counter_tick % 100 == 0) {
        kernel_putstring("Tick!\n");       // Visible tick
    }
    
    *CLINT_MTIMECMP = *CLINT_MTIME + INTERVAL;  // Next interrupt
}
```

#### Initialization
```c
void init_timer(void) {
    *CLINT_MTIMECMP = *CLINT_MTIME + 100000;  // ~100Hz
    
    write_csr(mie, read_csr(mie) | MIE_MTIE);    // Enable timer interrupt
    write_csr(mstatus, read_csr(mstatus) | MSTATUS_MIE);  // Global enable
}
```

---

## Instrucțiuni de Rulare

### Cerințe
- **Toolchain**: `riscv64-unknown-elf-gcc`, `ld`, `objcopy`
- **Emulator**: `qemu-system-riscv32`
- **OS**: Linux (testat pe Ubuntu)

### Build
```bash
# Clean build
make clean && make

# Outputs:
# - kernel.elf    (ELF executable)
# - kernel.img    (Raw binary for QEMU)
# - user1.elf/bin, user2.elf/bin
```

### Run
```bash
# Testare automată
./test_requirements.sh    # Test complet cerințe
./test_shell.sh          # Test rapid shell
./demo_shell.sh          # Demo cu scenarii

# Rulare interactivă
./run_shell.sh           # Shell interactiv
# Ieșire: Ctrl+A apoi X

# Rulare manuală
qemu-system-riscv32 \
    -machine virt \
    -bios none \
    -kernel kernel.img \
    -nographic \
    -serial mon:stdio
```

### Debugging
```bash
# Terminal 1: Start QEMU cu GDB server
qemu-system-riscv32 -machine virt -bios none \
    -kernel kernel.img -nographic -s -S

# Terminal 2: Connect GDB
riscv64-unknown-elf-gdb kernel.elf \
    -ex "target remote :1234" \
    -ex "break kmain" \
    -ex "continue"
```

---

## Demo Minim Obligatoriu

### Scenario Demo

```
1. Boot → Boot banner
   Expected: "Booting..." + "Hi Barbie!"
   
2. Tick counter
   Expected: "Tick!" periodic messages
   
3. Shell commands
   > help
   > uptime     ← Shows tick counter growing
   > mem        ← Shows heap stats
   
4. Execute programs
   > exec user1
   > exec user2
   > ps         ← Shows 2 processes with PID, state, CPU time
   
5. Syscall demonstration
   - USER1/USER2 output intercalat → preemption works
   - sys_write() functional
   - sys_yield() allows fair scheduling
   - sys_gettime() returns tick counter
   
6. Process management
   > kill 1
   > ps         ← Process 1 gone
   
7. Memory statistics
   > mem        ← Show heap before/after allocations
```

### Expected Output (Sample)
```
Booting...
Hi Barbie!
MM: Initializing kernel heap...
Timer initializat
RAMFS: Initialized
  - user1 (0x000003ab bytes)
  - user2 (0x000003ab bytes)

========================================
  OS-RISCV Shell Monitor v1.0
========================================

> help
Comenzile disponibile:
  help, ps, uptime, mem, kill, exec

> uptime
System Uptime:
  Tick Counter: 15
  Uptime: 0 seconds

> mem
Memory Statistics (Heap):
  Total size:  134145432 bytes
  Used:        232 bytes
  Free:        134145200 bytes

> exec user1
exec: started PID 1

> exec user2
exec: started PID 2

> ps
PID  STATE    CPU_TIME
---  -------  --------
0    READY    0 <--
1    READY    25
2    READY    18

Tick!
USER1: Running... [t=025]
USER2: Working... [t=018]
```

---

## Limitări și Direcții de Extindere

### Limitări Actuale

1. **No Paging**: Procese user rulează în memorie fizică directă
2. **Fixed Addresses**: user1 @ 0x80400000, user2 @ 0x80500000
3. **Simple Scheduler**: Round-robin fix, fără priorități
4. **No Fork**: Procese create manual prin exec
5. **No VFS**: Doar RAMFS embedded
6. **Polled I/O**: UART polled mode, nu interrupt-driven

### Direcții de Extindere (BONUS)

#### 1. **Memory Protection** ⭐⭐⭐
- Implement Sv32 paging
- Separate user/kernel address spaces
- Copy-on-write fork()

#### 2. **Advanced Scheduling** ⭐⭐
- Priority-based scheduling
- Multi-level feedback queue
- CPU affinity (pentru multi-core)

#### 3. **File System** ⭐⭐⭐
- Mini VFS layer
- virtio-blk driver (read-only)
- Load programs from disk

#### 4. **IPC** ⭐⭐
- Signals (SIGKILL, SIGTERM)
- Pipes pentru procese
- Shared memory

#### 5. **Networking** ⭐⭐⭐⭐
- virtio-net driver
- TCP/IP stack (lwIP)
- Socket API

#### 6. **Multi-core** ⭐⭐⭐⭐
- SMP support
- Per-CPU schedulers
- Spinlocks

---

## Testare

### Unit Tests
- Memory allocator: coalescing, alignment
- Scheduler: round-robin fairness
- Syscalls: user→kernel→user transition

### Integration Tests
```bash
./test_requirements.sh   # All features
./test_shell.sh         # Shell commands
./demo_shell.sh         # Demo scenario
```

### CI Pipeline (Potential)
```yaml
- Build kernel
- Boot test: grep "Booting"
- Timer test: grep "Tick"
- Shell test: exec → ps → kill
- Memory test: grep "SUCCESS! Coalescing"
```

---

## Referințe

- RISC-V Privileged Spec v1.12
- QEMU virt machine documentation
- XV6-RISCV (for reference, not copied)
- Linux RISC-V syscall ABI

---

**Autor**: OS-RISCV Team  
**Data**: Ianuarie 2026  
**Branch**: preemptive  
**Status**: ✅ Cerințe minime complete
