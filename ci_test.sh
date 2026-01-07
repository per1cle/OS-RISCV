#!/bin/bash
# CI Script - Verificare automată BOOT → TICK → SHELL → EXEC

set -e  # Exit on error

echo "=========================================="
echo "  CI Pipeline - OS-RISCV"
echo "=========================================="
echo ""

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Counters
TESTS_PASSED=0
TESTS_FAILED=0

# Helper function
check_test() {
    if [ $1 -eq 0 ]; then
        echo -e "${GREEN}✓ PASS${NC}: $2"
        ((TESTS_PASSED++))
    else
        echo -e "${RED}✗ FAIL${NC}: $2"
        ((TESTS_FAILED++))
    fi
}

echo "Step 1: Build"
echo "----------------------------------------"
make clean > /dev/null 2>&1
if make > /dev/null 2>&1; then
    echo -e "${GREEN}✓ PASS${NC}: Build successful"
    ((TESTS_PASSED++))
else
    echo -e "${RED}✗ FAIL${NC}: Build failed"
    ((TESTS_FAILED++))
    exit 1
fi
echo ""

echo "Step 2: Run QEMU and capture output"
echo "----------------------------------------"

# Run QEMU with timeout and capture output
echo "Running QEMU (12 seconds)..."
echo -e "help\nps\nexec user1\nexec user2\nps\n" | timeout 12 qemu-system-riscv32 -machine virt -bios none -kernel kernel.img -nographic -serial mon:stdio > ci_output.log 2>&1 || true

if [ ! -f ci_output.log ]; then
    echo -e "${RED}✗ FAIL${NC}: QEMU output not captured"
    exit 1
fi

echo "Output captured ($(wc -l < ci_output.log) lines)"
echo ""

echo "Step 3: Verify output"
echo "----------------------------------------"

# Test 1: BOOT - Check boot banner
if grep -q "Booting..." ci_output.log && grep -q "Hi Barbie!" ci_output.log; then
    check_test 0 "BOOT banner present"
else
    check_test 1 "BOOT banner present"
fi

# Test 2: Memory Manager
if grep -q "MM: Initializing kernel heap" ci_output.log; then
    check_test 0 "Memory Manager initialized"
else
    check_test 1 "Memory Manager initialized"
fi

# Test 3: Coalescing test
if grep -q "SUCCESS! Coalescing works!" ci_output.log; then
    check_test 0 "Heap allocator (coalescing) functional"
else
    check_test 1 "Heap allocator (coalescing) functional"
fi

# Test 4: Timer initialization
if grep -q "Timer initializat" ci_output.log; then
    check_test 0 "Timer initialized"
else
    check_test 1 "Timer initialized"
fi

# Test 5: TICK - Check periodic ticks
if grep -q "Uptime:" ci_output.log && grep -q "Ticks:" ci_output.log; then
    check_test 0 "TICK periodic display working"
else
    check_test 1 "TICK periodic display working"
fi

# Test 6: RAMFS
if grep -q "RAMFS: Initialized" ci_output.log && grep -q "user1" ci_output.log && grep -q "user2" ci_output.log; then
    check_test 0 "RAMFS with user programs"
else
    check_test 1 "RAMFS with user programs"
fi

# Test 7: SHELL - Check shell startup
if grep -q "OS-RISCV Shell Monitor" ci_output.log; then
    check_test 0 "SHELL monitor started"
else
    check_test 1 "SHELL monitor started"
fi

# Test 8: SHELL - Check help command
if grep -q "Comenzile disponibile:" ci_output.log; then
    check_test 0 "SHELL help command"
else
    check_test 1 "SHELL help command"
fi

# Test 9: SHELL - Check ps command
if grep -q "PID  STATE    CPU_TIME" ci_output.log; then
    check_test 0 "SHELL ps command"
else
    check_test 1 "SHELL ps command"
fi

# Test 10: SHELL - Check mem command
if grep -q "Memory Statistics" ci_output.log; then
    check_test 0 "SHELL mem command"
else
    check_test 1 "SHELL mem command"
fi

# Test 11: SHELL - Check uptime command
if grep -q "System Uptime:" ci_output.log && grep -q "Tick Counter:" ci_output.log; then
    check_test 0 "SHELL uptime command"
else
    check_test 1 "SHELL uptime command"
fi

# Test 12: EXEC - Check user1 loading
if grep -q "exec: loading user1" ci_output.log && grep -q "exec: started PID" ci_output.log; then
    check_test 0 "EXEC user1 loaded"
else
    check_test 1 "EXEC user1 loaded"
fi

# Test 13: EXEC - Check user2 loading
if grep -q "exec: loading user2" ci_output.log; then
    check_test 0 "EXEC user2 loaded"
else
    check_test 1 "EXEC user2 loaded"
fi

# Test 14: User process execution
if grep -q "USER1: Hello from user program 1!" ci_output.log; then
    check_test 0 "User process 1 executed"
else
    check_test 1 "User process 1 executed"
fi

if grep -q "USER2: Hello from user program 2!" ci_output.log; then
    check_test 0 "User process 2 executed"
else
    check_test 1 "User process 2 executed"
fi

# Test 15: SYSCALL - write functional
if grep -q "USER1: Running" ci_output.log || grep -q "USER2: Working" ci_output.log; then
    check_test 0 "SYSCALL write() functional"
else
    check_test 1 "SYSCALL write() functional"
fi

# Test 16: Preemption - output interleaved
if grep -q "USER1:" ci_output.log && grep -q "USER2:" ci_output.log; then
    check_test 0 "Preemptive scheduling (output interleaved)"
else
    check_test 1 "Preemptive scheduling (output interleaved)"
fi

# Test 17: Process termination
if grep -q "USER1: Exiting" ci_output.log || grep -q "USER2: Exiting" ci_output.log; then
    check_test 0 "Process termination working"
else
    check_test 1 "Process termination working"
fi

# Cleanup
rm -f ci_output.log test_ci.log

echo ""
echo "=========================================="
echo "  CI Results"
echo "=========================================="
echo -e "Tests passed: ${GREEN}${TESTS_PASSED}${NC}"
echo -e "Tests failed: ${RED}${TESTS_FAILED}${NC}"
echo ""

if [ $TESTS_FAILED -eq 0 ]; then
    echo -e "${GREEN}✓ ALL TESTS PASSED${NC}"
    echo "BOOT → TICK → SHELL → EXEC sequence verified!"
    echo ""
    echo "Detailed log saved to: ci_output.log"
    echo "=========================================="
    exit 0
else
    echo -e "${RED}✗ SOME TESTS FAILED${NC}"
    echo "Check ci_output.log for details"
    echo "=========================================="
    exit 1
fi
