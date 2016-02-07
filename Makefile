C_SOURCES = $(shell find . -path '*src/usr*' -prune -o -name "*.c" -print)
C_OBJECTS = $(patsubst %.c, %.o, $(C_SOURCES))
S_SOURCES = $(shell find . -path '*src/usr*' -prune -o -name "*.s" -print)
S_OBJECTS = $(patsubst %.s, %.o, $(S_SOURCES))

USER_DEP = ./src/usr/lib/printf.o ./src/lib/string.o \
		   ./src/usr/syscall.o ./src/usr/syscall_s.o \

USER_PROG = ./src/usr/init ./src/usr/sh ./src/usr/ls \
			./src/usr/cat ./src/usr/mkdir ./src/usr/rm

USER_OBJS := $(USER_PROG:%=%.o)

CC = gcc
LD = ld
ASM = nasm

C_FLAGS = -c -Werror -m32 -ggdb -gstabs+ -nostdinc -fno-builtin -fno-stack-protector -I src/include
LD_FLAGS = -T scripts/kernel.ld -m elf_i386 -nostdlib
ASM_FLAGS = -f elf -g -F stabs

all: $(S_OBJECTS) $(C_OBJECTS) $(USER_OBJS) $(USER_DEP) link  $(USER_PROG) update_image

# The automatic variable `$<' is just the first prerequisite
%.o: %*.c
	@echo 编译代码文件 $< ...
	$(CC) $(C_FLAGS) $< -o $@

%.o: %*.s
	@echo 编译汇编文件 $< ...
	$(ASM) $(ASM_FLAGS) $<


link:
	@echo 链接内核文件...
	$(LD) $(LD_FLAGS) $(S_OBJECTS) $(C_OBJECTS) -o turinux_kernel

$(USER_PROG): $(USER_OBJS) $(USER_DEP)
	$(LD) -m elf_i386 -nostdlib -e main $@.o $(USER_DEP) -o $@


.PHONY:clean
clean:
	$(RM) $(S_OBJECTS) $(C_OBJECTS) turinux_kernel
	$(RM) $(USER_OBJS) $(USER_PROG) $(USER_DEP)

.PHONY:update_image
update_image:
	sudo mount floppy.img /mnt
	sudo cp turinux_kernel /mnt/kernel
	sleep 1
	sudo umount /mnt
	sudo mount minix.img /mnt
	sudo cp $(USER_PROG) /mnt
	sudo cp ./icon /mnt
	sleep 1
	sudo umount /mnt

.PHONY:mount_image
mount_image:
	sudo mount floppy.img /mnt

.PHONY:umount_image
umount_image:
	sudo umount /mnt

.PHONY:qemu
qemu:
	qemu-system-i386 -fda floppy.img -hda minix.img

.PHONY:debug
debug:
	qemu -S -s -fda floppy.img -boot a &
	sleep 1
	cgdb -x scripts/gdbinit

