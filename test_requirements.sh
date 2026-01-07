#!/bin/bash
# Test complet pentru cerințele OS

echo "=========================================="
echo "  Test Cerințe OS-RISCV"
echo "=========================================="
echo ""

cat > test_requirements.txt << 'EOF'
help
uptime
mem
ps
exec user1
exec user2
ps
uptime
mem
kill 1
ps
mem
EOF

echo "Comenzi de test:"
cat test_requirements.txt
echo ""
echo "Executare..."
echo "=========================================="
echo ""

timeout 12 qemu-system-riscv32 -machine virt -bios none -kernel kernel.img -nographic -serial mon:stdio < test_requirements.txt 2>&1 || true

rm -f test_requirements.txt
echo ""
echo "=========================================="
echo "Test complet!"
