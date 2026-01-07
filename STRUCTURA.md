# Structură Finală Proiect - OS-RISCV

```
OS-RISCV/
│
├── 📄 README.md                    ← Overview principal proiect
├── 🎯 PREZENTARE.md                ← GHID PENTRU DEMO LIVE (ESENȚIAL!)
│
├── 🔧 Makefile                     ← Build system
│
├── 📁 headere/                     ← Header files (.h)
│   ├── csr.h                       ← CSR definitions
│   ├── kernel_lib.h                ← String functions
│   ├── mm.h                        ← Memory management
│   ├── proc.h                      ← Process management
│   └── syscall.h                   ← System calls
│
├── 📁 src/                         ← Source code
│   ├── boot/
│   │   └── start.S                 ← Boot entry point (Assembly)
│   ├── kernel/
│   │   ├── main.c                  ← Kernel main + shell loop
│   │   ├── shell.c                 ← Shell command handlers
│   │   ├── timer.c                 ← Timer + PREEMPTION
│   │   ├── proc.c                  ← Scheduler + processes
│   │   ├── mm.c                    ← Heap allocator
│   │   ├── syscall.c               ← Syscall handlers
│   │   ├── loader.c                ← ELF program loader
│   │   ├── uart.c                  ← UART driver
│   │   ├── interrupt.c             ← Interrupt handling
│   │   └── comutare.S              ← Context switching (Assembly)
│   └── kernel_lib/
│       └── kernel_lib.c            ← String utilities
│
├── 📁 user/                        ← User programs
│   ├── user1.c                     ← Test program 1
│   └── user2.c                     ← Test program 2
│
├── 📁 script/                      ← Linker scripts
│   ├── kernel.ld                   ← Kernel linker script
│   ├── user.ld                     ← User1 linker script
│   └── user2.ld                    ← User2 linker script
│
├── 📁 docs/                        ← 📚 DOCUMENTAȚIE TEHNICĂ (13 documente)
│   ├── README.md                   ← Index documentație
│   ├── ARCHITECTURE.md             ← Diagrame arhitectură
│   ├── BOOT_FLOW.md                ← Secvență boot
│   ├── SCHEDULER_README.md         ← TRUE Preemptive Scheduler
│   ├── HOW_TO_RUN.md               ← Build & run instructions
│   ├── DOCUMENTATION.md            ← Documentație completă
│   ├── DEMO_GUIDE.md               ← Demo scenarios
│   ├── SHELL_README.md             ← Shell documentation
│   ├── TRUE_PREEMPTIVE_IMPLEMENTATION.md
│   ├── REQUIREMENTS_CHECKLIST.md
│   ├── TESTING_CHECKLIST.md
│   ├── CHANGES_SUMMARY.md
│   └── PREEMPTION_DESIGN.md
│
├── 🧪 Test Scripts (optional)
│   ├── test_requirements.sh        ← Test complet
│   ├── run_shell.sh                ← Run shell interactive
│   ├── demo_shell.sh
│   ├── demo_timer.sh
│   ├── final_test.sh
│   ├── test_preemption.sh
│   ├── test_shell.sh
│   └── test_timer.sh
│
└── 📦 Build Artifacts (generated)
    ├── kernel.img                  ← Kernel binary (run this!)
    ├── kernel.elf                  ← Kernel ELF
    ├── user1.elf, user1.bin        ← User program 1
    └── user2.elf, user2.bin        ← User program 2
```

---

## 📚 Documentație Organizată

### 🎯 Pentru PREZENTARE LIVE
**→ `PREZENTARE.md`** - Instrucțiuni exacte pas cu pas!

### 📖 Pentru Înțelegere Tehnică
**→ `docs/`** - Toate documentele tehnice

### 🔍 Quick Access

| Ce vrei să faci? | Document |
|------------------|----------|
| **Demo LIVE** | `PREZENTARE.md` |
| **Build & Run** | `docs/HOW_TO_RUN.md` |
| **Înțelege arhitectura** | `docs/ARCHITECTURE.md` |
| **Înțelege boot** | `docs/BOOT_FLOW.md` |
| **Înțelege scheduler** | `docs/SCHEDULER_README.md` |
| **Toate detaliile** | `docs/DOCUMENTATION.md` |

---

## ✅ Structură Curată

✅ **Root directory**: Doar esențiale (README, PREZENTARE, Makefile)  
✅ **docs/**: Toată documentația tehnică organizată  
✅ **src/**: Cod sursă structurat pe componente  
✅ **headere/**: Header files separate  
✅ **user/**: User programs izolate  

---

**Total Files**:
- Source: ~15 fișiere `.c/.S`
- Headers: 5 fișiere `.h`
- Documentation: 14 fișiere `.md` (~2000 linii)
- Build scripts: 1 Makefile + 8 test scripts
