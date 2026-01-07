# OS-RISCV - True Preemptive Operating System

Un sistem de operare minimal pentru arhitectura RISC-V RV32IMA, implementat în C și Assembly RISC-V, rulând pe QEMU.

## 🎯 Caracteristici Principale

✅ **TRUE Preemptive Scheduling** - Context switch automat la fiecare tick (~10ms)  
✅ **Memory Management** - Heap allocator cu coalescing  
✅ **System Calls** - WRITE, EXIT, YIELD, GETTIME (Linux ABI)  
✅ **Program Loading** - RAMFS embedded cu loader ELF  
✅ **Interactive Shell** - 6 comenzi: help, ps, uptime, mem, kill, exec  
✅ **Timer & Interrupts** - CLINT timer cu uptime display  

---

## 🚀 Quick Start

### Cerințe Sistem
```bash
# Verifică toolchain
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

### Demo Rapid (20 secunde)
```bash
make run
# În shell:
exec user1
exec user2
ps
kill 1
kill 2
ps
# Ctrl+A X pentru exit
```

---

## 📖 Documentație

### Pentru Prezentare
👉 **[PREZENTARE.md](PREZENTARE.md)** - **Ghid complet pentru demo LIVE cu comenzi exacte**

### Documentație Tehnică
- 📐 [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) - Arhitectură sistem (diagrame)
- 🔄 [docs/BOOT_FLOW.md](docs/BOOT_FLOW.md) - Secvență de boot (9 pași)
- 🎮 [docs/SCHEDULER_README.md](docs/SCHEDULER_README.md) - TRUE Preemptive Scheduler
- 📚 [docs/DOCUMENTATION.md](docs/DOCUMENTATION.md) - Documentație completă

---

## 🎯 Shell Comenzi

| Comandă | Descriere | Exemplu Output |
|---------|-----------|----------------|
| `help` | Lista comenzi | Afișează toate comenzile |
| `ps` | Procese active | PID, STATE, CPU_TIME |
| `uptime` | Timp de la boot | Ticks și secunde |
| `mem` | Memory stats | Blocuri libere heap |
| `exec <prog>` | Lansare program | `exec user1` |
| `kill <pid>` | Terminare proces | `kill 1` |

---

## 🔬 TRUE Preemptive Scheduling

```c
// timer.c - tick() function
void tick() {
    timer_ticks++;
    if (timer_ticks % 1000 == 0) {
        // Uptime display la 10s
    }
    preempt();  // ← PREEMPTION: Context switch automat!
}
```

**Efect vizibil**: USER1 și USER2 alternează automat fără `yield()`!

---

## 📊 System Calls Implementate

| Syscall | Număr | Descriere |
|---------|-------|-----------|
| `SYS_WRITE` | 64 | Write to UART |
| `SYS_EXIT` | 93 | Terminate process |
| `SYS_YIELD` | 124 | Voluntary context switch |
| `SYS_GETTIME` | 169 | Read tick counter |

---

## 🏗️ Structura Proiect

```
OS-RISCV/
├── PREZENTARE.md          ← Ghid pentru demo LIVE
├── README.md              ← Acest fișier
├── Makefile               ← Build system
├── headere/               ← Header files
│   ├── proc.h             ← Process management
│   ├── mm.h               ← Memory management
│   └── syscall.h          ← System calls
├── src/
│   ├── boot/start.S       ← Boot entry point
│   ├── kernel/
│   │   ├── main.c         ← Kernel main + shell
│   │   ├── timer.c        ← Timer + preemption
│   │   ├── proc.c         ← Scheduler + processes
│   │   ├── mm.c           ← Heap allocator
│   │   ├── syscall.c      ← Syscall handlers
│   │   └── loader.c       ← ELF program loader
│   └── kernel_lib/        ← String functions
├── user/
│   ├── user1.c            ← Test program 1
│   └── user2.c            ← Test program 2
└── docs/                  ← Documentație tehnică
```

---

## ✅ Status Implementare

### Cerințe Funcționale Minime
- [x] **Boot to Kernel** - Boot sequence cu inițializare completă
- [x] **Interrupts & Timer** - CLINT timer cu tick la 100Hz
- [x] **TRUE Preemptive Scheduling** - Context switch la FIECARE tick
- [x] **Memory Management** - Heap allocator cu coalescing
- [x] **System Call ABI** - 4 syscalls (WRITE, EXIT, YIELD, GETTIME)
- [x] **Program Loading** - RAMFS + ELF loader
- [x] **Shell/Monitor** - 6 comenzi interactive
- [x] **Device I/O** - UART driver polled mode

### Cerințe Non-Funcționale
- [x] **Code Quality** - Compilare cu `-Wall -Werror` (zero warnings)
- [x] **Documentation** - Arhitectură, boot flow, how to run
- [x] **Deterministic Demo** - 100% reproducibil (zero randomness)

---

## 🎓 Pentru Dezvoltatori

### Build System
```bash
make clean    # Curăță build artifacts
make          # Compilare kernel + user programs
make run      # Pornește QEMU interactive
```

### Testing
```bash
./test_requirements.sh   # Test automat complet
```

### Debugging
```bash
# Terminal 1: Start QEMU în debug mode
qemu-system-riscv32 -M virt -kernel kernel.img -nographic -bios none -s -S

# Terminal 2: GDB
riscv64-unknown-elf-gdb kernel.elf
(gdb) target remote :1234
(gdb) break main
(gdb) continue
```

---

## 📝 Notițe Tehnice

- **Arhitectură**: RISC-V RV32IMA
- **Platform**: QEMU virt machine
- **Timer**: CLINT @ 0x02000000, 100Hz (10ms/tick)
- **Memory**: Heap @ 0x80012000
- **User Programs**: user1 @ 0x80400000, user2 @ 0x80500000
- **Tick Display**: Uptime automată la fiecare 10 secunde

---

## 📧 Contact & Contribuții

**Project**: OS-RISCV TRUE Preemptive System  
**Status**: ✅ Complete & Production Ready  
**Last Update**: Ianuarie 2026

---

## 🎬 Demo Live

**Pentru prezentare**: Vezi **[PREZENTARE.md](PREZENTARE.md)** - comenzi exacte pas cu pas!
