# Ghid de Prezentare - OS-RISCV

## Comenzi pentru PREZENTARE LIVE

### 1. Build (dacă nu e deja compilat)
```bash
cd /home/tatiana/os/OS-RISCV
make clean
make
```

**Output așteptat**: `kernel.img created successfully`

---

### 2. Pornire sistem
```bash
make run
```

**Output așteptat**:
- Boot sequence
- MM initialization cu test de coalescing
- Timer initialization
- RAMFS cu user1 și user2
- Shell prompt: `>`

---

### 3. Demo comenzi shell

#### a) Help - listă comenzi
```
help
```

#### b) PS - procese active
```
ps
```
**Output**: PID 0 (idle) în stare READY

#### c) Uptime - timp de la boot
```
uptime
```

#### d) Mem - memory allocator
```
mem
```
**Output**: Lista blocurilor libere cu adrese

---

### 4. Demo PREEMPTIVE SCHEDULER

#### a) Pornire user1
```
exec user1
```
**Output**: 
- `exec: loading user1...`
- `exec: started PID 1`
- USER1 va afișa mesaje alternând cu shell

#### b) Pornire user2 (în paralel)
```
exec user2
```
**Output**:
- `exec: loading user2...`
- `exec: started PID 2`
- **OBSERVĂ**: USER1 și USER2 alternează perfect (preemptive scheduling!)

#### c) Verificare procese
```
ps
```
**Output**: PID 0, 1, 2 - toate READY, vezi CPU_TIME incrementat

---

### 5. Demo Kill

#### a) Kill process 1
```
kill 1
```
**Output**: `Procesul 1 a fost terminat`

#### b) Verificare
```
ps
```
**Output**: Doar PID 0 și 2 (USER1 dispărut)

#### c) Kill process 2
```
kill 2
```

#### d) Verificare finală
```
ps
```
**Output**: Doar PID 0 (idle)

---

### 6. Ieșire din QEMU

Apasă: **Ctrl+A** apoi **X**

Sau lasă să ruleze și timeout-ul va închide automat.

---

## Secvență Completă de Prezentare (Copy-Paste)

```bash
# Build
cd /home/tatiana/os/OS-RISCV
make clean && make

# Run
make run

# În shell (tastează manual):
help
ps
uptime
mem
exec user1
exec user2
ps
kill 1
ps
kill 2
ps
uptime
# Ctrl+A X pentru exit
```

---

## Ce să SUBLINIEZI la Prezentare

### ✅ TRUE Preemptive Scheduling
- **Preempție la FIECARE tick** (~10ms)
- USER1 și USER2 **alternează automat** fără să ceară yield
- Funcția `preempt()` în `timer.c` - forțează context switch

### ✅ Syscalls Implementate
- **SYS_WRITE (64)**: Print mesaje
- **SYS_EXIT (93)**: Terminare clean a proceselor
- **SYS_YIELD (124)**: Context switch voluntar
- **SYS_GETTIME (169)**: Citește tick counter

### ✅ Memory Management
- **Heap allocator** cu coalescing (lipire blocuri)
- Test vizibil la boot: `SUCCESS! Coalescing works!`

### ✅ Shell Monitor
- **6 comenzi**: help, ps, uptime, mem, kill, exec
- Interactiv, funcționează în paralel cu user processes

### ✅ RAMFS
- **User programs** încărcate în memorie la boot
- Load la adrese diferite: user1 @ 0x80400000, user2 @ 0x80500000

---

## Troubleshooting

### Dacă nu pornește
```bash
# Verifică QEMU instalat
which qemu-system-riscv32

# Verifică toolchain
which riscv64-unknown-elf-gcc
```

### Dacă compilarea eșuează
```bash
make clean
make
```

### Dacă shell nu răspunde
- Așteaptă prompt `>`
- Dacă procesele printează, tastează între output-uri
- Folosește `kill` pentru a opri procese zgomotoase

---

## Note Importante

1. **Tick rate**: 100 Hz (10ms per tick)
2. **Uptime display**: La fiecare 10 secunde (1000 ticks)
3. **User processes**: Printează la fiecare 5 ticks (~50ms)
4. **TRUE Preemptive**: Context switch automat la FIECARE tick
5. **Deterministic**: Zero randomness - demo 100% reproducibil

---

## Comenzi Scurte pentru Demo Rapid (20 secunde)

```
exec user1
exec user2
ps
kill 1
ps
kill 2
ps
```

Această secvență arată:
- ✅ Preemptive scheduling (alternare automată)
- ✅ Process management (exec + kill)
- ✅ System calls (write, exit)
- ✅ Shell interactiv
