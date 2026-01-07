#!/bin/bash
# Test automat pentru comenzile shell-ului

echo "Test automat shell OS-RISCV"
echo "============================"

# Creăm un fișier cu comenzi
cat > test_commands.txt << 'EOF'
help
ps
exec user1
ps
exec user2
ps
kill 3
ps
EOF

echo "Comenzi de test:"
cat test_commands.txt
echo ""
echo "Executare..."
echo ""

# Rulăm QEMU cu input din fișier și timeout
timeout 10 qemu-system-riscv32 -machine virt -bios none -kernel kernel.img -nographic -serial mon:stdio < test_commands.txt 2>&1 || true

rm -f test_commands.txt
echo ""
echo "Test complet!"
