# 🚀 Boot Flow - OS-RISCV

## 📋 Secvența Completă de Inițializare

```
Power-On / Reset
      │
      ▼
┌─────────────────────────────────────────────────────────────┐
│  1. BOOT (start.S - entry point)                             │
│     @ Address: 0x80000000                                    │
│                                                               │
│     ✓ Setează gp (global pointer)                           │
│     ✓ Setează sp (stack pointer) → stack_top                │
│     ✓ Configurează mtvec → handler (trap vector)            │
│     ✓ Clear BSS section (zero-initialize)                   │
│     ✓ Call kmain()                                           │
└─────────────────────────────────────────────────────────────┘
      │
      ▼
┌─────────────────────────────────────────────────────────────┐
│  2. KERNEL MAIN (main.c::kmain)                              │
│                                                               │
│     ✓ uart_putstring("Booting...\n")                        │
│     ✓ uart_putstring("Hi Barbie!\n")                        │
└─────────────────────────────────────────────────────────────┘
      │
      ▼
┌─────────────────────────────────────────────────────────────┐
│  3. MEMORY MANAGER (mm.c::init_mm)                           │
│                                                               │
│     ✓ Setează heap start/end (_heap_start, _heap_end)      │
│     ✓ Creează primul bloc liber (întregul heap)            │
│     ✓ Test 1-4: Aloc/free standard                          │
│     ✓ Test 5: Coalescing (merge adjacent free blocks)       │
│     ✓ Output: "SUCCESS! Coalescing works!"                  │
└─────────────────────────────────────────────────────────────┘
      │
      ▼
┌─────────────────────────────────────────────────────────────┐
│  4. TIMER (timer.c::init_timer)                              │
│                                                               │
│     ✓ Citește mtime (current time)                          │
│     ✓ Setează mtimecmp = mtime + TIMER_INTERVAL            │
│     ✓ Enable timer interrupt (MIE_MTIE în mie)              │
│     ✓ Enable global interrupts (MSTATUS_MIE în mstatus)    │
│     ✓ Output: "Timer initializat"                           │
└─────────────────────────────────────────────────────────────┘
      │
      ▼
┌─────────────────────────────────────────────────────────────┐
│  5. RAMFS LOADER (loader.c::init_ramfs)                      │
│                                                               │
│     ✓ Find embedded binaries (_binary_user1_bin_start)     │
│     ✓ Register user1 → RAMFS entry                          │
│     ✓ Register user2 → RAMFS entry                          │
│     ✓ Output: "RAMFS: Initialized"                          │
│     ✓ Output: "  - user1 (XXX bytes)"                       │
│     ✓ Output: "  - user2 (XXX bytes)"                       │
└─────────────────────────────────────────────────────────────┘
      │
      ▼
┌─────────────────────────────────────────────────────────────┐
│  6. PROCESS TABLE (proc.c::init_proc)                        │
│                                                               │
│     ✓ Initialize proc_table[0..9]                           │
│     ✓ Set all states to UNUSED                              │
│     ✓ Set PIDs                                               │
│     ✓ Zero CPU time counters                                │
└─────────────────────────────────────────────────────────────┘
      │
      ▼
┌─────────────────────────────────────────────────────────────┐
│  7. SHELL BANNER                                             │
│                                                               │
│     ✓ Output: "========================================"     │
│     ✓ Output: "  OS-RISCV Shell Monitor v1.0"               │
│     ✓ Output: "========================================"     │
│     ✓ Output: "Tastati 'help' pentru lista de comenzi"      │
└─────────────────────────────────────────────────────────────┘
      │
      ▼
┌─────────────────────────────────────────────────────────────┐
│  8. SCHEDULER START (proc.c::scheduler)                      │
│                                                               │
│     ✓ Set proc_actual = 0 (idle process)                    │
│     ✓ Enter infinite loop:                                   │
│         while(1) {                                           │
│           shell_poll();          ← Check for input          │
│           find_next_ready();     ← Round-robin              │
│           comutare(idle, next);  ← Context switch           │
│         }                                                    │
│                                                               │
│     🔄 System now running - accepting commands              │
└─────────────────────────────────────────────────────────────┘
      │
      ▼
┌─────────────────────────────────────────────────────────────┐
│  9. BACKGROUND TIMER                                         │
│     (Parallel cu scheduler loop)                             │
│                                                               │
│     Every ~10ms (100Hz):                                     │
│       ✓ Timer interrupt fires                               │
│       ✓ tick() called                                        │
│       ✓ counter_tick++                                       │
│       ✓ CPU time tracking                                    │
│       ✓ preempt() → context switch (if user process)        │
│                                                               │
│     Every 10s (1000 ticks):                                  │
│       ✓ Display: "[Uptime: Xm Ys | Ticks: Z]"               │
└─────────────────────────────────────────────────────────────┘
      │
      ▼
   ┌─────────┐
   │ RUNNING │  ← System operational, accepting shell commands
   └─────────┘
```

