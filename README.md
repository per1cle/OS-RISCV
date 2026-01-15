# OS-RISCV - Sistema de Operare Preemptiv

Un sistem de operare minimal pentru arhitectura RISC-V RV32IMA, implementat în C și Assembly RISC-V, rulând pe QEMU virtual machine.

## Caracteristici Principale

 **TRUE Preemptive Scheduling** - Context switch automat la fiecare tick (10ms)  
 **Memory Management** - Heap allocator cu coalescing (first-fit)  
 **System Calls** - WRITE, EXIT, YIELD, GETTIME (Linux ABI)  
 **Program Loading** - RAMFS embedded cu loader ELF  
 **Interactive Shell** - 6 comenzi: help, ps, uptime, mem, kill, exec  
 **Timer & Interrupts** - CLINT timer @ 100Hz cu uptime display  

---

## 🚀 Quick Start

### Cerințe Sistem

```bash
# Verifică toolchain RISC-V
which riscv64-unknown-elf-gcc
which qemu-system-riscv32
```

### Build & Run

```bash
# Compilare
make clean
make

# Pornire sistem
make run
```

### Exit QEMU

**Ctrl+A** apoi **X**

---

## 🎮 Shell - Comenzi Interactive

| Comandă | Descriere | Exemplu Output |
|---------|-----------|----------------|
| help | Lista comenzi disponibile | Afișează toate comenzile |
| ps | Afișează procese active | PID, STATE, CPU_TIME |
| uptime | Timp de la boot | Ticks și secunde |
| mem | Statistici memorie heap | Total, Used, Free, Blocks |
| exec <prog> | Lansare program | exec user1, exec user2 |
| kill <pid> | Terminare proces | kill 1 |

### Demo Rapid

```bash
make run

# În shell:
help
ps
exec user1
exec user2      # VEZI ALTERNAREA AUTOMATĂ!
ps
kill 1
ps
```

---

## 🎬 Demo Automat (Script)

```bash
./final_test.sh
```

Script-ul demonstrează automat toate funcționalitățile.

---

## 📖 Documentație Tehnică

### 1. Arhitectura Sistemului

Sistemul este organizat pe mai multe straturi (layers):

```
┌─────────────────────────────────────────┐
│       User Programs (user1, user2)      │  ← Procese user-space
├─────────────────────────────────────────┤
│     Shell + Syscall Interface           │  ← Interfață interactivă
├─────────────────────────────────────────┤
│   Scheduler + Process Manager           │  ← Preemptive scheduling
├─────────────────────────────────────────┤
│   Memory Manager + Timer + Loader       │  ← Gestiune resurse
├─────────────────────────────────────────┤
│      Hardware (UART, CLINT)             │  ← Device drivers
└─────────────────────────────────────────┘
```

#### Boot Sequence (9 Pași)

1. **start.S** - Entry point (0x80000000), setup stack, call main()
2. **main()** - Kernel initialization, print banner
3. **mm_init()** - Initialize heap @ 0x80012000, run coalescing tests
4. **timer_init()** - Configure CLINT @ 100Hz (10ms/tick)
5. **ramfs_init()** - Load user programs (user1 @ 0x80400000, user2 @ 0x80500000)
6. **proc_init()** - Initialize process table, idle process (PID 0)
7. **interrupt_init()** - Set trap handler, enable MIE
8. **Shell loop** - Interactive mode, read/execute commands
9. **Scheduler** - Round-robin preemptive, triggered by timer (10ms)

#### Memory Layout

```
0x80000000  ┌──────────────────┐
            │  Kernel Code     │  ← start.S, main.c, etc.
0x80012000  ├──────────────────┤
            │  Heap (dynamic)  │  ← kmalloc/kfree
            │  ...             │
0x80400000  ├──────────────────┤
            │  User1 Program   │  ← user1.bin (ELF loaded)
0x80500000  ├──────────────────┤
            │  User2 Program   │  ← user2.bin (ELF loaded)
            └──────────────────┘
```

---

### 2. Scheduler & System Calls

#### TRUE Preemptive Scheduling

**Mecanism:**

```c
// timer.c - tick() function
void tick() {
    timer_ticks++;
    preempt();  // ← PREEMPTION: Context switch la FIECARE tick!
}
```

**Efect vizibil:**

```
USER1: Running...  [t=000]
USER2: Working...  [t=000]  ← Alternare AUTOMATĂ fără yield()!
USER1: Running...  [t=001]
USER2: Working...  [t=001]
```

