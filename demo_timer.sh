#!/bin/bash
# Demo pentru afișarea timer-ului îmbunătățit

echo "=========================================="
echo "  Demo Timer Îmbunătățit"
echo "=========================================="
echo ""
echo "Timer-ul afișează automat timpul la fiecare 10 secunde:"
echo "  Format: [Uptime: XXm YYs | Ticks: ZZZZ]"
echo ""
echo "Rulare shell pentru ~30 secunde..."
echo "Poți tasta comenzi în timpul rulării!"
echo ""
echo "Apasă Ctrl+C pentru a opri."
echo "=========================================="
echo ""

# Rulează fără timeout pentru testare interactivă
qemu-system-riscv32 -machine virt -bios none -kernel kernel.img -nographic -serial mon:stdio
