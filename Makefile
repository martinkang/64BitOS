all: BootLoader Kernel32 Disk.img 

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

Disk.img: BootLoader/BootLoader.bin
	@echo
	@echo ================= Disk Image Build Start ==================
	@echo
	cp BootLoader/BootLoader.bin Disk.img
	@echo
	@echo ================== All Build complete =====================
	@echo

clean:
	make -C BootLoader clean
	rm -f Disk.img

