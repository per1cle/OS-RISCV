# Checklist Cerințe OS-RISCV

## ✅ Target & Toolchain

- [x] **ISA**: RISC-V RV32IMA + Zicsr
- [x] **Platform**: QEMU qemu-system-riscv32 (virt machine)
- [x] **Boot**: Firmware stub simplu (fără multiboot/GRUB)
- [x] **Toolchain**: riscv64-unknown-elf-gcc, ld, objcopy, gdb

## ✅ Cerințe Funcționale Minime

### 1. Boot to Kernel
- [x] Kernel pornește corect din firmware
- [x] Ajunge în funcția `kmain()`
- [x] Boot banner afișat prin UART ("Booting...", "Hi Barbie!")

### 2. Interrupts & Timer
- [x] Implementare trap/interrupt vector funcțional (`handler` în start.S)
- [x] Configurare machine timer (CLINT) pentru tick periodic (~100Hz)
- [x] Demonstrație vizibilă: tick counter (`counter_tick`) crește constant
- [x] Output periodic: "Tick!" la fiecare 100 tick-uri
- [x] **Comandă shell**: `uptime` - afișează tick counter și uptime

### 3. Cooperative → Preemptive Scheduling
- [x] Implementare inițială cooperativă (yield())
- [x] Extindere la preemptive scheduling
- [x] Cel puțin două procese runnable (user1, user2)
- [x] Procese se execută alternativ
- [x] Preempția la fiecare tick de timer
- [x] Round-robin, time slice fix
- [x] Output intercalat demonstrează fair preemption

### 4. Memory Management
- [x] Implementare listă simplă de blocuri libere
- [x] Funcții: `kmalloc()` / `kfree()`
- [x] Coalescing (lipirea blocurilor adiacente)
- [x] Alignment 8 bytes
- [x] Demonstrație: alocări și eliberări în timpul execuției
- [x] Raportare memorie: test în boot + comandă `mem` în shell
- [x] Test coalescing în boot ("SUCCESS! Coalescing works!")

### 5. System Call ABI
- [x] **Syscall 64 (SYS_WRITE)**: write(fd=UART) pentru output
- [x] **Syscall 124 (SYS_YIELD)**: yield() pentru cedarea procesorului
- [x] **Syscall 169 (SYS_GETTIME)**: gettime() pentru citirea tick counter-ului
- [x] Demonstrație tranziție user → kernel → user
- [x] Wrapper-uri în user space
- [x] Handler în kernel (`syscall_handler()`)
- [x] Parametrii corecți prin registre (a0-a7)

### 6. Program Loading
- [x] Încărcare program ELF din RAMFS embedded
- [x] Kernel lansează cel puțin 2 programe user distincte (user1, user2)
- [x] user1 @ 0x80400000, user2 @ 0x80500000
- [x] Entry point corect (offset 0x1a8 de la bază)
- [x] Stack setup corect
- [x] Funcție `exec()` / `exec_program()`

### 7. Shell / Monitor (UART)
- [x] Interfață simplă pe UART cu prompt (`>`)
- [x] **Comandă `help`**: afișează lista comenzi
- [x] **Comandă `ps`**: afișează PID, state, CPU time
- [x] **Comandă `kill <pid>`**: termină un proces
- [x] **Comandă `exec <prog>`**: lansează un program user
- [x] **Comandă `uptime`**: afișează tick counter și uptime ✨ BONUS
- [x] **Comandă `mem`**: afișează statistici heap ✨ BONUS
- [x] Ieșire lizibilă prin terminalul QEMU
- [x] Parsing comenzi + argumente
- [x] Validare input (PID range, program name)

### 8. Basic Device I/O
- [x] Implementare driver UART polled mode
- [x] Funcții: `kernel_putchar()`, `kernel_putstring()`, `kernel_print_hex()`
- [x] Funcții citire: `kernel_getchar()` (blocking)
- [x] Funcții citire: `kernel_getchar_nonblock()` (non-blocking)
- [x] Integrare cu shell (polling în idle loop)

## ✅ Cerințe Non-Funcționale Minime

### Code Hygiene
- [x] Cod în C + Assembly
- [x] Compilare cu `-Wall -Werror` (fără warnings)
- [x] Cod formatat consistent
- [x] Comentarii clare în secțiuni critice

### Documentație
- [x] **README.md**: Overview, quick start, comenzi shell
- [x] **DOCUMENTATION.md**: Documentație tehnică completă
  - [x] Diagramă arhitecturală (straturi kernel, procese, drivere)
  - [x] Boot flow (succesiunea de inițializări)
  - [x] Scheduler & syscalls detaliat
  - [x] Memory management explicat
  - [x] Device I/O documentat
  - [x] Instrucțiuni de rulare (how to run)
  - [x] Instrucțiuni debugging (GDB)
