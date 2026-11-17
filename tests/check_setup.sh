#!/bin/sh
# Check neuronte-setup and its two font steps (system/).
#
# usage: sh tests/check_setup.sh BUILD_DIR [THREADS]
#
# Everything is made under BUILD_DIR/setup-test; the weights are the tree's.
# 1. With nothing made, --check says so (exit 1) and writes nothing.
# 2. --early makes the four Sans files and not Mono; a plain run makes the
#    rest; then --check exits 0.
# 3. A run when everything is current does nothing: no file, stamp or
#    directory entry has another inode, size or time afterwards.
# 4. A font file with a new time and the same bytes is still current; one
#    with a changed byte is not, and a run makes it again as it was and
#    removes what a killed run left behind.
# 5. A stamp of another generator version, another option, or a weights
#    directory with one more bundle in it is not current.
# 6. A step that fails gives exit 1 and the steps after it are still run;
#    a step named by skip= is not run; --only with no such step is exit 1.
# 7. At a start (NEURONTE_SETUP_BOOT=1) a step that exits 75 is deferred,
#    not failed, and stays not current until a run without it does the
#    work; a step that takes longer than boot_limit is stopped and counted
#    as failed; outside a start, exit 75 is a failure.
# 8. --user runs the user's steps directory with the state under
#    $XDG_STATE_HOME/neuronte, readable by that user only, tells the step
#    its scope and the system's state directory, reads the user's own
#    settings over the system's, and refuses --early.
# 9. The cells step (30-cells, tools/neuronte-cells.c): the settings choose
#    cells, characters and styles, for all bundles or for one; a changed
#    setting or a touched file makes it not current; a file with a float
#    changed and its size and time kept is found by --verify only; a cells
#    file nobody asks for any more and what a stopped run left are removed;
#    a bundle with a colour net gets none; a mistake in a setting fails the
#    step; one thread and several write the same bytes. The status marks
#    are added in the regular style at their own cells, once where the
#    other settings already draw them, and cells_marks=none leaves them out.
# It leaves BUILD_DIR/setup-test/state/fonts for tests/check_fontfile.py.
set -u
build=${1:?usage: check_setup.sh BUILD_DIR [THREADS]}
threads=${2:-2}
root=$(cd "$(dirname "$0")/.." && pwd)
build=$(cd "$build" && pwd)
t=$build/setup-test
rm -rf "$t"
mkdir -p "$t/steps" "$t/weights"
state=$t/state
fonts=$state/fonts
printf 'weights=%s\nstate=%s\nthreads=%s\n' "$root/weights" "$state" "$threads" > "$t/setup.conf"
export NEURONTE_SETUP_CONF="$t/setup.conf" NEURONTE_SETUP_STEPS="$root/system/setup.d"
export NEURONTE_FONTFILE="$build/neuronte-fontfile" NEURONTE_CELLS="$build/neuronte-cells"
# fc-cache, when there is one, writes here and not into the caller's cache.
export XDG_CACHE_HOME="$t/cache"
setup="sh $root/system/neuronte-setup --quiet"
fails=0
checks=0

expect() { # expect CODE WHAT COMMAND...
    want=$1 what=$2
    shift 2
    "$@" > "$t/out.txt" 2>&1
    got=$?
    checks=$((checks + 1))
    if [ "$got" != "$want" ]; then
        echo "FAIL $what: exit $got, expected $want"
        sed 's/^/     /' "$t/out.txt"
        fails=$((fails + 1))
    fi
}
is() { # is WHAT TEST-ARGS...
    what=$1
    shift
    checks=$((checks + 1))
    if ! test "$@"; then
        echo "FAIL $what"
        fails=$((fails + 1))
    fi
}
# Every entry under the state directory but the lock, with inode, size and time.
snapshot() { find "$state" ! -name setup.lock -exec stat -c '%n %i %s %y' {} + | sort; }
sha() { sha256sum "$1" | cut -c1-64; }

# 1
expect 1 "--check with nothing made" $setup --check
is "--check wrote nothing" ! -e "$state"

# 2
expect 0 "--early" $setup --early
for f in Regular Bold Italic BoldItalic; do
    is "--early made NeuronteSans-$f.ttf" -s "$fonts/NeuronteSans-$f.ttf"
