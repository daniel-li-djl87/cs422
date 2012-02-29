# -*-Makefile-*-

KERN_SRCFILES	+= $(ARCH_DIR)/i386/entry.S
KERN_SRCFILES	+= $(ARCH_DIR)/i386/pcpu.c
KERN_SRCFILES	+= $(ARCH_DIR)/i386/pcpu_mp.c
KERN_SRCFILES	+= $(ARCH_DIR)/i386/mem.c
KERN_SRCFILES	+= $(ARCH_DIR)/i386/pmap.c
KERN_SRCFILES	+= $(ARCH_DIR)/i386/trapasm.S

KERN_BINFILES	+= $(ARCH_OBJDIR)/i386/boot_ap

# Targets

$(ARCH_OBJDIR)/i386/%.o: $(ARCH_DIR)/i386/%.c
	@echo + cc[KERN/i386] $<
	@mkdir -p $(@D)
	$(V)$(CC) $(KERN_CFLAGS) -c -o $@ $<

$(ARCH_OBJDIR)/i386/%.o: $(ARCH_DIR)/i386/%.S
	@echo + as[KERN/i386] $<
	@mkdir -p $(@D)
	$(V)$(CC) $(KERN_CFLAGS) -c -o $@ $<

$(ARCH_OBJDIR)/i386/boot_ap.o: $(ARCH_DIR)/i386/boot_ap.S
	@echo + as[KERN/i386] $<
	@mkdir -p $(@D)
	$(V)$(CC) $(KERN_CFLAGS) -c -o $@ $<

$(ARCH_OBJDIR)/i386/boot_ap: $(ARCH_OBJDIR)/i386/boot_ap.o
	@echo + ld[KERN/i386] $<
	$(V)$(LD) -nostdlib -m elf_i386 -e start_ap -N -Ttext 0x8000 -o $@.elf $^
	$(V)$(OBJDUMP) -S $@.elf >$@.asm
	$(V)$(OBJCOPY) -S -O binary $@.elf $@