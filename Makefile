TARGET = riscv64-unknown-elf
CC = $(TARGET)-gcc
LD = $(TARGET)-gcc
OBJCOPY = $(TARGET)-objcopy

CFLAGS = -Wall -Werror -O0 -nostdlib -ffreestanding \
	 -mabi=ilp32 -march=rv32ima_zicsr -mcmodel=medany \
	 -Iheadere

LDFLAGS = -T script/kernel.ld
SRC_S = src/boot/start.S src/kernel/comutare.S
SRC_C = src/kernel/main.c src/kernel/uart.c src/kernel/interrupt.c src/kernel/timer.c src/kernel_lib/kernel_lib.c \
	  src/kernel/proc.c
	  
OBJS = $(SRC_S:.S=.o) $(SRC_C:.c=.o)

all: kernel.img

kernel.elf: $(OBJS)
	$(LD) $(CFLAGS) $(LDFLAGS) -o $@ $^

kernel.img: kernel.elf
	$(OBJCOPY) --output-target binary $< $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	/usr/bin/qemu-system-riscv32 \
		-M virt \
		-kernel kernel.img \
		-nographic \
		-bios none

clean:
	rm -f $(OBJS) kernel.elf kernel.img
