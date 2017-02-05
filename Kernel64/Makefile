########################################################
# 빌드 환경 및 규칙 설정
########################################################
NASM64 = nasm -f elf64
GCC64 = /usr/cross/bin/x86_64-pc-linux-gcc -c -m64 -ffreestanding
LD64 = /usr/cross/bin/x86_64-pc-linux-ld -melf_x86_64 -T ../elf_x86_64.x -nostdlib -e Main -Ttext 0x200000

# 바이너리 이미지 생성을 위한 OBJCOPY 옵션 정의
OBJCOPY64 = /usr/cross/bin/x86_64-pc-linux-objcopy -j .text -j .data -j .rodata -j .bss -S -O binary

OBJECTDIR = obj
SOURCEDIR = src

########################################################
# 빌드 항목 및 빌드 방법 설정
########################################################
all : prepare Kernel64.bin

# 오브젝트 파일이 위치할 디렉터리를 생성
prepare:
	mkdir -p $(OBJECTDIR)

# 커널의 C 소스 파일에 대한 의존성 정보 생성
dep:
	@echo === Make Dependancy File ========
	make -C $(OBJECTDIR) -f ../Makefile InternalDependency
	@echo === Dependancy Serch Complete ===

# 디렉터리를 오브젝트 파일 디렉터리로 이동해서 의존성 파일 및 실행 파일을 생성
ExecuteInternalBuild: dep
	make -C $(OBJECTDIR) -f ../Makefile Kernel64.elf

# 커널 이미지를 바이너리 파일로 변환하여 IA-32e 모드 바이너리 생성
Kernel64.bin: ExecuteInternalBuild
	$(OBJCOPY64) $(OBJECTDIR)/Kernel64.elf $@

clean:
	rm -f *.bin
	rm -f $(OBJECTDIR)/*.*

########################################################
# Make 에 의해 다시 호출되는 부분, obj 디렉터리를 기준으로 수행
########################################################
# 빌드할 어셈블리어 엔트리 포인트 소스 파일 정의, obj 디렉터리를 기준으로 설정
ENTRYPOINTSOURCEFILE = ../$(SOURCEDIR)/EntryPoint.s
ENTRYPOINTOBJECTFILE = EntryPoint.o

# 빌드할 C 소스 파일 정의
CSOURCE = $(wildcard ../$(SOURCEDIR)/*.c)
ASSEMBLYSOURCE = $(wildcard ../$(SOURCEDIR)/*.asm)
COBJECT = $(notdir $(patsubst %.c,%.o,$(CSOURCE))))
ASSEMBLYOBJECT = $(notdir $(patsubst %.asm,%.o,$(ASSEMBLYSOURCE)))

# 어셈블리어 엔트리 포인트 빌드
$(ENTRYPOINTOBJECTFILE): $(ENTRYPOINTSOURCEFILE)
	$(NASM64) -o $@ $<

# .c 파일을 .o 파일로 바꾸는 규칙 정의
%.o: ../$(SOURCEDIR)/%.c
	$(GCC64) -c $<

# .asm 파일을 .o 파일로 바꾸는 규칙 정의
%.o: ../$(SOURCEDIR)/%.asm
	$(NASM64) -o $@ $<

InternalDependency:
	$(GCC64) -MM $(CSOURCE) > Dependency.dep
	
Kernel64.elf: $(ENTRYPOINTOBJECTFILE) $(COBJECT) $(ASSEMBLYOBBJECT)
	$(LD64) -o $@ $^

ifeq (Dependency.dep, $(wildcard Dependency.dep))
include Dependency.dep
endif


