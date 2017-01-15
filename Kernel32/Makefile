########################################################
# 빌드 환경 및 규칙 설정
########################################################
NASM32 = nasm
GCC32 = /usr/cross/bin/x86_64-pc-linux-gcc -c -m32 -ffreestanding
LD32 = /usr/cross/bin/x86_64-pc-linux-ld -melf_i386 -T ../elf_x86_64.x -nostdlib -e Main -Ttext 0x10200
OBJCOPY32 = /usr/cross/bin/x86_64-pc-linux-objcopy -j .text -j .data -j .rodata -j .bss -S -O binary

OBJECTDIR = obj
SOURCEDIR = src

########################################################
# 빌드 항목 및 빌드 방법 설정
########################################################
all : prepare Kernel32.bin

prepare:
	mkdir -p $(OBJECTDIR)

$(OBJECTDIR)/EntryPoint.bin: $(SOURCEDIR)/EntryPoint.s
	$(NASM32) -o $@ $<

dep:
	@echo === Make Dependancy File ========
	make -C $(OBJECTDIR) -f ../Makefile InternalDependency
	@echo === Dependancy Serch Complete ===

ExecuteInternalBuild: dep
	make -C $(OBJECTDIR) -f ../Makefile Kernel32.elf

$(OBJECTDIR)/Kernel32.elf.bin: ExecuteInternalBuild
	$(OBJCOPY32) $(OBJECTDIR)/Kernel32.elf $@



Kernel32.bin: $(OBJECTDIR)/EntryPoint.bin $(OBJECTDIR)/Kernel32.elf.bin
	cat $^ > $@

clean:
	rm -f *.bin
	rm -f $(OBJECTDIR)/*.*

########################################################
# Make 에 의해 다시 호출되는 부분, obj 디렉터리를 기준으로 수행
########################################################
CENTRYPOINTOBJECT = Main.o
CSOURCE = $(wildcard ../$(SOURCEDIR)/*.c)
ASSEMBLYSOURCE = $(wildcard ../$(SOURCEDIR)/*.asm)
COBJECT = $(subst Main.o, , $(notdir $(patsubst %.c,%.o,$(CSOURCE))))
ASSEMBLYOBJECT = $(notdir $(patsubst %.asm,%.o,$(ASSEMBLYSOURCE)))

%.o: ../$(SOURCEDIR)/%.c
	$(GCC32) -c $<

%.o: ../$(SOURCEDIR)/%.asm
	$(NASM32) -f elf32 -o $@ $<

InternalDependency:
	$(GCC32) -MM $(CSOURCE) > Dependency.dep
	
Kernel32.elf: $(CENTRYPOINTOBJECT) $(COBJECT) $(ASSEMBLYOBBJECT)
	$(LD32) -o $@ $^

ifeq (Dependency.dep, $(wildcard Dependency.dep))
include Dependency.dep
endif


