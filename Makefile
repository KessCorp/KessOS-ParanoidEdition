all:
	mkdir -p kernel/lib/arch/memory/x86_64
	mkdir -p kernel/lib/proc/x86_64
	mkdir -p kernel/lib/interrupts/x86_64 kernel/lib/interrupts/sycall kernel/lib/interrupts/syscall/x86_64
	mkdir -p kernel/lib/drivers/ps2/x86_64
	cd gnu-efi/; make; make bootloader; cd ../; cd kernel; make; make buildimg

run:
	cd kernel/; make run

debug:
	cd kernel/; make debug

dump:
	objdump -d -M intel kernel/bin/kernel.elf

burndanger:
	dd if=kernel/bin/HSCorpOS.img of=/dev/sdb status=progress