---

## ⏱️ Timeline Aproximativ

| Timp | Step | Descriere |
|------|------|-----------|
| 0ms | Boot | Entry point, stack setup, trap vector |
| ~1ms | kmain | Banner messages |
| ~2ms | MM Init | Heap allocator + coalescing test |
| ~3ms | Timer Init | CLINT configuration |
| ~4ms | RAMFS Init | Load user programs metadata |
| ~5ms | Proc Init | Process table initialization |
| ~6ms | Shell Banner | Display welcome message |
| ~7ms | Scheduler | Enter main loop |
| 10ms | First Tick | Timer interrupt #1 |
| 10s | First Uptime | Display "[Uptime: 10s | Ticks: 1000]" |

---

## 🔧 Detalii Pe Componente

### 1. Boot Assembly (start.S)

```asm
boot:
    # 1. Setup global pointer
    la gp, gp
    
    # 2. Setup stack
    la sp, stack_top        # Point to top of kernel stack
    
    # 3. Setup trap handler
    la t0, handler
    csrw mtvec, t0          # Set trap vector
    
    # 4. Clear BSS (uninitialized data)
    la a0, _bss
    la a1, _bss_end
bss_loop:
    bge a0, a1, bss_done
    sw zero, 0(a0)
    addi a0, a0, 4
    j bss_loop
bss_done:
    
    # 5. Jump to C code
    call kmain
    
    # Should never return
nope:
    j nope
```

**Registre Setup:**
- `gp` (x3): Global pointer pentru data section
- `sp` (x2): Stack pointer pentru kernel stack
- `mtvec`: Machine trap vector (adresa handler)

**BSS Clear:**
- Zeroes all uninitialized global variables
- Range: `_bss` to `_bss_end` (defined in linker script)

---

### 2. Kernel Main (main.c)

```c
void kmain(void) {
    // Welcome message
    uart_putstring("Booting...\n");
    uart_putstring("Hi Barbie!\n");
    
    // Initialize subsystems
    init_mm();      // Memory manager
    init_timer();   // Timer + interrupts
    init_ramfs();   // Load user programs
    init_proc();    // Process table
    
    // Display shell banner
    uart_putstring("\n========================================\n");
    uart_putstring("  OS-RISCV Shell Monitor v1.0\n");
    uart_putstring("========================================\n");
    uart_putstring("Tastati 'help' pentru lista de comenzi\n\n> ");
    
    // Start scheduler (never returns)
    scheduler();
}
```

---

### 3. Memory Manager Init (mm.c)

```c
void init_mm(void) {
    uart_putstring("MM: Initializing kernel heap...\n");
    
    // Get heap boundaries (from linker script)
    heap_start = &_heap_start;
    heap_end = &_heap_end;
    
    // Create initial free block (entire heap)
    struct block *initial = (struct block *)heap_start;
    initial->size = (char*)heap_end - (char*)heap_start 
                    - sizeof(struct block);
    initial->free = 1;
    initial->next = NULL;
    
    freelist = initial;
    
    // Run tests
    test_mm();  // Includes coalescing verification
}
```

**Output Example:**
```
MM: Initializing kernel heap...
--- Test 1-4 (Standard) ---
  > p1 alloc at: 0x80011e38
  > p2 alloc at: 0x80011ea8
  > p1 freed.
  > p3 (reusing p1) alloc at: 0x80011e38
--- Test 5 (Coalescing / Lipire) ---
  > SUCCESS! Coalescing works! Addr: 0x80011e38
--- MM TEST COMPLETE ---
```

---

### 4. Timer Init (timer.c)

```c
void init_timer(void) {
    // Get current time
    long current_time = *CLINT_MTIME;
    
    // Set compare value (next interrupt in 100000 cycles)
    *CLINT_MTIMECMP = current_time + TIMER_INTERVAL;
    
    // Enable timer interrupt in MIE register
    long mie_val = read_csr(mie);
    mie_val |= MIE_MTIE;
    write_csr(mie, mie_val);
    
    // Enable global interrupts in MSTATUS
    long mstatus_val = read_csr(mstatus);
    mstatus_val |= MSTATUS_MIE;
    write_csr(mstatus, mstatus_val);
    
    uart_putstring("Timer initializat\n");
}
```