done
is "--early left Mono for later" ! -e "$fonts/NeuronteMono-Regular.ttf"
expect 0 "--early --check after --early" $setup --early --check
expect 1 "--check before the rest is made" $setup --check
expect 0 "a plain run" $setup
is "the run made NeuronteMono-Regular.ttf" -s "$fonts/NeuronteMono-Regular.ttf"
expect 0 "--check when everything is made" $setup --check
is "the fonts directory is readable by every user" "$(stat -c %a "$fonts")" = 755
is "the font files are readable by every user" "$(stat -c %a "$fonts/NeuronteSans-Regular.ttf")" = 644
is "the run made the newest split bundle's cells file" -s "$state/cells/neuronte-split-v9.cells"
# The sun and the cloud at Kilix's five cells: 2 characters, 976 pixels.
is "a bundle with no ASCII has a cells file for its status marks" \
    "$(stat -c %s "$state/cells/neuronte-term-dingbat-v1.cells")" = $((128 + 16 * 10 + 4 * 2 * 976))
is "the cells file is readable by every user" \
    "$(stat -c %a "$state/cells" "$state/cells/neuronte-split-v9.cells" | tr '\n' ' ')" = "755 644 "
left=$(find "$state" -name '*.tmp' | wc -l)
is "no temporary file is left" "$left" = 0

# 3
snapshot > "$t/before.txt"
expect 0 "a run when current" $setup
expect 0 "an early run when current" $setup --early
snapshot > "$t/after.txt"
is "a run when current changes nothing" "$(sha "$t/before.txt")" = "$(sha "$t/after.txt")"

# 4
bold=$fonts/NeuronteSans-Bold.ttf
whole=$(sha "$bold")
touch "$bold"
expect 0 "--check after a font file's time changed" $setup --check
expect 0 "a run after a font file's time changed" $setup
is "that run did not write the font file again" "$(sha "$bold")" = "$whole"
printf 'X' | dd of="$bold" bs=1 seek=5000 conv=notrunc 2> /dev/null
expect 1 "--check after a byte of a font file changed" $setup --check
expect 0 "a run after a byte of a font file changed" $setup
is "that run made the font file as it was" "$(sha "$bold")" = "$whole"
rm "$fonts/NeuronteSans-Italic.ttf"
: > "$fonts/.NeuronteSans-Italic.ttf.12345.tmp" # what a killed run leaves
expect 1 "--early --check after a font file was removed" $setup --early --check
expect 0 "an early run after a font file was removed" $setup --early
expect 0 "--check after that" $setup --check
left=$(find "$state" -name '*.tmp' | wc -l)
is "the file a killed run left is gone" "$left" = 0

