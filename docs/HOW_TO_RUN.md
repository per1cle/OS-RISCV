# 🚀 How to Run - OS-RISCV

## 📋 Prerequisites

### Required Tools

```bash
# RISC-V Toolchain
riscv64-unknown-elf-gcc
riscv64-unknown-elf-ld
riscv64-unknown-elf-objcopy

# QEMU
qemu-system-riscv32

# Build tools
make
```

### Installation (Ubuntu/Debian)

```bash
# Install RISC-V toolchain
sudo apt-get install gcc-riscv64-unknown-elf

# Install QEMU
sudo apt-get install qemu-system-misc

# Verify installation
riscv64-unknown-elf-gcc --version
qemu-system-riscv32 --version
```

---

## 🔨 Building

### Quick Build

```bash
# Clean and build
make clean && make
```

### Build Output

Expected files after successful build:
```
kernel.img      # Final kernel binary (to run in QEMU)
kernel.elf      # Kernel ELF (for debugging)
user1.elf       # User program 1 ELF
user2.elf       # User program 2 ELF
user1.bin       # User program 1 raw binary
user2.bin       # User program 2 raw binary
*.o files       # Object files
```

### Compilation Flags

```makefile
CFLAGS = -Wall -Werror -O0 -nostdlib -ffreestanding \
         -mabi=ilp32 -march=rv32ima_zicsr -mcmodel=medany
```

- `-Wall -Werror`: All warnings as errors
- `-O0`: No optimization (for debugging)
- `-nostdlib`: No standard library
- `-ffreestanding`: Freestanding environment
- `-mabi=ilp32`: 32-bit integer ABI
- `-march=rv32ima_zicsr`: RV32I + M(mul) + A(atomic) + Zicsr

---

## ▶️ Running

### Method 1: Make Run (Recommended)

```bash
make run
```

This executes:
```bash
qemu-system-riscv32 -M virt -kernel kernel.img -nographic -bios none
```

### Method 2: Manual QEMU

```bash
qemu-system-riscv32 \
    -M virt \
    -kernel kernel.img \
    -nographic \
    -bios none
```

### QEMU Parameters Explained

| Parameter | Description |
|-----------|-------------|
| `-M virt` | Use RISC-V 'virt' machine (has UART, CLINT, etc.) |
| `-kernel kernel.img` | Load our kernel binary |
| `-nographic` | No graphical window, use terminal |
| `-bios none` | Don't load default BIOS/firmware |

---

## 🎮 Using the System

### After Boot

You should see:
```
Booting...
Hi Barbie!
MM: Initializing kernel heap...
...
========================================
  OS-RISCV Shell Monitor v1.0
========================================
Tastati 'help' pentru lista de comenzi

>
```

### Available Commands

```bash
help          # Show command list
ps            # Show running processes
uptime        # Show system uptime and tick counter
mem           # Show heap statistics
kill <pid>    # Terminate process
exec <prog>   # Launch program (user1 or user2)
```

### Example Session

```bash
> help
Comenzile disponibile:
  help          - Afiseaza acest mesaj
  ps            - Afiseaza procesele active
  uptime        - Afiseaza tick counter si uptime
  mem           - Afiseaza statistici memorie (heap)
  kill <pid>    - Termina procesul cu PID-ul specificat
  exec <prog>   - Lanseaza un program (user1/user2)

> ps
PID  STATE    CPU_TIME
---  -------  --------
0    READY    0 <--

> exec user1
Lansare user1...
exec: loading user1...
exec: started PID 1

> USER1: Hello from user program 1!
USER1: Running...  [t=000]
USER1: Running...  [t=000]

> exec user2
Lansare user2...
exec: started PID 2

> USER2: Hello from user program 2!
USER1: Running...  [t=001]
USER2: Working... [t=001]
USER1: Running...  [t=001]
USER2: Working... [t=001]

> ps
PID  STATE    CPU_TIME
---  -------  --------
0    READY    0 <--
1    READY    125
2    READY    118

> kill 1
Process 1 killed

> ps
PID  STATE    CPU_TIME
---  -------  --------
0    READY    0 <--
2    READY    218

> mem
Memory Statistics (Heap):
  Total size:  134144532 bytes
  Used:        116 bytes
  Free:        134144416 bytes
  Blocks:      2
```

---

## ⏹️ Exiting QEMU

### Exit Commands

