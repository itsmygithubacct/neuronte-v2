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
export NEURONTE_FONTFILE="$build/neuronte-fontfile"
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
for f in "$root"/weights/*.nrt; do ln -s "$f" "$t/weights/"; done
ln -s "$root/weights/neuronte-term-math-v1.nrt" "$t/weights/neuronte-term-zz-added-v1.nrt"
printf 'weights=%s\nstate=%s\nthreads=%s\n' "$t/weights" "$state" "$threads" > "$t/more.conf"
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
is "the steps after the failing one were looked at" "$(grep -c ': current$' "$t/out.txt")" = 2
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

if [ "$fails" != 0 ]; then
    echo "check_setup: $fails of $checks checks failed"
    exit 1
fi
echo "check_setup: $checks checks of neuronte-setup and its font steps, 0 failed"