# 5
stamp=$state/20-fontfile-sans.stamp
cp "$stamp" "$t/stamp.keep"
sed '1s/.*/neuronte-fontfile 0/' "$t/stamp.keep" > "$stamp"
expect 1 "--check with a stamp of another generator version" $setup --early --check
sed '2s/trace=[0-9]*/trace=48/' "$t/stamp.keep" > "$stamp"
expect 1 "--check with a stamp of another option" $setup --early --check
cp -p "$t/stamp.keep" "$stamp"
expect 0 "--check with the stamp put back" $setup --early --check
for f in "$root"/weights/*.nrt "$root"/weights/*.symbols.txt; do
    [ ! -e "$f" ] || ln -s "$f" "$t/weights/"
done
printf 'weights=%s\nstate=%s\nthreads=%s\n' "$t/weights" "$state" "$threads" > "$t/more.conf"
# A bundle drawn for another box than one cell says so with a card,
# NAME.symbols.txt, and no glyph of a font file is made from it.
ln -s "$root/weights/neuronte-term-math-v1.nrt" "$t/weights/neuronte-kilix-zz-two-v1.nrt"
: > "$t/weights/neuronte-kilix-zz-two-v1.symbols.txt"
expect 0 "Mono with one more bundle that is not for one cell" \
    env NEURONTE_SETUP_CONF="$t/more.conf" $setup --only 25-fontfile-mono --check
ln -s "$root/weights/neuronte-term-math-v1.nrt" "$t/weights/neuronte-term-zz-added-v1.nrt"
expect 0 "the Sans files with one more bundle in the weights directory" \
    env NEURONTE_SETUP_CONF="$t/more.conf" $setup --early --check
expect 1 "Mono with one more bundle in the weights directory" \
    env NEURONTE_SETUP_CONF="$t/more.conf" $setup --check

# 6
for f in "$root"/system/setup.d/*; do ln -s "$f" "$t/steps/"; done
printf '#!/bin/sh\n[ "$1" = check ] && exit 1\necho "this step fails" >&2\nexit 3\n' > "$t/steps/10-fails"
chmod +x "$t/steps/10-fails"
snapshot > "$t/before.txt"
expect 1 "a run with a failing step" env NEURONTE_SETUP_STEPS="$t/steps" sh "$root/system/neuronte-setup"
is "the steps after the failing one were looked at" "$(grep -c ': current$' "$t/out.txt")" = 3
is "the failure is named" "$(grep -c '10-fails: FAILED (exit 3)' "$t/out.txt")" = 1
expect 1 "--check with a step that is not current" env NEURONTE_SETUP_STEPS="$t/steps" $setup --check
printf 'weights=%s\nstate=%s\nthreads=%s\nskip=10-fails\n' "$root/weights" "$state" "$threads" > "$t/skip.conf"
expect 0 "a run with the failing step skipped" \
    env NEURONTE_SETUP_STEPS="$t/steps" NEURONTE_SETUP_CONF="$t/skip.conf" $setup
expect 1 "--only with no such step" $setup --only 99-nothing
expect 0 "--early when a step it names is not installed" sh "$root/system/neuronte-setup" --early
is "the step that is not installed is said once" "$(grep -c '10-backend: not installed, skipped' "$t/out.txt")" = 1
expect 2 "an unknown option" $setup --nonsense
printf 'weights=%s\nthreads=many\n' "$root/weights" > "$t/bad.conf"
expect 2 "settings with a bad value" env NEURONTE_SETUP_CONF="$t/bad.conf" $setup --check
snapshot > "$t/after.txt"
is "none of that changed a file" "$(sha "$t/before.txt")" = "$(sha "$t/after.txt")"

# 7
mkdir -p "$t/boot-steps"
cat > "$t/boot-steps/10-defers" << 'STEP'
#!/bin/sh
# Stands in for a step whose work is too long for a start.
case $1 in
check) [ -e "$NEURONTE_STATE/10-defers.stamp" ] ;;
run)
    if [ "${NEURONTE_SETUP_BOOT:-}" = 1 ]; then
        echo pending > "$NEURONTE_STATE/choice"
        exit 75
    fi
    echo measured > "$NEURONTE_STATE/choice"
    echo done > "$NEURONTE_STATE/10-defers.stamp"
    ;;
esac
STEP
printf '#!/bin/sh\n[ "$1" = check ] && exit 1\nsleep 20\n' > "$t/boot-steps/20-slow"
chmod +x "$t/boot-steps/10-defers" "$t/boot-steps/20-slow"
printf 'weights=%s\nstate=%s\nboot_limit=1\n' "$root/weights" "$t/boot-state" > "$t/boot.conf"
boot="env NEURONTE_SETUP_STEPS=$t/boot-steps NEURONTE_SETUP_CONF=$t/boot.conf"
expect 0 "a deferring step at a start" $boot NEURONTE_SETUP_BOOT=1 sh "$root/system/neuronte-setup" --only 10-defers
is "it is reported as deferred" "$(grep -c '10-defers: deferred' "$t/out.txt")" = 1
is "it left something safe" "$(cat "$t/boot-state/choice")" = pending
expect 1 "--check after a deferral" $boot $setup --only 10-defers --check
before=$(date +%s)
expect 1 "a step longer than boot_limit at a start" $boot NEURONTE_SETUP_BOOT=1 sh "$root/system/neuronte-setup" --only 20-slow
is "it was stopped, not waited for" $(($(date +%s) - before)) -lt 10
is "it is reported as stopped" "$(grep -c '20-slow: FAILED (stopped' "$t/out.txt")" = 1
expect 0 "the run after the start does the deferred work" $boot $setup --only 10-defers
is "the deferred work was done" "$(cat "$t/boot-state/choice")" = measured
expect 0 "--check after it" $boot $setup --only 10-defers --check
printf '#!/bin/sh\n[ "$1" = check ] && exit 1\nexit 75\n' > "$t/boot-steps/30-always75"
chmod +x "$t/boot-steps/30-always75"
expect 1 "exit 75 outside a start" $boot $setup --only 30-always75
expect 0 "exit 75 at a start" $boot NEURONTE_SETUP_BOOT=1 $setup --only 30-always75

# 8
mkdir -p "$t/user-steps" "$t/home/.config/neuronte"
cat > "$t/user-steps/40-user-warm" << 'STEP'
#!/bin/sh
case $1 in
check) [ -e "$NEURONTE_STATE/40-user-warm.stamp" ] ;;
run) echo "$NEURONTE_SETUP_SCOPE $NEURONTE_SYSTEM_STATE $NEURONTE_THREADS $NEURONTE_WEIGHTS" \
    > "$NEURONTE_STATE/40-user-warm.stamp" ;;
esac
STEP
chmod +x "$t/user-steps/40-user-warm"
echo 'threads=1' > "$t/home/.config/neuronte/setup.conf"
user="env HOME=$t/home XDG_STATE_HOME=$t/xdg-state XDG_CONFIG_HOME=$t/home/.config NEURONTE_SETUP_USER_STEPS=$t/user-steps"
snapshot > "$t/before.txt"
expect 1 "--user --check with nothing made" $user $setup --user --check
expect 0 "--user" $user $setup --user
is "the user's step was told its scope, the system's state, the user's threads and the weights" \
    "$(cat "$t/xdg-state/neuronte/40-user-warm.stamp")" = "user $state 1 $root/weights"
is "the user's state is the user's alone" "$(stat -c %a "$t/xdg-state/neuronte")" = 700
expect 0 "--user --check after it" $user $setup --user --check
expect 2 "--user --early" $user $setup --user --early
expect 0 "--user with no steps directory" env HOME="$t/home" XDG_STATE_HOME="$t/xdg-state2" \
    NEURONTE_SETUP_USER_STEPS="$t/no-such-directory" $setup --user
expect 0 "the system's steps are not the user's" $user $setup --user --list
is "only the user's step is listed" "$(cat "$t/out.txt")" = 40-user-warm
snapshot > "$t/after.txt"
is "a user's run changed nothing of the system's" "$(sha "$t/before.txt")" = "$(sha "$t/after.txt")"

# 9
cs=$t/cells-state
split=$cs/cells/neuronte-split-v9.cells
conf() {
    printf 'weights=%s\nstate=%s\nthreads=%s\ncells_marks=none\n' "$root/weights" "$cs" "$threads" > "$t/cells.conf"
    [ $# = 0 ] || printf '%s\n' "$@" >> "$t/cells.conf"
}
cells="env NEURONTE_SETUP_CONF=$t/cells.conf $setup --only 30-cells"
tool="$build/neuronte-cells --weights $root/weights --out $cs/cells --stamp $cs/30-cells.stamp --conf $t/cells.conf"
conf 'cells=9x18,5x10' 'cells_styles=regular,bold'
expect 1 "the cells step's check with nothing made" $cells --check
expect 0 "the cells step with settings" $cells
# 189 characters of ASCII and Latin-1, two cells, two styles.
is "the cells file holds what the settings ask for" "$(stat -c %s "$split")" = $((128 + 16 * 756 + 4 * 189 * 2 * (162 + 50)))
expect 0 "the cells step's check after it" $cells --check
expect 0 "--verify draws every entry again" $tool --verify --threads "$threads"
expect 0 "one thread" "$build/neuronte-cells" --weights "$root/weights" --out "$t/cells-one" \
    --conf "$t/cells.conf" --threads 1
is "one thread and $threads write the same bytes" "$(sha "$t/cells-one/neuronte-split-v9.cells")" = "$(sha "$split")"
cp -p "$split" "$t/keep.cells"
printf '\377' | dd of="$split" bs=1 seek=$(($(stat -c %s "$split") - 2)) conv=notrunc 2> /dev/null
touch -r "$t/keep.cells" "$split"
expect 0 "a float changed with size and time kept is not seen by the check" $cells --check
expect 1 "it is seen by --verify" $tool --verify --threads "$threads"
touch "$split"
expect 1 "a cells file with another time is not current" $cells --check
expect 0 "it is made again" $cells
is "with the same bytes as before" "$(sha "$split")" = "$(sha "$t/keep.cells")"
conf 'cells=9x18' 'cells_chars.neuronte-term-dingbat-v1=2700-27BF' \
    'cells.neuronte-ui14-v1=14x28' 'cells_chars.neuronte-ui14-v1=ascii'
expect 1 "other settings make it not current" $cells --check
expect 0 "a run with settings for single bundles" $cells
is "a bundle given characters of its own has a cells file" -s "$cs/cells/neuronte-term-dingbat-v1.cells"
is "a weight file the settings name has a cells file" -s "$cs/cells/neuronte-ui14-v1.cells"
is "a bundle trained on none of the characters has none" ! -e "$cs/cells/neuronte-term-math-v1.cells"
: > "$cs/cells/neuronte-split-v9.cells.999.tmp" # what a killed run leaves
conf 'cells=9x18' 'cells.neuronte-emoji-v1=9x18' 'cells_chars.neuronte-emoji-v1=all'
expect 0 "a run after the settings for single bundles are gone" $cells
# What is left is what the stamp says was made, whatever bundles the
# weights directory holds: no file more and none fewer.
left=$(find "$cs/cells" -type f | sort | sed 's|.*/||' | tr '\n' ' ')
made=$(sed -n 's/^bundle \([^ ]*\) .* entries [1-9][0-9]* file .*/\1.cells/p' "$cs/30-cells.stamp" | sort | tr '\n' ' ')
is "the cells directory holds the files the stamp names and no other" "$left" = "$made"
is "the split bundle's file is among them" -s "$split"
is "the file of a bundle that lost its own characters is gone" ! -e "$cs/cells/neuronte-term-dingbat-v1.cells"
is "the file of a weight file the settings no longer name is gone" ! -e "$cs/cells/neuronte-ui14-v1.cells"
is "a colour net has no cells file" ! -e "$cs/cells/neuronte-emoji-v1.cells"
is "what a killed run left is gone" "$(find "$cs/cells" -name '*.tmp' | wc -l)" = 0
expect 0 "--verify of what is left" $tool --verify --threads "$threads"
find "$cs" ! -name setup.lock -exec stat -c '%n %i %s %y' {} + | sort > "$t/before.txt"
expect 0 "the cells step when current" $cells
find "$cs" ! -name setup.lock -exec stat -c '%n %i %s %y' {} + | sort > "$t/after.txt"
is "the cells step when current changes nothing" "$(sha "$t/before.txt")" = "$(sha "$t/after.txt")"
# The status marks. The split bundle is trained on two of them (the dots).
# Asked for at 9x18 and 5x10 with ASCII and Latin-1 at 9x18 regular, they add
# two entries at 9x18 and two at 5x10; with the dots already among the
# characters, only the two at 5x10.
conf 'cells=9x18' 'cells_styles=regular' 'cells_marks=2022,25CF,2600,2601,1F311-1F318' 'cells_marks_cells=9x18,5x10'
expect 0 "status marks beside the characters" $cells
is "they are drawn at their cells, regular" "$(stat -c %s "$split")" = $((128 + 16 * (189 + 4) + 4 * (189 * 162 + 2 * 162 + 2 * 50)))
is "a bundle trained only on marks has a cells file" \
    "$(stat -c %s "$cs/cells/neuronte-term-dingbat-v1.cells")" = $((128 + 16 * 4 + 4 * 2 * (162 + 50)))
