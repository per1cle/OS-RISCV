#!/bin/bash
{
    sleep 2
    printf "help\n"
    sleep 2
    printf "ps\n"
    sleep 2
    printf "uptime\n"
    sleep 2
    printf "mem\n"
    sleep 2
    printf "exec user1\n"
    sleep 3
    printf "exec user2\n"
    sleep 7         
    printf "ps\n"
    sleep 2
    printf "uptime\n"
    sleep 2
    printf "kill 1\n"
    sleep 10        
    printf "ps\n"
    sleep 2
    printf "kill 2\n"
    sleep 2
    printf "ps\n"
    sleep 2
    printf "mem\n"
    sleep 2
    printf "uptime\n"
    sleep 2
   
    printf "\x01x"
} | timeout 60 qemu-system-riscv32 -machine virt -bios none -kernel kernel.img -nographic -serial mon:stdio 2>&1 || true

