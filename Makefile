CC ?= cc
CFLAGS ?= -O3
# Flags the build needs, kept apart so CFLAGS=... on the command line adds to them.
NRT_CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -fPIC -Iinclude $(CPPFLAGS) $(CFLAGS)
LDLIBS = -lm
PYTHON ?= python3
OBJS = build/neuronte.o build/utf8.o build/video.o build/bits.o

all: build/libneuronte.a build/libneuronte.so

build:
	mkdir -p build

build/%.o: src/%.c include/neuronte.h | build
	$(CC) $(NRT_CFLAGS) -c -o $@ $<

build/libneuronte.a: $(OBJS)
	$(AR) rcs $@ $^

build/libneuronte.so: $(OBJS)
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(LDLIBS)

build/test_%: tests/test_%.c build/libneuronte.a
	$(CC) $(NRT_CFLAGS) -o $@ $< build/libneuronte.a $(LDLIBS)

# In-process decoding (src/media.c) links FFmpeg 7.1's libraries, which
# libneuronte itself never does. It is optional: without it the player runs
# the ffmpeg command. Building it needs the libav*-dev packages.
PKG_CONFIG ?= pkg-config
MEDIA_PACKAGES = libavformat libavcodec libavfilter libavutil
MEDIA_CFLAGS ?= $(shell $(PKG_CONFIG) --cflags $(MEDIA_PACKAGES))
MEDIA_LIBS ?= $(shell $(PKG_CONFIG) --libs $(MEDIA_PACKAGES))

media: build/libneuronte-media.so

build/libneuronte-media.so: src/media.c include/neuronte_media.h | build
	$(CC) $(NRT_CFLAGS) $(MEDIA_CFLAGS) $(LDFLAGS) -shared -o $@ $< $(MEDIA_LIBS) -pthread -lm

# The whole shipped set: checksums, every face against its drawing, and every
# packed file's pixels against tests/golden.json.
test: all build/test_utf8 build/test_api
	./build/test_utf8
	./build/test_api weights/neuronte-split-v8.nrt --full --accent --pixel
	OPENBLAS_NUM_THREADS=1 $(PYTHON) tests/check_set.py build/libneuronte.so

# A line of text in the terminal, drawn by the desktop faces.
build/neuronte-say: demo/neuronte-say.c build/libneuronte.a
	$(CC) $(NRT_CFLAGS) -o $@ $< build/libneuronte.a $(LDLIBS)

demo: build/neuronte-say
	./build/neuronte-say -s 11 "The quick brown fox jumps over the lazy dog. 0123456789"
	./build/neuronte-say -s 11 -b "Bold is the same letter struck twice."
	./build/neuronte-say -s 16 -i "Italic, sheared on its own grid."
	./build/neuronte-say -s 20 -i "One net draws 17 to 32 px."
	./build/neuronte-say -s 24 "Neural nets as a font"

clean:
	rm -rf build

.PHONY: all media test demo clean

.PHONY: all media test clean
