#!/bin/bash
# Demo complet al shell-ului OS-RISCV

echo "=========================================="
echo "  OS-RISCV Shell - Demo Complet"
echo "=========================================="
echo ""

cat > demo_commands.txt << 'EOF'
help
ps
exec user1
exec user2
ps
kill 1
ps
EOF

echo "Scenariul demo:"
echo "  1. Afișare help"
echo "  2. Listare procese (gol)"
echo "  3. Lansare user1"
echo "  4. Lansare user2"
echo "  5. Listare procese (2 procese active)"
echo "  6. Terminare proces user1 (PID 1)"
echo "  7. Listare procese (doar user2)"
echo ""
echo "Executare..."
echo "=========================================="
echo ""

timeout 15 qemu-system-riscv32 -machine virt -bios none -kernel kernel.img -nographic -serial mon:stdio < demo_commands.txt 2>&1 || true

rm -f demo_commands.txt
echo ""
echo "=========================================="
echo "Demo complet!"
echo ""
