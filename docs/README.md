# Documentație Tehnică - OS-RISCV

Aceasta este colecția completă de documentație tehnică pentru proiectul OS-RISCV.

## 📚 Documente Disponibile

### 🎯 Esențiale pentru Prezentare
- **[../PREZENTARE.md](../PREZENTARE.md)** - **Ghid complet pentru demo LIVE cu comenzi exacte**
- **[../README.md](../README.md)** - Overview-ul principal al proiectului

---

### 📐 Arhitectură & Design
- **[ARCHITECTURE.md](ARCHITECTURE.md)** - Diagrame complete arhitectură sistem
  - Layers diagram
  - Data flow diagram  
  - Memory layout diagram
  - File structure

- **[BOOT_FLOW.md](BOOT_FLOW.md)** - Secvența de boot în 9 pași
  - Timeline boot sequence
  - Initialization steps
  - Component startup order

---

### 🔄 Scheduler & Processes
- **[SCHEDULER_README.md](SCHEDULER_README.md)** - TRUE Preemptive Scheduler
  - Implementare preemption
  - Context switching
  - Process states
  - Round-robin algorithm

- **[TRUE_PREEMPTIVE_IMPLEMENTATION.md](TRUE_PREEMPTIVE_IMPLEMENTATION.md)** - Detalii implementare
  - timer.c modifications
  - preempt() function
  - Testing results

- **[PREEMPTION_DESIGN.md](PREEMPTION_DESIGN.md)** - Design decisions

---

### 🎮 User Interface
- **[SHELL_README.md](SHELL_README.md)** - Shell/Monitor documentation
  - Comenzi disponibile
  - Command implementation
  - User interaction

- **[DEMO_GUIDE.md](DEMO_GUIDE.md)** - Ghid demo complet
  - Demo scenarios
  - Expected outputs
  - Troubleshooting

---

### 📖 Documentație Completă
- **[DOCUMENTATION.md](DOCUMENTATION.md)** - Documentație tehnică comprehensivă
  - Toate componentele
  - Implementation details
  - Code organization

- **[HOW_TO_RUN.md](HOW_TO_RUN.md)** - Instrucțiuni detaliate build & run
  - Prerequisites
  - Build steps
  - Running options
  - Debugging

---

### ✅ Testing & Requirements
- **[REQUIREMENTS_CHECKLIST.md](REQUIREMENTS_CHECKLIST.md)** - Checklist cerințe
  - Functional requirements
  - Non-functional requirements
  - Status tracking

- **[TESTING_CHECKLIST.md](TESTING_CHECKLIST.md)** - Testing checklist
  - Test scenarios
  - Validation steps
  - Expected results

---

### 📝 Change History
- **[CHANGES_SUMMARY.md](CHANGES_SUMMARY.md)** - Summary of changes
  - Implementation timeline
  - Major modifications
  - Bug fixes

- **[README_NEW.md](README_NEW.md)** - Extended README (archived)

---

## 🎯 Quick Navigation

### Pentru Prezentare
👉 Start aici: **[../PREZENTARE.md](../PREZENTARE.md)**

### Pentru Înțelegere Tehnică
1. Citește **[ARCHITECTURE.md](ARCHITECTURE.md)** - înțelege structura
2. Citește **[BOOT_FLOW.md](BOOT_FLOW.md)** - înțelege boot sequence
3. Citește **[SCHEDULER_README.md](SCHEDULER_README.md)** - înțelege scheduler-ul

### Pentru Build & Run
👉 Vezi **[HOW_TO_RUN.md](HOW_TO_RUN.md)**

### Pentru Demo
👉 Vezi **[DEMO_GUIDE.md](DEMO_GUIDE.md)**

---

## 📊 Document Status

| Document | Lines | Status | Last Update |
|----------|-------|--------|-------------|
| ARCHITECTURE.md | ~250 | ✅ Complete | Jan 2026 |
| BOOT_FLOW.md | ~200 | ✅ Complete | Jan 2026 |
| SCHEDULER_README.md | ~180 | ✅ Complete | Jan 2026 |
| DOCUMENTATION.md | ~450 | ✅ Complete | Jan 2026 |
| HOW_TO_RUN.md | ~170 | ✅ Complete | Jan 2026 |

---

**Total Documentation**: ~2000 de linii de documentație tehnică detaliată.
