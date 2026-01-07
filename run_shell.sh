#!/bin/bash
# Script pentru testare interactivă a shell-ului OS-RISCV

echo "==================================================="
echo "  OS-RISCV Shell - Testare Interactivă"
echo "==================================================="
echo ""
echo "Comenzi disponibile în shell:"
echo "  help          - Lista comenzi"
echo "  ps            - Afișează procese"
echo "  exec user1    - Lansează user1"
echo "  exec user2    - Lansează user2"
echo "  kill <pid>    - Termină proces"
echo ""
echo "Pentru a ieși din QEMU: Ctrl+A apoi X"
echo ""
echo "Pornire QEMU..."
echo "==================================================="
echo ""

qemu-system-riscv32 -machine virt -bios none -kernel kernel.img -nographic -serial mon:stdio
