PATCH:=data/vike_patch/vike_patch.plainbsdiff
PATCH_HEADER:=src/common/headers/vike_patch.h

all: $(PATCH) bin/g2hr_menu.exe bin/g2hr_native.exe bin/g2hr.exe bin/proxy.dll $(PATCH_HEADER)

Linux: $(PATCH) bin/g2hr_menu.exe bin/g2hr_native.bin bin/g2hr.exe bin/proxy.dll $(PATCH_HEADER)


# The four main components
bin/proxy.dll:
	$(MAKE) -C src/injected/

bin/g2hr.exe:
	$(MAKE) -C src/meta/

bin/g2hr_menu.exe:
	$(MAKE) -C src/menu/ Windows

bin/g2hr_menu.bin:
	$(MAKE) -C src/menu/ Linux

bin/g2hr_native.bin:
	$(MAKE) -C src/native/ Linux

bin/g2hr_native.exe:
	$(MAKE) -C src/native/ Windows

$(PATCH) $(PATCH_HEADER):
	$(MAKE) -C build/vike_patch


# installer

installer: all
	$(MAKE) -C src/installer/ force_repack
	$(MAKE) -C src/installer/


# Additional targets for easy debugging
run: bin/g2hr_menu.exe bin/g2hr.exe bin/g2hr_native.bin
	bin/g2hr.sh 640 480


# Run native.bin with gdb and print a stack trace, when it fails. The
# parameter makes the native component run the linux-binary (usually we
# run the windows binary!) with the same gdb options.
gdb-mixed: bin/g2hr_menu.bin bin/g2hr_meta.exe bin/g2hr.bin
	gdb -batch -ex run -ex bt --args bin/g2hr.bin \
		--menu-gdb-mixed

gdb-wine: bin/g2hr_menu.exe bin/g2hr_meta.exe bin/g2hr.bin
	gdb -batch -ex run -ex bt --args bin/g2hr.bin \
		--menu-gdb-wine


# Sniff registry changes performed by the GTA2 manager
# You should install the 'power patch' registry key, which will
# enable the debug mode ( http://gtamp.com/GTA2/powerpatch.zip )
regsniff: GTA2/gta2manager.exe
	cd GTA2 && WINEDEBUG=+reg wine gta2manager.exe

clean:
	rm bin/*.exe bin/*.bin || true
	rm build/wineprefix/drive_c/users/*/Application\ Data/bytewerk -r \
		|| true
	$(MAKE) -s -C src/menu clean
	
.PHONY: clean all bin/g2hr_menu.exe bin/g2hr.exe bin/g2hr_native.exe bin/g2hr_native.bin bin/proxy.dll regsniff winecfg installer
