#!/bin/bash
# usage: [MODES="old new new21 none"] bench.sh K95_DIR KILIX_DIR TREES_DIR OUT REPS ; TREES_DIR holds one Neuronte tree per mode (none is an empty directory). Each rep: one cold process (empty home), then one warm.
set -u; K=$1; KX=$2; TR=$3; OUT=$4; REPS=${5:-5}; HERE=$(dirname "$(readlink -f "$0")")
: > "$OUT"
# The state library is not timed. Outside a git checkout (a copied tree) build it with make and pass its path.
LIB=${KILIX_STATE_LIBRARY:-}
if [ -z "$LIB" ]; then
  NS=$(mktemp -d)
  LIB=$(KILIX_STORAGE_HOME=$NS/storage KILIX_BUILD_DIRECTORY=$NS/storage/build "$KX/scripts/build-state-library.sh" --print-path) || { echo "state library build failed" >&2; exit 1; }
fi
for rep in $(seq 1 $REPS); do for mode in ${MODES:-old new none}; do
  home=$(mktemp -d)
  for temp in cold warm; do
    s=$(date +%s.%N)
    line=$(cd "$K" && env -i PATH="$PATH" HOME="$home" LANG=C.UTF-8 KILIX_HOME="$KX" KILIX_STATE_LIBRARY="$LIB" NEURONTE_HOME="$TR/$mode" PYTHONDONTWRITEBYTECODE=1 OPENBLAS_NUM_THREADS=1 python3 -B "$HERE/bench_one.py" "$K" 2>"$home/err") || { echo "FAILED $mode $temp: $(tail -3 "$home/err")" >&2; continue; }
    e=$(date +%s.%N)
    echo "{\"mode\":\"$mode\",\"start\":\"$temp\",\"rep\":$rep,\"process_s\":$(echo "$e-$s"|bc),\"r\":$line}" >> "$OUT"
  done; rm -rf "$home"
done; done
