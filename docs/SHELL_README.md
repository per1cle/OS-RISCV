# OS-RISCV Shell Monitor

## Descriere

Shell/Monitor interactiv pentru OS-RISCV cu suport pentru gestionarea proceselor și lansarea programelor user.

## Caracteristici Implementate

### 1. Shell/Monitor pe UART
- ✅ Interfață simplă pe UART cu prompt `>`
- ✅ Citire input non-blocking (nu blochează scheduler-ul)
- ✅ Echo caractere, suport pentru backspace
- ✅ Parsing comenzi și argumente
- ✅ Output lizibil prin terminalul QEMU

### 2. Comenzi Disponibile

#### `help`
Afișează lista de comenzi disponibile.

```
> help
Comenzile disponibile:
  help          - Afiseaza acest mesaj
  ps            - Afiseaza procesele active
  uptime        - Afiseaza tick counter si uptime
  mem           - Afiseaza statistici memorie (heap)
  kill <pid>    - Termina procesul cu PID-ul specificat
  exec <prog>   - Lanseaza un program (user1/user2)
```

**Timer Automată**: Sistemul afișează automat timpul la fiecare 10 secunde:
```
[Uptime: 10s | Ticks: 1000]
[Uptime: 20s | Ticks: 2000]
[Uptime: 1m 05s | Ticks: 6500]
```

#### `ps`
Afișează procesele active cu PID, STATE și CPU_TIME.

```
> ps
PID  STATE    CPU_TIME
---  -------  --------
0    READY    0 <--
1    READY    15
2    READY    12
```

#### `kill <pid>`
Termină procesul cu PID-ul specificat.
- Validare PID (range 0-9)
- Protecție idle task (nu poți termina PID 0)
- Verificare existență proces

```
> kill 1
Procesul 1 a fost terminat
```

#### `exec <prog>`
Lansează un program user (user1 sau user2).
- Validare nume program
- Încărcare din RAMFS
- Creare proces nou

```
> exec user1
Lansare user1...
exec: loading user1...
exec: started PID 1
```

### 3. Device I/O - UART

#### Funcții Implementate

**Citire (Input):**
- `kernel_getchar()` - Blocking read
- `kernel_getchar_nonblock()` - Non-blocking read (returnează -1 dacă nu e input)

**Scriere (Output):**
- `kernel_putchar(char)` - Scrie un caracter
- `kernel_putstring(const char*)` - Scrie un string
- `kernel_print_hex(unsigned int)` - Afișează număr în hex

#### Registre UART (16550)
- `UART_THR` (0x10000000) - Transmit Holding Register
- `UART_RBR` (0x10000000) - Receive Buffer Register  
- `UART_LSR` (0x10000005) - Line Status Register
  - Bit 0: Data Ready
  - Bit 5: Transmit Holding Register Empty

### 4. Integrare cu Scheduler

Shell-ul rulează în mod polling, integrat cu scheduler-ul:
- `shell_init()` - Inițializare și afișare banner
- `shell_poll()` - Verificare input non-blocking (apelat din idle loop)
- Permite procesele user să ruleze în paralel cu shell-ul
- Output intercalat între shell și procese

## Utilizare

### Compilare

```bash
make clean && make
```

### Testare Automată

```bash
./test_shell.sh      # Test rapid
./demo_shell.sh      # Demo complet
```

### Testare Interactivă

```bash
./run_shell.sh
```

Pentru ieșire din QEMU: **Ctrl+A** apoi **X**

## Exemple

### Lansare și Gestionare Procese

```
> ps
PID  STATE    CPU_TIME
---  -------  --------
0    READY    0 <--

> exec user1
Lansare user1...
exec: loading user1...
exec: started PID 1

> exec user2
Lansare user2...
exec: loading user2...
exec: started PID 2

> ps
PID  STATE    CPU_TIME
---  -------  --------
0    READY    0 <--
1    READY    25
2    READY    18

> kill 1
Procesul 1 a fost terminat

> ps
PID  STATE    CPU_TIME
---  -------  --------
0    READY    0 <--
2    READY    32
```

## Arhitectură

```
main() 
  └─> init_timer()
  └─> init_ramfs()
  └─> init_proc()
  └─> shell_init()          # Afișează banner și prompt
  └─> scheduler()
        └─> while(1)
              ├─> shell_poll()      # Verifică input UART
              └─> comutare(proces)  # Schimbă la proces user
```

## Fișiere Modificate/Adăugate

### Noi:
- `src/kernel/shell.c` - Implementare shell
- `headere/shell.h` - Header shell
- `run_shell.sh` - Script testare interactivă
- `test_shell.sh` - Script testare automată
- `demo_shell.sh` - Script demo complet

### Modificate:
- `src/kernel_lib/kernel_lib.c` - Adăugat citire UART
- `headere/kernel_lib.h` - Declarații funcții UART
- `src/kernel/main.c` - Integrare shell
- `src/kernel/proc.c` - Adăugat shell_poll() în scheduler
- `src/kernel/loader.c` - Wrapper exec() pentru shell
- `headere/proc.h` - Export PROC_MAX

## Limitări Cunoscute

1. **Output Intercalat**: Când procesele user printează, output-ul se intercalează cu shell-ul
2. **Comenzi Simple**: Doar 4 comenzi de bază (extensibil)
3. **Erori de Input**: Caractere invalide sunt ignorate
4. **Buffer Fix**: Comenzi limitate la 128 caractere

## Extensii Posibile

- [ ] Comanda `clear` pentru curățare ecran
- [ ] Comanda `mem` pentru afișare memorie
- [ ] Istoricul comenzilor (săgeți sus/jos)
- [ ] Tab completion
- [ ] Pipe și redirectare
- [ ] virtio-block pentru încărcare binare din disc

## Status: ✅ COMPLET și FUNCȚIONAL
