# CHEAT SHEET - Prezentare OS-RISCV

## 🎯 Comenzi Exacte pentru Demo (Copy-Paste Friendly!)

### 1️⃣ Build (dacă e necesar)
```bash
cd /home/tatiana/os/OS-RISCV
make clean
make
```
**Verifică**: `kernel.img created successfully`

---

### 2️⃣ Start Sistem
```bash
make run
```
**Așteaptă**: Shell prompt `>`

---

### 3️⃣ Comenzi Shell (tastează manual, câte una!)

```
help
```
**Output**: Lista cu 6 comenzi

```
ps
```
**Output**: PID 0 (idle) READY

```
uptime
```
**Output**: Tick Counter și uptime în secunde

```
mem
```
**Output**: Memory stats (Total/Used/Free/Blocks)

```
exec user1
```
**Output**: 
- `exec: loading user1...`
- `exec: started PID 1`
- USER1 începe să printeze

```
exec user2
```
**Output**:
- `exec: loading user2...`
- `exec: started PID 2`
- **OBSERVĂ**: USER1 și USER2 alternează! (TRUE PREEMPTIVE!)

```
ps
```
**Output**: PID 0, 1, 2 - vezi CPU_TIME diferit

```
kill 1
```
**Output**: `Procesul 1 a fost terminat`

```
ps
```
**Output**: Doar PID 0 și 2 (USER1 dispărut)

```
kill 2
```
**Output**: `Procesul 2 a fost terminat`

```
ps
```
**Output**: Doar PID 0 (sistem curat)

```
uptime
```
**Output**: Vezi că tick counter a crescut

---

### 4️⃣ Exit QEMU
Apasă: **Ctrl+A** apoi **X**

---

## 🎓 Ce să SUBLINIEZI

### ✅ TRUE Preemptive Scheduling
- USER1 și USER2 **alternează automat**
- Preempție la **fiecare tick** (~10ms)
- **NU folosesc yield()** - e forțat de kernel!

### ✅ Syscalls Funcționale
- **SYS_WRITE (64)**: Print mesaje
- **SYS_EXIT (93)**: Terminare clean
- **SYS_YIELD (124)**: Context switch voluntar
- **SYS_GETTIME (169)**: Citește tick counter

### ✅ Memory Management
- **Coalescing**: Vezi la boot "SUCCESS! Coalescing works!"
- **Heap allocator**: Funcțional, vezi stats cu `mem`

### ✅ Shell Interactive
- **6 comenzi**: help, ps, uptime, mem, kill, exec
- Funcționează **în paralel** cu user processes

---

## ⚡ Demo Rapid (15 secunde)

Dacă ai puțin timp:
```
exec user1
exec user2
ps
kill 1
kill 2
ps
```

Arată:
- ✅ Preemptive scheduling (alternare)
- ✅ Process management (exec + kill)
- ✅ System calls (toate funcționale)

---

## 🐛 Troubleshooting Live

| Problemă | Soluție |
|----------|---------|
| Shell nu răspunde | Așteaptă `>`, tastează între output-uri |
| Prea mult output | Folosește `kill` pentru a opri procesele |
| QEMU nu pornește | Verifică `which qemu-system-riscv32` |
| Build eșuează | `make clean && make` |

---

## 📊 Statistici Impresionante

- **~15 fișiere** cod sursă C/Assembly
- **~2000 linii** documentație tehnică
- **4 syscalls** implementate (Linux ABI)
- **6 comenzi** shell interactive
- **100% deterministic** (zero randomness)
- **Zero warnings** compilare (`-Wall -Werror`)

---

## 🎬 Secvență Prezentare Completă (30s)

```bash
# Terminal
make run

# Shell
help
ps
uptime
exec user1
exec user2
ps
# OBSERVĂ ALTERNAREA!
kill 1
ps
kill 2
ps
uptime
# Ctrl+A X
```

---

**Good luck cu prezentarea! 🚀**

---

## 📱 Quick Reference Card

```
┌─────────────────────────────────────────┐
│  OS-RISCV Demo Quick Reference          │
├─────────────────────────────────────────┤
│  Build:    make clean && make           │
│  Run:      make run                     │
│  Exit:     Ctrl+A → X                   │
├─────────────────────────────────────────┤
│  help      Lista comenzi                │
│  ps        Procese active               │
│  uptime    Tick counter                 │
│  mem       Memory stats                 │
│  exec <p>  Lansează program             │
│  kill <n>  Termină PID                  │
└─────────────────────────────────────────┘
```
