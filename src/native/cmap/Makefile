COMPONENT := cmap
LIBS:=sdl2

run: ../out/$(COMPONENT)_test.bin
	cd ../../../ &&  src/native/out/$(COMPONENT)_test.bin


../out/$(COMPONENT)_test.bin: *.c *.h
	mkdir -p ../out/
	LANG=C gcc -std=c99 -Wall -Werror -g \
		`pkg-config --cflags --libs $(LIBS)` -o $@ *.c \
			../../common/ini/ini.c ../../common/fs/fs.c \
			../../common/cstr/cstr.c


valgrind: ../out/$(COMPONENT)_test.bin
	cd ../../../ && valgrind --leak-check=full --show-leak-kinds=all \
	 src/native/out/$(COMPONENT)_test.bin
