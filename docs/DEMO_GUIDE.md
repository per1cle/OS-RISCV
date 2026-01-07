# 🎯 Ghid Demonstrație Live - OS-RISCV

## 🎮 Demo Determinist - Fără Aleatorie

### ✅ Garantii Deterministe

| Component | Comportament |
|-----------|--------------|
| **Scheduling** | Round-robin FIX - PID 1 → PID 2 → PID 1 (circular) |
| **Timer** | 100Hz FIX - Tick la fiecare 10ms constant |
| **Time slice** | 10ms FIX - TIMER_INTERVAL = 100000 (constant) |
| **Memory allocation** | First-fit DETERMINIST - Aceeași ordine mereu |
| **Process IDs** | SECVENȚIAL - 1, 2, 3, ... (no randomness) |
| **Load addresses** | FIXE - user1: 0x80400000, user2: 0x80500000 |
| **Context switch** | PREDICTIBIL - La fiecare tick (100Hz) |

### ❌ NU Există Componente Aleatoare

- ❌ NO random seeds
- ❌ NO random scheduling decisions
- ❌ NO random memory addresses
- ❌ NO random process selection
- ❌ NO random time slices

### 📊 Reproducibilitate 100%

- **Același build** → identical binary (deterministic compilation)
- **Același run** → same execution order
- **Aceleași comenzi** → same output sequence
- **Aceeași secvență** → reproducible demonstration

**Rezultat**: Demo-ul poate fi repetat cu ACELAȘI output de fiecare dată!

---

## 📋 Demo Minim Obligatoriu - Checklist