- [x] **SHELL_README.md**: Ghid shell/monitor

### Demo Determinist
- [x] Boot banner consistent
- [x] Tick counter predictibil
- [x] Procese în ordine deterministă (user1, user2)
- [x] Scripts de test automate:
  - [x] `test_requirements.sh` - Test complet cerințe
  - [x] `test_shell.sh` - Test rapid shell
  - [x] `demo_shell.sh` - Demo scenarii
  - [x] `run_shell.sh` - Testare interactivă

## ✅ Demo Minim Obligatoriu

- [x] **Boot** → afișează banner ("Booting...", "Hi Barbie!")
- [x] **Tick counter** → afișează "Tick!" crescând periodic
- [x] **Shell** → prompt `>` funcțional
- [x] **exec u1 & exec u2** → lansează 2 procese
- [x] **ps** → se observă 2 procese runnable cu PID, state, CPU time
- [x] **Syscall write()** → ambele procese scriu intercalat
- [x] **Fair preemption** → output interleaved (USER1/USER2 alternează)
- [x] **kill <pid>** → reduce numărul de procese
- [x] **ps** confirmă → procesul dispare din listă
- [x] **Heap stats** → afișate la boot (test coalescing)
- [x] **mem** → statistici heap înainte și după execuție

### Test Complet
```bash
./test_requirements.sh
```

**Output așteptat**:
```
Booting... Hi Barbie!
MM: Initializing kernel heap...
--- Test 1-4 (Standard) ---
  > SUCCESS! Coalescing works!
Timer initializat
RAMFS: Initialized
  - user1, user2

Shell Monitor v1.0
> help
> uptime (Tick Counter: X)
> mem (Total/Used/Free)
> exec user1
> exec user2
> ps (2 procese active)
> kill 1
> ps (1 proces activ)

Tick! (periodic)
USER1: Running... [intercalat]
USER2: Working... [intercalat]
```

## ✅ Constrângeri

- [x] NU portare kernel existent (XV6, uCore) - cod propriu
- [x] Fără memorie dinamică de la host
- [x] Fără libc - bibliotecă minimală proprie (libk + libu)
- [x] Fără interfață grafică (GUI) - doar UART
- [x] Code documented (not copied)

## ✅ Testare Minimă

- [x] Build complet prin `make`
- [x] Run prin `make run` sau script
- [x] Log UART dovedește:
  - [x] Boot complet
  - [x] Tick-uri periodice
  - [x] Două procese active și preemptate corect
  - [x] Syscalls funcționale (write, yield, gettime)
  - [x] Heap allocator funcțional (coalescing)
  - [x] Shell funcțional (toate comenzile)

## ✅ Livrabile Finale

- [x] **Repository complet** cu cod sursă
- [x] **README.md** clar și concis
- [x] **DOCUMENTATION.md** - documentație tehnică:
  - [x] Arhitectura sistemului
  - [x] Scheduler & syscalls
  - [x] Management memorie
  - [x] Boot flow
  - [x] Device I/O
  - [x] Limitări și direcții de extindere
- [x] **Scripts de testare**:
  - [x] `test_requirements.sh`
  - [x] `test_shell.sh`
  - [x] `demo_shell.sh`
  - [x] `run_shell.sh`

## 🎁 Opțional (BONUS) - Implementat Parțial

- [ ] Separare user/kernel cu paginare Sv32
- [ ] Mecanism copy-on-write fork()
- [ ] Virtio-blk loader (citire read-only)
- [ ] Signals (SIGKILL, SIGTERM)
- [ ] Pipes între procese
- [ ] Mini VFS
- [ ] Script GDB pentru debugging remote
- [x] **Comanda `uptime`** în shell ✨
- [x] **Comanda `mem`** pentru statistici heap ✨

## 📊 Rezumat Final

### Cerințe Funcționale: **8/8** ✅ 100%
### Cerințe Non-Funcționale: **3/3** ✅ 100%
### Demo Minim: **Complet** ✅
### Testare: **Completă** ✅
### Documentație: **Completă** ✅

---

## 🎯 Status Final

**TOATE CERINȚELE MINIME SUNT IMPLEMENTATE ȘI FUNCȚIONALE** ✅

**Bonusuri adăugate**:
- ✨ Comandă `uptime` pentru monitoring sistem
- ✨ Comandă `mem` pentru statistici heap detaliate
- ✨ Documentație extinsă (DOCUMENTATION.md, SHELL_README.md)
- ✨ Suite completă de teste automate

**Sistem gata de demonstrație și evaluare!** 🚀

---

**Ultima verificare**: Ianuarie 2026  
**Build status**: ✅ Compilează fără warnings  
**Test status**: ✅ Toate testele trec  
**Documentation**: ✅ Completă
