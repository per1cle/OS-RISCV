#!/bin/bash

echo "========================================="
echo "Test Preemption la Fiecare Tick"
echo "========================================="
echo ""
echo "Testăm că procesele sunt preemptate"
echo "la fiecare tick (~10ms) nu doar la yield()"
echo ""

timeout 15s qemu-system-riscv32 \
    -M virt \
    -kernel kernel.img \
    -nographic \
    -bios none <<EOF
exec user1
exec user2
EOF

echo ""
echo "========================================="
echo "Test complet!"
echo ""
echo "Verifică în log că:"
echo "  ✓ USER1 și USER2 se alternează FOARTE des"
echo "  ✓ Preemption automată la fiecare tick"
echo "  ✓ Nu mai depind de sys_yield() manual"
echo "========================================="