```
Ctrl+A, X       # Kill QEMU
Ctrl+A, C       # QEMU monitor
  > quit        # Quit from monitor
Ctrl+C          # Force terminate (may not work)
```

**Recommended**: Use `Ctrl+A` then `X`

---

## 🧪 Demo Minim Obligatoriu

### Script Complet Demo

```bash
# 1. Build
make clean && make

# 2. Run
make run

# În shell:

# 3. Verificare boot
# Așteaptă să apară:
#   - "Booting..."
#   - "SUCCESS! Coalescing works!"
#   - Shell prompt ">"

# 4. Wait 10 seconds
# Trebuie să apară: [Uptime: 10s | Ticks: 1000]

# 5. Test heap ÎNAINTE de procese
mem

# 6. Launch procese
exec user1
exec user2

# 7. Verificare procese active
ps
# Trebuie să vezi PID 1 și 2

# 8. Observă output intercalat (fair preemption)
# USER1 și USER2 se alternează perfect

# 9. Kill un proces
kill 1

# 10. Verificare proces eliminat
ps
# Doar PID 2 mai rulează

# 11. Test heap DUPĂ exec
mem
# Used memory a crescut

# 12. Exit
# Ctrl+A, X
```

### Output Așteptat - Demo Complet

Vezi [DEMO_GUIDE.md](DEMO_GUIDE.md) pentru script complet de demonstrație.

---

## 🐛 Troubleshooting

### Problem: "command not found: qemu-system-riscv32"

**Solution**:
```bash
sudo apt-get install qemu-system-misc
# or
sudo apt-get install qemu-system
```

### Problem: "riscv64-unknown-elf-gcc: not found"

**Solution**:
```bash
sudo apt-get install gcc-riscv64-unknown-elf
# or build from source
```

### Problem: System hangs at boot

**Debug**:
```bash
# Add debug output
qemu-system-riscv32 -M virt -kernel kernel.img -nographic -bios none -d int,cpu_reset
```

### Problem: No output after boot

**Check**:
1. UART initialization
2. Trap handler setup
3. Stack pointer configuration

---

## 📊 Testing

### Unit Tests (Automated)

```bash
# Test allocator
./test_mm.sh

# Test timer
./test_timer.sh

# Test preemption
./test_preemption.sh

# Test all requirements
./test_requirements.sh
```

### Manual Verification

See [TESTING_CHECKLIST.md](TESTING_CHECKLIST.md) for complete checklist.

---

## 🔧 Advanced Options

### Debug Mode

```bash
# Run with GDB server
qemu-system-riscv32 -M virt -kernel kernel.img -nographic -bios none -s -S

# In another terminal:
riscv64-unknown-elf-gdb kernel.elf
(gdb) target remote :1234
(gdb) continue
```

### Trace Execution

```bash
# Trace interrupts
qemu-system-riscv32 -M virt -kernel kernel.img -nographic -bios none -d int

# Trace all
qemu-system-riscv32 -M virt -kernel kernel.img -nographic -bios none -d in_asm,int,cpu
```

### Serial Log to File

```bash
qemu-system-riscv32 -M virt -kernel kernel.img -nographic -bios none \
    -serial file:output.log
```

---

## 📝 Quick Reference

| Task | Command |
|------|---------|
| Build | `make` |
| Clean | `make clean` |
| Run | `make run` |
| Rebuild | `make clean && make` |
| Full test | `make && make run` |
| Exit QEMU | `Ctrl+A, X` |

---

## ✅ Verificare Build Success

După `make`:
```
✓ No warnings
✓ No errors
✓ kernel.img created
✓ user1.elf, user2.elf created
```

După `make run`:
```
✓ "Booting..." appears
✓ "SUCCESS! Coalescing works!"
✓ "Timer initializat"
✓ Shell prompt ">" appears
✓ Commands work (help, ps, etc.)
✓ Tick counter visible after 10s
```

---

## 🚀 Ready to Go!

System complet funcțional și gata de demonstrat! 🎉

Pentru demo detaliat, vezi:
- [DEMO_GUIDE.md](DEMO_GUIDE.md) - Script de demonstrație pas cu pas
- [TESTING_CHECKLIST.md](TESTING_CHECKLIST.md) - Checklist complet verificare
- [TRUE_PREEMPTIVE_IMPLEMENTATION.md](TRUE_PREEMPTIVE_IMPLEMENTATION.md) - Detalii preemption
