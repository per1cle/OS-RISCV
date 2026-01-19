# Documentatie Tehnica OS-RISCV

Acest document descrie arhitectura si implementarea unui sistem de operare minimal pentru arhitectura RISC-V (32-bit), dezvoltat pentru masina virtuala QEMU `virt`. Sistemul implementeaza multiprogramare preemptiva, management de memorie si un shell interactiv.

## 1. Arhitectura Generala

Sistemul ruleaza in Machine Mode (M-Mode), cel mai inalt nivel de privilegii pe RISC-V, avand acces direct la resursele hardware.

- **Platforma**: QEMU RISC-V 32-bit Virt Machine
- **Privilegii**: Machine Mode (fara separare Kernel/User space prin PMP/Virtual Memory momentan, user space-ul este emulat logic)
- **Interrupt Controller**: CLINT (Core Local Interruptor) pentru timer interrupts

## 2. Boot si Initializare

Procesul de boot incepe in fisierul `src/boot/start.S`.

1.  **Reset Vector**: QEMU sare la adresa de start a kernelului (`0x80000000`).
2.  **Initializare Stack**: Registrul `sp` (Stack Pointer) este setat la `stack_top` (definit in linker script).
3.  **Setup Interrupts**: Adresa functiei `handler` (trap vector) este scrisa in registrul `mtvec`.
4.  **BSS Clear**: Sectiunea `.bss` este initializata cu zero.
5.  **Salt la C**: Se apeleaza functia `kmain` din `src/kernel/main.c`.

In `kmain`, sistemul initializeaza subsistemele in ordine:
- `uart_init()`: Driver serial pentru consola.
- `mm_init()`: Initializare alocator de memorie.
- `timer_init()`: Programare timer CLINT pentru tick-ul sistemului.
- `sched_init()`: Initializare structuri planificator.

## 3. Managementul Memoriei

Memoria RAM este mapata incepand de la adresa fizica `0x80000000`. Dimensiunea totala utilizata este de 128MB.

### Layout Memorie
Organizarea memoriei este definita in `script/kernel.ld`:

- **Text (Code)**: Incepe la `0x80000000`.
- **Data/BSS**: Urmeaza dupa sectiunea text.
- **Kernel Heap**: Gestioneaza memoria dinamica ramasa dupa sectiunile statice.
- **User Programs**: Incarcate la adrese fixe (ex: `0x80400000` pentru user1, `0x80500000` pentru user2) de catre loader.

### Alocator (Heap)
Implementarea se afla in `src/kernel/mm.c`.
- **Algoritm**: First-Fit.
- **Coalescing**: La eliberarea unui bloc (`kfree`), se verifica blocurile adiacente. Daca sunt libere, sunt unite pentru a reduce fragmentarea.

## 4. Gestiunea Proceselor si Planificator

Sistemul foloseste un planificator Round-Robin cu preemptiune bazata pe timer.

### Structura Procesului (PCB)
Fiecare proces este definit de o structura care contine:
- `pid`: Identificator unic.
- `state`: Starea curenta (READY, RUNNING, SLEEPING, TERMINATED).
- `sp`: Stack Pointer-ul kernel-ului pentru contextul salvat.
- `context`: Valoarea registrelor salvate (ra, sp, s0-s11).
- `cpu_time`: Timpul total de executie (contorizat in tick-uri).

### Scheduling si Context Switch
Fisierul `src/kernel/comutare.S` si `src/kernel/proc.c` gestioneaza schimbarea contextului.

1.  **Voluntary Switch (Yield)**: Procesul apeleaza `yield()`. Kernelul salveaza registrele callee-saved si schimba stack stack-ul.
2.  **Involuntary Switch (Preemption)**:
    - Timer-ul genereaza o intrerupere.
    - Se intra in `handler` (in `start.S`).
    - Se salveaza toate registrele (Trap Frame) pe stiva curenta.
    - Se apeleaza `handler_c` -> `tick()`.
    - Daca `tick()` decide ca e nevoie de reschedulare (`need_resched = 1`), se apeleaza `preempt_from_handler`.
    - `preempt_from_handler` salveaza pointerul de stiva al procesului intrerupt si sare in bucla planificatorului.

**Nota Importanta**: Pentru a permite preemtiunea codului userspace, procesele sunt lansate printr-un `entry_point_wrapper` care activeaza intreruperile (bitul MPIE/MIE din mstatus) inainte de a sari la codul utilizator.

## 5. System Calls (Apeluri de Sistem)

Interactiunea dintre programele utilizator si kernel se face prin instructiunea `ecall`.

- **Mecanism**: `ecall` genereaza o exceptie sincrona.
- **Parametri**: Numarul apelului in `a7`, argumentele in `a0`, `a1`, `a2`.
- **Implementare**: `syscall() ` in `src/kernel/syscall.c`.

Apeluri suportate:
| ID | Nume | Descriere |
|----|------|-----------|
| 1 | WRITE | Scrie caractere la consola |
| 2 | EXIT | Termina procesul curent |
| 3 | YIELD | Cedeaza procesorul voluntar |
| 4 | GETTIME| Returneaza timpul sistemului (uptime) |

## 6. Utilizare si Testare

### Cerinte
- Toolchain: `riscv64-unknown-elf-gcc`
- Emulator: `qemu-system-riscv32`

### Compilare si Rulare

Pentru a compila proiectul:
```bash
make
```

Pentru a rula in QEMU:
```bash
make run
```

### Comenzi Shell
Kernelul porneste un shell interactiv pe UART. Comenzi disponibile:
- `help`: Afiseaza lista comenzilor.
- `ps`: Afiseaza procesele active (PID, stare, timp CPU).
- `msg`: Printeaza un mesaj de test.
- `uptime`: Afiseaza timpul de la pornire.
- `kill <pid>`: Opreste fortat un proces.
- `exec <nume>`: Porneste un program utilizator (ex: `exec user1`).

Un scenariu tipic de testare a multiprogramarii:
```bash
exec user1
exec user2
ps
kill 1
```
Aceasta secventa porneste doua procese care ruleaza in paralel (afisand mesaje intercalate), apoi il opreste pe primul, demonstrand independenta executiei si stabilitatea planificatorului.