expect 0 "--verify with status marks" $tool --verify --threads "$threads"
conf 'cells=9x18' 'cells_styles=regular' 'cells_chars=latin1,2022,25CF' 'cells_marks=2022,25CF' 'cells_marks_cells=9x18,5x10'
expect 0 "status marks that the characters already hold" $cells
is "they are in the file once" "$(stat -c %s "$split")" = $((128 + 16 * (191 + 2) + 4 * (191 * 162 + 2 * 50)))
conf 'cells=9x18' 'cells_styles=regular' 'cells_marks=none'
expect 0 "no status marks" $cells
is "cells_marks=none leaves them out" "$(stat -c %s "$split")" = $((128 + 16 * 189 + 4 * 189 * 162))
is "and the bundle trained only on marks has no file" ! -e "$cs/cells/neuronte-term-dingbat-v1.cells"
# A bundle of Kilix's own, by its name: all its characters, regular, at
# Kilix's cells and their two-cell boxes. A desktop face stands in for it.
mkdir -p "$t/kilix-weights"
ln -s "$root/weights/neuronte-ui14-v1.nrt" "$t/kilix-weights/neuronte-kilix-test-v1.nrt"
expect 0 "a bundle named neuronte-kilix-*" "$build/neuronte-cells" --weights "$t/kilix-weights" \
    --out "$t/kilix-cells" --threads "$threads"
is "it is drawn at Kilix's cells and two-cell boxes, regular, every character" \
    "$(stat -c %s "$t/kilix-cells/neuronte-kilix-test-v1.cells")" = \
    $((128 + 96 * 10 * 16 + 4 * 96 * (136 + 162 + 190 + 200 + 288 + 544 + 648 + 760 + 800 + 1152)))
conf 'cells=9by18'
expect 1 "a cell that is none in the settings" env NEURONTE_SETUP_CONF="$t/cells.conf" \
    sh "$root/system/neuronte-setup" --only 30-cells
is "the step is reported as failed" "$(grep -c '30-cells: FAILED (exit 2)' "$t/out.txt")" = 1

if [ "$fails" != 0 ]; then
    echo "check_setup: $fails of $checks checks failed"
    exit 1
fi
echo "check_setup: $checks checks of neuronte-setup and its font steps, 0 failed"
