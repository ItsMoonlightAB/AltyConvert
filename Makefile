TARGET_NAME = AltyConvert

CC = gcc
WINDRES = windres

CFLAGS = $(shell pkg-config --cflags gtk4) -Iinclude -O3
LIBS = $(shell pkg-config --libs gtk4) -lole32 -luuid -lshell32 -mwindows

SRC = src/main.c src/utils.c src/converter.c src/conv_audio.c src/conv_image.c src/conv_video.c
RES = build/resource.res

all: $(RES)
	mkdir -p build
	$(CC) $(SRC) $(RES) -o build/$(TARGET_NAME).exe $(CFLAGS) $(LIBS)
	@echo "-------------------------------------------"
	@echo "  Build completed : build/$(TARGET_NAME).exe"
	@echo "-------------------------------------------"

$(RES): resource.rc
	mkdir -p build
	$(WINDRES) resource.rc -O coff -o $(RES)

clean:
	rm -rf build/*.exe build/*.res