**Policy:** Round-robin
- Fiecare proces primește 10ms (1 tick)
- Context switch automat via comutare.S
- Nu necesită yield() voluntar

#### System Calls Implementate

| Syscall | Număr | Descriere | Parametri |
|---------|-------|-----------|-----------|
| SYS_WRITE | 64 | Write to UART | a0=fd, a1=buf, a2=len |
| SYS_EXIT | 93 | Terminate process | a0=exit_code |
| SYS_YIELD | 124 | Voluntary context switch | - |
| SYS_GETTIME | 169 | Read tick counter | - |

**Entry Flow:**

1. User program: ecall instruction
2. CPU: Trap to kernel mode (M-mode)
3. interrupt.c: Dispatch to syscall_handler()
4. Execute syscall
5. Return to user mode via mret

---

### 3. Memory Management

#### Heap Allocator

**Algoritm:** First-fit cu coalescing

**Funcții Principale:**

```c
void* kmalloc(int size);  // Alocă memorie
void kfree(void *ptr);    // Eliberează memorie
```

**Coalescing (Lipire blocuri adiacente):**

Test vizibil la boot:

```
--- Test 5 (Coalescing / Lipire) ---
  > Freeing p3 (addr: 0x80012084)
  > Freeing p2 (addr: 0x800120f4)
  > Attempting kmalloc(220) (should succeed if merged)...
  > SUCCESS! Coalescing works! Addr: 0x80012084
```

**Avantaje:**
- Previne fragmentarea memoriei
- Blocuri libere adiacente sunt unite automat
- Eficiență crescută pentru alocări mari

**Limitări:**
- Algoritm simplu (first-fit, nu best-fit)
- Nu există protecție overflow/underflow
- Heap size fix (stabilit la compile-time)

---

### 4. Limitări și Direcții de Extindere

#### Limitări Curente

**1. Arhitectură:**
- ❌ Rulează doar în M-mode (Machine mode) - nu există separare kernel/user
- ❌ Nu există page tables sau MMU (Memory Management Unit)
- ❌ Procese user rulează cu privilegii kernel (risc securitate)

**2. Memory Management:**
- ❌ Heap size fix (~134MB) - nu poate crește dinamic
- ❌ Nu există protecție memorie între procese
- ❌ Nu există swap sau virtual memory

**3. Scheduler:**
- ❌ Round-robin simplu - nu există priorități
- ❌ Nu există sleep/wake mechanism
- ❌ Timeslice fix (10ms) - nu este configurabil

**4. I/O:**
- ❌ UART polled mode - nu folosește interrupts pentru RX
- ❌ Nu există buffer circular pentru input

**5. File System:**
- ❌ RAMFS read-only - nu poate crea/șterge fișiere
- ❌ User programs embedded static la compile-time

**6. Sincronizare:**
- ❌ Nu există mutexe sau semafoare
- ❌ Nu există atomic operations

#### Direcții de Extindere

**Prioritate ÎNALTĂ:**

1. **U-mode + S-mode**
   - Implementare S-mode (Supervisor) pentru kernel
   - Procese user în U-mode (User)
   - **Impact:** Separare reală kernel/user, securitate îmbunătățită

2. **Virtual Memory (SV32)**
   - Page tables (4KB pages)
   - MMU pentru traducere virtuală → fizică
   - **Impact:** Izolare procese, securitate

3. **Priority Scheduler**
   - Multiple queue levels (high, normal, low)
   - Time slice variabil
   - **Impact:** Responsiveness mai bun

**Prioritate MEDIE:**

4. **UART Interrupt-driven**
   - RX interrupt pentru input
   - **Impact:** Shell mai responsive

5. **Sleep/Wake Mechanism**
   - sleep(ms) syscall
   - Process state: RUNNING, READY, SLEEPING
   - **Impact:** Eficiență CPU

6. **VFS + Real Filesystem**
   - VFS layer abstract
   - FAT32 sau ext2 simplu
   - **Impact:** Persistență date

**Prioritate SCĂZUTĂ:**

7. **Multi-core Support** - SMP, spinlocks, CPU affinity
8. **Networking** - Ethernet driver, TCP/IP stack
9. **Debugging Tools** - GDB stub, breakpoints software

#### Roadmap Sugerat

**Faza 1 (1-2 luni):** U-mode + S-mode, Virtual memory SV32, UART interrupt-driven