✅ **1. Boot** → afișează banner („Booting...")  
✅ **2. Tick counter** → afișează și crește la fiecare 10s  
✅ **3. exec user1 & user2; ps** → 2 procese runnable  
✅ **4. syscall write()** → output intercalat (fair preemption)  
✅ **5. kill <pid>** → reduce procese; ps confirmă  
✅ **6. Heap stats** → înainte și după execuție (mem command)  

---

## Pregătire

```bash
cd /home/tatiana/os/OS-RISCV
make clean && make
```

---

## 📋 Demonstrație Completă (7-10 minute)

### 1️⃣ BOOT → Afișare Banner (30 secunde)

```bash
./run_shell.sh
```

**Ce să demonstrezi:**
- ✅ "Booting..." și "Hi Barbie!" → Boot complet
- ✅ "MM: Initializing kernel heap..." → Memory manager
- ✅ "SUCCESS! Coalescing works!" → Heap allocator funcțional
- ✅ "Timer initializat" → Interrupt setup
- ✅ "RAMFS: Initialized" → Program loader
- ✅ Shell banner și prompt `>`

**Timp estimat: ~5 secunde**

---

### 2️⃣ TICK → Afișare Tick-uri Periodice

**În shell, așteaptă 10 secunde fără să tastezi nimic**

**Ce să arăti:**
- ✅ `[Uptime: 10s | Ticks: 1000]` → Tick-uri periodice funcționale
- ✅ Format clar cu minute și secunde
- ✅ Tick counter crește constant

**Timp estimat: 10 secunde**

---

### 3️⃣ SHELL → Comenzi Interactive

**Tastează în ordine:**

```
help
```
**Arată:** Lista completă de comenzi

```
uptime
```
**Arată:** Tick counter live și uptime

```
mem
```
**Arată:** Statistici heap (Total, Used, Free, Blocks)

```
ps
```
**Arată:** Lista procese (doar idle PID 0 inițial)

**Timp estimat: 30 secunde**

---

### 4️⃣ EXEC → Lansare Procese User

```
exec user1
```

**Ce să arăti:**
- ✅ "exec: loading user1..."
- ✅ "exec: started PID 1"
- ✅ **"USER1: Hello from user program 1!"** → Program user rulează!
- ✅ **Output intercalat** cu tastarea comenzilor → Preemption!

```
exec user2
```

**Arată:**
- ✅ "exec: started PID 2"
- ✅ "USER2: Hello from user program 2!"

**Timp estimat: 20 secunde**

---

### 5️⃣ Procese Active → Verificare Preemption

```
ps
```

**Ce să demonstrezi:**
```
PID  STATE    CPU_TIME
---  -------  --------
0    READY    0 <--
1    READY    25        ← Proces user1 activ!
2    READY    18        ← Proces user2 activ!
```

**Arată:**
- ✅ **Două procese active** (cerință îndeplinită)
- ✅ **CPU_TIME diferit** → procese preemptate corect
- ✅ **Output intercalat** USER1/USER2 → fair scheduling

**Timp estimat: 10 secunde**

---

### 6️⃣ SYSCALLS → Demonstrație Funcționale

**Explică în timp ce procesele rulează:**

1. **sys_write (64)**: 
   - USER1 și USER2 printează → syscall write funcționează
   - Output: "USER1: Running...", "USER2: Working..."

2. **sys_yield (124)**:
   - Procesele cedează CPU-ul → smooth scheduling
   - Vezi cum se alternează output-ul

3. **sys_gettime (169)**:
   - USER1/USER2 afișează `[t=XXX]` → citesc tick counter-ul
   - Timp crește: `[t=000]` → `[t=025]` etc.

**Timp estimat: 15 secunde (observație)**

---

### 7️⃣ Process Management → Kill

```
kill 1
```

**Arată:**
- ✅ "Procesul 1 a fost terminat"

```
ps
```

**Verifică:**
```
PID  STATE    CPU_TIME
---  -------  --------
0    READY    0 <--
2    READY    45        ← Doar user2 mai rulează!
```

- ✅ Proces 1 dispărut
- ✅ Proces 2 continuă să ruleze

**Timp estimat: 15 secunde**

---

### 8️⃣ Heap Statistics → Verificare Finală

```
mem
```

**Arată:**
```
Memory Statistics (Heap):
  Total size:  134144532 bytes
  Used:        XXX bytes      ← A crescut față de început
  Free:        YYY bytes      ← A scăzut
  Blocks:      Z              ← Număr blocuri
```

**Demonstrează:**
- ✅ Heap allocator funcțional
- ✅ Memorie alocată pentru procese
- ✅ Tracking corect Used/Free

**Timp estimat: 10 secunde**

---

### 9️⃣ Ieșire

**Apasă:** `Ctrl+A` apoi `X` (sau Ctrl+C)

---

## 📊 Checklist Demonstrație

### Cerințe Dovedite:

- [x] **Boot complet** → "Booting...", "Hi Barbie!", banner shell
- [x] **Tick-uri periodice** → `[Uptime: 10s | Ticks: 1000]` la fiecare 10s
- [x] **Două procese active** → PID 1 și 2 în `ps`
- [x] **Preemptate corect** → CPU_TIME diferit, output intercalat
- [x] **Syscalls funcționale:**
  - [x] write(64) → USER1/USER2 printează
  - [x] yield(124) → smooth scheduling
  - [x] gettime(169) → `[t=XXX]` în output
- [x] **Heap allocator funcțional** → Test coalescing + `mem` stats

---

## 🎬 Script Verbal Sugerat

### Intro (30s)
> "Vă prezint OS-RISCV, un sistem de operare minimal pentru RISC-V RV32IMA. 
> Vom demonstra boot-ul, scheduling-ul preemptiv, syscall-urile și management-ul memoriei."

### Boot (10s)
> "Kernel-ul bootează, inițializează heap-ul cu coalescing, 
> configurează timer-ul și încarcă programele user din RAMFS."

### Shell (20s)
> "Avem un shell interactiv cu 6 comenzi: help, ps, uptime, mem, kill, exec.
> Timer-ul afișează timpul automat la fiecare 10 secunde."

### Exec (30s)
> "Lansez două procese user. Observați output-ul intercalat - 
> acesta demonstrează preemption-ul funcțional. Procesele folosesc 
> syscall-uri pentru write, yield și gettime."

### Verificare (20s)
> "Comanda ps arată ambele procese cu CPU time diferit. 
> Kill termină un proces. Mem arată statisticile heap-ului."

### Închidere (10s)
> "Sistemul implementează toate cerințele minime: boot, timer, 
> scheduling preemptiv, syscalls, memory management, shell și device I/O."

---

## 🔥 Tips pentru Demonstrație

1. **Pregătește ecranul** - Font mare, terminal full screen
2. **Explică vizual** - Indică cu mouse-ul output-ul important
3. **Repetă key points** - "Vedeți aici preemption-ul..."
4. **Gestionează timing** - Așteaptă tick-urile să apară
5. **Backup plan** - Dacă ceva nu merge, ai `./test_requirements.sh`

---

## 📸 Screenshots Recomandate

Dacă vrei să pregătești slides, ia screenshots la:
1. Boot banner cu SUCCESS coalescing
2. `[Uptime: 10s | Ticks: 1000]`
3. USER1/USER2 output intercalat
4. `ps` cu 2 procese active
5. `mem` statistics

---

## ⚡ Demo Express (3 minute)

Dacă ai timp limitat:

```bash
./run_shell.sh
# Așteaptă 10s → Tick apare
help
exec user1
exec user2
ps              # Arată 2 procese
# Observă output intercalat
kill 1
ps              # Arată 1 proces
Ctrl+A X
```

✅ **Gata! Toate cerințele dovedite în 3 minute!**
