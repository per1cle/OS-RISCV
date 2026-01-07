#!/bin/bash
# Test timer display cu timp mai lung

echo "Test afișare timer (15 secunde)..."
echo "Observă: [Uptime: XXs | Ticks: YYYY] la fiecare 5 secunde"
echo ""

cat > timer_test.txt << 'EOF'
help
uptime
ps
mem
EOF

timeout 17 qemu-system-riscv32 -machine virt -bios none -kernel kernel.img -nographic -serial mon:stdio < timer_test.txt 2>&1 || true

rm -f timer_test.txt
echo ""
echo "Test complet!"
