TARGET = riscv64-unknown-elf
CC = $(TARGET)-gcc
LD = $(TARGET)-ld
OBJCOPY = $(TARGET)-objcopy

# Flags pentru compilare
CFLAGS = -Wall -Werror -O0 -nostdlib -ffreestanding \
	 -mabi=ilp32 -march=rv32ima_zicsr -mcmodel=medany \
	 -Iheadere

# Flags pentru linking (fără -mcmodel)
LINK_FLAGS = -nostdlib -ffreestanding \
	     -mabi=ilp32 -march=rv32ima_zicsr

LDFLAGS = -T script/kernel.ld
USER_LDFLAGS = -T script/user.ld

# Kernel sources
SRC_S = src/boot/start.S src/kernel/comutare.S
SRC_C = src/kernel/main.c src/kernel/mm.c src/kernel/uart.c \
        src/kernel/interrupt.c src/kernel/timer.c \
        src/kernel_lib/kernel_lib.c src/kernel/proc.c \
        src/kernel/syscall.c src/kernel/loader.c src/kernel/shell.c

OBJS = $(SRC_S:.S=.o) $(SRC_C:.c=.o)

# User programs
USER_PROGS = user1.bin user2.bin
USER_OBJS = user1.o user2.o

all: kernel.img

# Build user programs
user1.bin: user/user1.c script/user.ld
	$(CC) $(CFLAGS) -T script/user.ld -nostartfiles user/user1.c -o user1.elf
	$(OBJCOPY) -O binary user1.elf user1.bin

user2.bin: user/user2.c script/user2.ld
	$(CC) $(CFLAGS) -T script/user2.ld -nostartfiles user/user2.c -o user2.elf
	$(OBJCOPY) -O binary user2.elf user2.bin

# Embed user programs as binary objects - CU FIX
user1.o: user1.bin
	$(LD) -m elf32lriscv -r -b binary user1.bin -o user1.o

user2.o: user2.bin
	$(LD) -m elf32lriscv -r -b binary user2.bin -o user2.o

# Kernel build
kernel.elf: $(OBJS) $(USER_OBJS)
	$(CC) $(LINK_FLAGS) $(LDFLAGS) -Wl,--no-warn-rwx-segments -o $@ $(OBJS) $(USER_OBJS)

kernel.img: kernel.elf
	$(OBJCOPY) --output-target binary $< $@

# Pattern rules
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
	rm -f $(OBJS) $(USER_OBJS) $(USER_PROGS) \
	      user1.elf user2.elf kernel.elf kernel.img