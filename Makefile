CC ?= cc
CFLAGS ?= -O3
# Flags the build needs, kept apart so CFLAGS=... on the command line adds to them.
NRT_CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -fPIC -Iinclude $(CPPFLAGS) $(CFLAGS)
LDLIBS = -lm
PYTHON ?= python3
OBJS = build/neuronte.o build/utf8.o build/video.o build/bits.o build/cells.o

all: build/libneuronte.a build/libneuronte.so build/neuronte-fontfile build/neuronte-cells

build:
	mkdir -p build

build/%.o: src/%.c include/neuronte.h | build
	$(CC) $(NRT_CFLAGS) -c -o $@ $<

build/libneuronte.a: $(OBJS)
	$(AR) rcs $@ $^

build/libneuronte.so: $(OBJS)
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(LDLIBS)

build/cells.o: include/neuronte_cells.h

build/test_%: tests/test_%.c build/libneuronte.a
	$(CC) $(NRT_CFLAGS) -o $@ $< build/libneuronte.a $(LDLIBS)

# The cells test again with every read and write of src/cells.c and of the
# test checked by AddressSanitizer and the undefined behaviour checks. The
# nets themselves are the library's ordinary build.
SANITIZE ?= -fsanitize=address,undefined -fno-sanitize-recover=all
build/test_cells_checked: tests/test_cells.c src/cells.c build/libneuronte.a include/neuronte_cells.h
	$(CC) -std=c11 -Wall -Wextra -Wpedantic -Iinclude $(CPPFLAGS) -O1 -g $(SANITIZE) -o $@ \
		tests/test_cells.c src/cells.c build/neuronte.o build/utf8.o $(LDLIBS)

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
# Then the font files: neuronte-setup makes them through its steps
# (tests/check_setup.sh), the generator makes them a second time with another
# number of threads, and tests/check_fontfile.py reads both and compares
# their bytes with tests/fontfile-golden.txt.
TEST_THREADS ?= 2
test: all build/test_utf8 build/test_api build/test_cells build/test_cells_checked
	./build/test_utf8
	./build/test_api weights/neuronte-split-v9.nrt --full --accent --pixel
	OPENBLAS_NUM_THREADS=1 $(PYTHON) tests/check_set.py build/libneuronte.so
	OPENBLAS_NUM_THREADS=1 $(PYTHON) tests/check_rank.py
	./build/test_cells weights/neuronte-split-v9.nrt weights/neuronte-term-dingbat-v1.nrt build/cells-test
	./build/test_cells_checked weights/neuronte-split-v9.nrt weights/neuronte-term-dingbat-v1.nrt build/cells-test
	sh tests/check_setup.sh build $(TEST_THREADS)
	./build/neuronte-fontfile --weights weights --out build/setup-test/again --threads 3 --no-fc-cache
	OPENBLAS_NUM_THREADS=1 $(PYTHON) tests/check_fontfile.py build/libneuronte.so build/setup-test/state/fonts \
		build/setup-test/again --golden tests/fontfile-golden.txt

# The font-file generator, the setup command with its steps, the start-up
# units and the fontconfig file, where a system keeps them. The settings
# file is not replaced when one is there. docs/neuronte-fontfile.md.
PREFIX ?= /usr
SYSCONFDIR ?= /etc
UNITDIR ?= /usr/lib/systemd/system
USERUNITDIR ?= /usr/lib/systemd/user
install-setup: build/neuronte-fontfile build/neuronte-cells
	install -d $(DESTDIR)$(PREFIX)/bin $(DESTDIR)$(PREFIX)/lib/neuronte/setup.d \
		$(DESTDIR)$(PREFIX)/lib/neuronte/user-setup.d $(DESTDIR)$(USERUNITDIR) \
		$(DESTDIR)$(SYSCONFDIR)/neuronte $(DESTDIR)$(SYSCONFDIR)/fonts/conf.d \
		$(DESTDIR)$(UNITDIR) $(DESTDIR)$(PREFIX)/share/doc/neuronte
	install -m 755 build/neuronte-fontfile build/neuronte-cells system/neuronte-setup $(DESTDIR)$(PREFIX)/bin/
	install -m 755 system/setup.d/20-fontfile-sans system/setup.d/22-fontfile-metric system/setup.d/25-fontfile-mono \
		system/setup.d/30-cells \
		$(DESTDIR)$(PREFIX)/lib/neuronte/setup.d/
	test -e $(DESTDIR)$(SYSCONFDIR)/neuronte/setup.conf || \
		install -m 644 system/setup.conf $(DESTDIR)$(SYSCONFDIR)/neuronte/setup.conf
	install -m 644 system/66-neuronte.conf $(DESTDIR)$(SYSCONFDIR)/fonts/conf.d/
	install -m 644 system/neuronte-setup-early.service system/neuronte-setup.service $(DESTDIR)$(UNITDIR)/
	install -m 644 system/neuronte-setup-user.service $(DESTDIR)$(USERUNITDIR)/
	install -m 644 docs/neuronte-fontfile.md docs/neuronte-cells.md docs/neuronte-fontfile-chromium.png docs/neuronte-fontfile-fallback.png \
		$(DESTDIR)$(PREFIX)/share/doc/neuronte/

# Font files for programs that take their text from fontconfig, made from
# what the nets draw. docs/neuronte-fontfile.md.
build/neuronte-fontfile: tools/neuronte-fontfile.c build/libneuronte.a
	$(CC) $(NRT_CFLAGS) -pthread -o $@ $< build/libneuronte.a $(LDLIBS)

# Cells files: likely characters drawn once for every process to map.
# docs/neuronte-cells.md.
build/neuronte-cells: tools/neuronte-cells.c build/libneuronte.a include/neuronte_cells.h
	$(CC) $(NRT_CFLAGS) -pthread -o $@ $< build/libneuronte.a $(LDLIBS)

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

.PHONY: all media test demo clean install-setup
