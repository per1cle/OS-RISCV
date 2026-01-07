# OS-RISCV - Mini Operating System

Un sistem de operare minimal pentru arhitectura RISC-V (RV32IMA), implementat în C și Assembly, care rulează pe QEMU.

## ✨ Caracteristici Principale

✅ **TRUE Preemptive Scheduler** - Round-robin cu time slice fix (~10ms), preemption forțată la fiecare tick  
✅ **Memory Manager** - Heap allocator cu first-fit și coalescing  
✅ **Timer (CLINT)** - 100Hz interrupts, CPU time tracking  
✅ **System Calls** - write(), yield(), gettime() (ABI RISC-V standard)  
✅ **Program Loader** - RAMFS simplu pentru user programs  
✅ **Interactive Shell** - 6 comenzi (help, ps, uptime, mem, kill, exec)  
✅ **Device I/O** - UART driver pentru serial console  

---

## 🚀 Quick Start

### Build & Run

```bash
# Build
make clean && make

# Run
make run

# În shell:
help
exec user1
exec user2
ps
```

### Expected Output

```
Booting...
Hi Barbie!
SUCCESS! Coalescing works!
Timer initializat

> exec user1
> exec user2

USER1: Running...  [t=000]
USER2: Working... [t=000]    ← Fair preemption!
USER1: Running...  [t=000]
USER2: Working... [t=000]
```

---

## 📚 Documentație Completă

### Cerințe Non-Funcționale ✅

#### Code Hygiene
- ✅ Limbaje: C + Assembly RISC-V
- ✅ Compilare: `-Wall -Werror` (zero warnings)
- ✅ Cod curat și comentat

#### Documentație

| Document | Descriere |
|----------|-----------|
| [ARCHITECTURE.md](ARCHITECTURE.md) | 🏗️ Diagramă arhitecturală (straturi kernel, procese, drivere) |
| [BOOT_FLOW.md](BOOT_FLOW.md) | 🚀 Boot flow (succesiunea de inițializări) |
| [HOW_TO_RUN.md](HOW_TO_RUN.md) | ▶️ Instrucțiuni de rulare complete |
| [DEMO_GUIDE.md](DEMO_GUIDE.md) | 🎬 Demo pas cu pas (7-10 min) |
| [TRUE_PREEMPTIVE_IMPLEMENTATION.md](TRUE_PREEMPTIVE_IMPLEMENTATION.md) | ⚙️ Detalii preemptive scheduler |
| [TESTING_CHECKLIST.md](TESTING_CHECKLIST.md) | ✅ Checklist verificare cerințe |

#### Demo Determinist

**Fără componente aleatoare:**
- Scheduling: Round-robin FIX (PID 1 → 2 → 1)
- Timer: 100Hz CONSTANT
- Time slice: 10ms FIX
- Memory: First-fit DETERMINIST
- PIDs: SECVENȚIAL
- Load addresses: FIXE

**Reproducibilitate 100%** - Demo-ul produce același output!

---

## 🎯 Demo Minim Obligatoriu - Checklist

✅ **1. Boot → banner ("Booting...")**  
✅ **2. Tick counter → afișare și incrementare**  
✅ **3. exec u1 & u2; ps → 2 procese runnable**  
✅ **4. syscall write() → output intercalat (fair preemption)**  
✅ **5. kill <pid> → reduce procese; ps confirmă**  
✅ **6. Heap stats → înainte și după execuție**  

### Demo Script

```bash
make run

# Așteaptă boot
# [Uptime: 10s | Ticks: 1000]  ← După 10 secunde

mem                    # Heap ÎNAINTE
exec user1
exec user2
ps                     # 2 procese active
# Observă output intercalat
kill 1
ps                     # 1 proces rămas
mem                    # Heap DUPĂ
```

---

## 📂 Structura

```
OS-RISCV/
├── headere/           # Headers (.h)
├── src/
│   ├── boot/          # start.S (boot + trap handler)
│   ├── kernel/        # Kernel C + Assembly
│   └── kernel_lib/    # Utilities
├── user/              # user1.c, user2.c
├── script/            # Linker scripts
└── Makefile
```

---

## 🔧 Implementare Tehnică

### TRUE Preemptive Scheduler

```c
void tick() {
    counter_tick++;
    proc_table[proc_actual].cpu_time++;
    
    // PREEMPTION FORȚATĂ la fiecare tick
    preempt();  // ← 10ms time slice!
}
```

**Caracteristici:**
- Preemption la fiecare ~10ms
- Round-robin strict
- Fair CPU garantat
- Time slice FIX

### Memory Manager

```c
void *kmalloc(long size);  // Allocate
void kfree(void *ptr);     // Free (+ coalescing)
void mm_stats(void);       // Statistics
```

**Features:**
- First-fit allocation
- Adjacent block coalescing
- Fragmentation reduction

### System Calls

| # | Name | Descriere |
|---|------|-----------|
| 64 | SYS_WRITE | Write to UART |
| 124 | SYS_YIELD | Yield CPU |
| 169 | SYS_GETTIME | Get tick counter |

---

## 🧪 Testing

```bash
./test_requirements.sh     # Full test
./test_preemption.sh       # Preemption test
```

---

## ✅ Status Final

🟢 **COMPLET** - Toate cerințele îndeplinite!

### Cerințe Funcționale
- ✅ Boot complet cu banner
- ✅ Timer 100Hz funcțional
- ✅ Scheduler preemptiv (round-robin, time slice fix)
- ✅ 2+ procese runnable
- ✅ Fair preemption (output intercalat)
- ✅ Syscalls funcționale (write, yield, gettime)
- ✅ Memory manager cu coalescing
- ✅ Shell interactiv (6 comenzi)
- ✅ Process control (exec, kill, ps)

### Cerințe Non-Funcționale
- ✅ Code hygiene: C + Assembly, -Wall -Werror
- ✅ Diagramă arhitecturală completă
- ✅ Boot flow documentat
- ✅ Instrucțiuni de rulare
- ✅ Demo determinist (fără aleatorie)

### Demo Minim
- ✅ Boot → banner
- ✅ Tick counter afișat
- ✅ 2 procese runnable
- ✅ Output intercalat (fair preemption)
- ✅ kill funcționează
- ✅ Heap stats înainte/după

**System ready for demonstration!** 🚀

---

## 📖 Referințe

- xv6 (MIT) - Educational OS
- RISC-V Specification
- SiFive CLINT Documentation

## 👤 Autor

**Tatiana** - OS-RISCV Project  
Branch: `preemptive`

---

**Pentru demo detaliat vezi**: [DEMO_GUIDE.md](DEMO_GUIDE.md)  
**Pentru troubleshooting vezi**: [HOW_TO_RUN.md](HOW_TO_RUN.md)