**Faza 2 (2-3 luni):** Priority scheduler, Sleep/wake mechanism, Mutexe

**Faza 3 (3-4 luni):** VFS + FAT32, Dynamic loading, Multi-core

**Faza 4 (4+ luni):** Networking, Advanced debugging, Optimizări

---

## 🛠️ Build System

### Compilare

```bash
make clean    # Curăță build artifacts
make          # Compilare kernel + user programs
make run      # Pornește QEMU interactive
```

### Flags Compilare

```makefile
CFLAGS = -Wall -Werror -O0 -nostdlib -ffreestanding \
         -mabi=ilp32 -march=rv32ima_zicsr -mcmodel=medany
```

- **-Wall -Werror**: Toate warning-urile devin erori
- **-O0**: Fără optimizări (pentru debugging)
- **-nostdlib**: Nu folosi standard library
- **-ffreestanding**: Freestanding environment (no OS)
- **-march=rv32ima_zicsr**: RV32 + Integer + Mul/Div + Atomic + CSR

---

## 🧪 Testing & Debugging

### Testing Manual

```bash
make run

# Test comenzi shell
help
ps
uptime
mem

# Test exec + scheduling
exec user1
exec user2      # Vezi alternarea USER1/USER2

# Test kill
ps
kill 1
ps              # Verifică că PID 1 a dispărut
```

### Demo Automat

```bash
./final_test.sh
```

Script execută secvența completă cu timing corect.

### Debugging cu GDB

**Terminal 1 - Start QEMU în debug mode:**

```bash
qemu-system-riscv32 -M virt -kernel kernel.img -nographic -bios none -s -S
```

**Terminal 2 - Conectare GDB:**

```bash
riscv64-unknown-elf-gdb kernel.elf

(gdb) target remote :1234
(gdb) break main
(gdb) continue
(gdb) step
(gdb) print timer_ticks
```

---

## ✅ Checklist Cerințe Implementate

### Cerințe Funcționale

- [x] **Boot to Kernel** - Boot sequence cu inițializare completă (9 pași)
- [x] **Interrupts & Timer** - CLINT timer @ 100Hz (10ms/tick)
- [x] **TRUE Preemptive Scheduling** - Context switch la FIECARE tick
- [x] **Memory Management** - Heap allocator cu coalescing (first-fit)
- [x] **System Call ABI** - 4 syscalls: WRITE, EXIT, YIELD, GETTIME
- [x] **Program Loading** - RAMFS embedded + ELF loader
- [x] **Shell/Monitor** - 6 comenzi interactive
- [x] **Device I/O** - UART driver polled mode (RX/TX)

### Cerințe Non-Funcționale

- [x] **Code Quality** - Compilare cu -Wall -Werror (zero warnings)
- [x] **Documentation** - README complet cu arhitectură, boot flow, API
- [x] **Deterministic Demo** - 100% reproducibil (zero randomness)
- [x] **Clean Code** - Fără comentarii, cod production-ready

---

## 📊 Specificații Tehnice

| Parametru | Valoare |
|-----------|---------|
| Arhitectură | RISC-V RV32IMA |
| Platform | QEMU virt machine |
| Timer | CLINT @ 0x02000000, 100Hz |
| Heap Start | 0x80012000 |
| User1 Address | 0x80400000 |
| User2 Address | 0x80500000 |
| Context Switch | La fiecare 10ms (100Hz) |
| Process Duration | ~10-15 minute (100k iterations) |
| Print Frequency | La 1000 iterații |

---

## 📝 Notițe Finale

### Puncte Forte

✅ **TRUE Preemptive** - Alternare automată fără yield()  
✅ **Vizibil** - Procese lungi (~10 min) permit testare live  
✅ **Clean Code** - Zero comentarii, zero warnings  
✅ **Deterministic** - Demo 100% reproducibil  

### Limitări Cunoscute

❌ **M-mode only** - Nu există separare kernel/user reală  
❌ **No virtual memory** - Fără page tables, fără protecție memorie  
❌ **Simple scheduler** - Round-robin fără priorități  
❌ **Polled UART** - Nu folosește interrupts pentru RX  

---

## 📧 Contact

**Project:** OS-RISCV TRUE Preemptive System  
**Status:** ✅ Complete & Production Ready  
**Repository:** https://github.com/per1cle/OS-RISCV  
**Branch:** preemptive  
**Last Update:** Ianuarie 2026  

---

**Happy hacking! 🚀**
