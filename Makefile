all:
	mkdir -p kernel/lib/arch/memory/asm
	mkdir -p kernel/lib/cpu
	mkdir -p kernel/lib/proc/asm
	mkdir -p kernel/lib/interrupts/asm kernel/lib/interrupts/sycall kernel/lib/interrupts/syscall/asm
	mkdir -p kernel/lib/drivers/ps2/asm
	mkdir -p kernel/lib/protection kernel/lib/protection/asm
	cd gnu-efi/; make; make bootloader; cd ../; cd kernel; make; make buildimg

run:
	cd kernel/; make debug

debug:
	cd kernel/; make debug

dump:
	objdump -d -M intel kernel/bin/kernel.elf

burndanger:
	dd if=kernel/bin/HSCorpOS.img of=/dev/sdb status=progress
