all: BootLoader Kernel32 Kernel64 Disk.img 

BootLoader:
	@echo
	@echo ==================== Build Boot Loader ====================
	@echo
	
	make -C BootLoader
	
	@echo
	@echo ===================== Build Complete ======================
	@echo

Kernel32:
	@echo
	@echo ===================== Build 32 Kernel =====================
	@echo
	
	make -C Kernel32
	
	@echo
	@echo ====================== Build Complete =====================
	@echo

Kernel64:
	@echo
	@echo ===================== Build 64 Kernel =====================
	@echo
	
	make -C Kernel64
	
	@echo
	@echo ====================== Build Complete =====================
	@echo


Disk.img: BootLoader/BootLoader.bin Kernel32/Kernel32.bin Kernel64/Kernel64.bin
	@echo
	@echo ================= Disk Image Build Start ==================
	@echo

	Utility/ImageMaker/ImageMaker.exe $^

	@echo
	@echo ================== All Build complete =====================
	@echo

clean:
	make -C BootLoader clean
	make -C Kernel32 clean
	make -C Kernel64 clean
	rm -f Disk.img

