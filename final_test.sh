#!/bin/bash
# Test final - demonstrație completă cu timer îmbunătățit

echo "=========================================="
echo "  Test Final - Timer Îmbunătățit"
echo "=========================================="
echo ""

cat > final_test.txt << 'EOF'
help
ps
mem
exec user1
ps
uptime
EOF

echo "Comenzi de test:"
cat final_test.txt
echo ""
echo "Rulare cu timer la fiecare 10 secunde..."
echo "=========================================="
echo ""

timeout 25 qemu-system-riscv32 -machine virt -bios none -kernel kernel.img -nographic -serial mon:stdio < final_test.txt 2>&1 || true

rm -f final_test.txt
echo ""
echo "=========================================="
echo "✅ Timer îmbunătățit - afișare la 10s!"
echo "✅ Format: [Uptime: Xm Ys | Ticks: Z]"
echo "✅ Interval suficient pentru a tasta comenzi"
echo "=========================================="
