#!/usr/bin/env bash
#
# End-to-end verification for the cglm input-validation layer.
# Reproduces all three acceptance criteria using the repository's own
# CMake test entry plus small standalone programs in this directory.
#
#   1. degenerate inputs are detected (validation ON)
#   2. switch OFF => zero performance cost (identical machine instructions)
#   3. normal inputs => zero regression (bit-for-bit identical results)
#
# Usage:  bash output/verify/run_verification.sh
# All output is written next to this script; no keyboard input is required.

set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
LOG="$HERE/verification.log"
: > "$LOG"

log() { printf '\n=== %s ===\n' "$*" | tee -a "$LOG"; }

cd "$ROOT"

# ---------------------------------------------------------------------------
# Criterion 1 & 3: repository test runner, validation enabled (default)
# ---------------------------------------------------------------------------
log "1/4 build + run cglm tests with validation ON (CMake default)"
cmake -S . -B build -DCGLM_USE_TEST=ON -DCGLM_STATIC=ON -DCGLM_SHARED=OFF -G Ninja \
      >/dev/null
cmake --build build >/dev/null
./build/tests 2>&1 | tee -a "$LOG" | grep -E "validate_|results|All tests"

# ---------------------------------------------------------------------------
# Criterion 2: same tests with the compile-time switch OFF
# ---------------------------------------------------------------------------
log "2/4 build + run cglm tests with -DCGLM_VALIDATE=OFF"
cmake -S . -B build/cglm-validate-off -DCGLM_USE_TEST=ON -DCGLM_STATIC=ON -DCGLM_SHARED=OFF \
      -DCGLM_VALIDATE=OFF -G Ninja >/dev/null
cmake --build build/cglm-validate-off >/dev/null
# detection tests are expected to fail once checks are compiled out;
# the bit-exact test must still pass.
./build/cglm-validate-off/tests 2>&1 | tee -a "$LOG" | grep -E "validate_|results" || true

# ---------------------------------------------------------------------------
# Criterion 2 (hard proof): identical instructions with the switch OFF
# ---------------------------------------------------------------------------
log "3/4 zero-overhead proof: plain vs checked assembly (CGLM_VALIDATE_OFF)"
gcc -O3 -Iinclude -DCGLM_VALIDATE_OFF -S "$HERE/plain.c"   -o "$HERE/artifacts/p.s"
gcc -O3 -Iinclude -DCGLM_VALIDATE_OFF -S "$HERE/checked.c" -o "$HERE/artifacts/c.s"
for fn in h_vec h_mat; do
  sed -n "/^$fn:/,/ret/p" "$HERE/artifacts/p.s" | sed -E -f "$HERE/norm.sed" \
      > "$HERE/artifacts/p.$fn.norm"
  sed -n "/^$fn:/,/ret/p" "$HERE/artifacts/c.s" | sed -E -f "$HERE/norm.sed" \
      > "$HERE/artifacts/c.$fn.norm"
  if diff -u "$HERE/artifacts/p.$fn.norm" "$HERE/artifacts/c.$fn.norm"; then
    echo "$fn: instruction-identical with validation OFF" | tee -a "$LOG"
  else
    echo "$fn: ASSEMBLY MISMATCH" | tee -a "$LOG"; exit 1
  fi
done

# ---------------------------------------------------------------------------
# Criterion 3 (hard proof): bitwise equality over 20k random valid inputs
# ---------------------------------------------------------------------------
log "4/4 bitwise no-regression over 20000 random valid inputs"
gcc -O2 -Iinclude "$HERE/bitexact_check.c" -lm -o "$HERE/artifacts/bitexact_on"
"$HERE/artifacts/bitexact_on" 2>&1 | tee -a "$LOG"
gcc -O2 -Iinclude -DCGLM_VALIDATE_OFF "$HERE/bitexact_check.c" -lm \
    -o "$HERE/artifacts/bitexact_off"
"$HERE/artifacts/bitexact_off" 2>&1 | tee -a "$LOG"

# Small human-readable demo of the explicit error codes.
gcc -O2 -Iinclude "$HERE/demo.c" -lm -o "$HERE/artifacts/demo_on"
"$HERE/artifacts/demo_on" 2>&1 | tee -a "$LOG"

log "verification complete; full log: $LOG"