**CSR Registers:**
- `mie`: Machine Interrupt Enable (bit 7 = timer)
- `mstatus`: Machine Status (bit 3 = global interrupt enable)

---

### 5. RAMFS Init (loader.c)

```c
void init_ramfs(void) {
    uart_putstring("RAMFS: Initialized\n");
    
    // user1
    ramfs[0].name = "user1";
    ramfs[0].data = _binary_user1_bin_start;
    ramfs[0].size = _binary_user1_bin_end - _binary_user1_bin_start;
    
    uart_putstring("  - user1 (0x");
    print_hex(ramfs[0].size);
    uart_putstring(" bytes)\n");
    
    // user2
    ramfs[1].name = "user2";
    ramfs[1].data = _binary_user2_bin_start;
    ramfs[1].size = _binary_user2_bin_end - _binary_user2_bin_start;
    
    uart_putstring("  - user2 (0x");
    print_hex(ramfs[1].size);
    uart_putstring(" bytes)\n");
}
```

**Embedded Binaries:**
- Linked into kernel image using `ld -r -b binary`
- Symbols created: `_binary_<name>_bin_start/end`

---

### 6. Process Table Init (proc.c)

```c
void init_proc(void) {
    for (int i = 0; i < PROC_MAX; i++) {
        proc_table[i].state = UNUSED;
        proc_table[i].pid = i;
        proc_table[i].cpu_time = 0;
    }
}
```

**Initial State:**
- All 10 slots marked UNUSED
- PID 0 reserved for scheduler/idle
- PIDs 1-9 available for user processes

---

### 7. Scheduler Start (proc.c)

```c
void scheduler(void) {
    proc_actual = 0;
    proc_table[0].state = READY;
    static int last_proc = 0;
    
    while(1) {
        // Check for shell input
        shell_poll();
        
        // Find next ready process (round-robin)
        int next = find_next_ready();
        if(next == -1) continue;
        
        // Switch to process
        proc_actual = next;
        last_proc = next;
        comutare(&proc_table[0].context, 
                 &proc_table[next].context);
        
        // Return here after preemption
        proc_actual = 0;
    }
}
```

---

## 🎯 Verificare Boot Success

### Mesaje Așteptate la Boot:

```
Booting...
Hi Barbie!
MM: Initializing kernel heap...
--- Test 1-4 (Standard) ---
  > p1 alloc at: 0x...
  > p2 alloc at: 0x...
  > p1 freed.
  > p3 (reusing p1) alloc at: 0x...
--- Test 5 (Coalescing / Lipire) ---
  > SUCCESS! Coalescing works!
--- MM TEST COMPLETE ---
Timer initializat
RAMFS: Initialized
  - user1 (0x000003ab bytes)
  - user2 (0x000003ab bytes)

========================================
  OS-RISCV Shell Monitor v1.0
========================================
Tastati 'help' pentru lista de comenzi

>
```

### Indicatori de Success:

✅ "Booting..." apare  
✅ "SUCCESS! Coalescing works!" - Memory manager funcțional  
✅ "Timer initializat" - Interrupts configurate  
✅ "RAMFS: Initialized" - User programs încărcate  
✅ Shell prompt `>` apare - Scheduler pornit  

După 10 secunde:  
✅ `[Uptime: 10s | Ticks: 1000]` - Timer funcționează  

---

## 🔍 Debugging Boot Issues

### Dacă sistemul nu bootează:

1. **Nu apare nimic**:
   - Check UART configuration
   - Verify entry point în linker script
   - Check QEMU command

2. **Crash după "Booting..."**:
   - Check stack setup
   - Verify BSS clearing
   - Check heap boundaries

3. **Crash la timer init**:
   - Verify CLINT addresses
   - Check CSR access permissions

4. **Nu apare shell prompt**:
   - Check scheduler loop
   - Verify shell_poll() implementation

---

## 📊 State After Boot

```
┌──────────────────────────────────────────┐
│  System State @ Boot Complete            │
├──────────────────────────────────────────┤
│  CPU: Running scheduler (PID 0)          │
│  Timer: Active, 100Hz interrupts         │
│  Memory: Heap initialized, tested        │
│  Programs: user1, user2 loaded in RAMFS │
│  Processes: All slots UNUSED (ready)     │
│  Shell: Waiting for commands             │
│  Tick Counter: Incrementing              │
└──────────────────────────────────────────┘
```

**System is now ready to accept commands!** 🚀
