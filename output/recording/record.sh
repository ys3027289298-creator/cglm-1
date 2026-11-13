#!/usr/bin/env bash
#
# Produces a segmented terminal demo of the validation work:
# each segment starts with a titled banner, runs one verification step,
# then pauses before the next segment. No keyboard input is required.
#
# Output:
#   output/recording/cast/seg_*.cast   asciinema v2 recordings
#   output/recording/validation_demo.cast   concatenated full recording
#

set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
CAST_DIR="$HERE/cast"
mkdir -p "$CAST_DIR"

COLUMNS=100
LINES=30
PAUSE=2            # pause between segments, seconds

record_segment() {
  local index="$1" title="$2" script="$3"
  local cast="$CAST_DIR/seg_${index}.cast"
  local socket="cglm_rec_${index}"

  # Plain non-login shell, no status bar, fixed geometry -> clean frames.
  # asciinema owns the session (its child shell is the recorded pane),
  # so stopping the client ends the capture without an "[exited]" frame.
  # Create the headless session before asciinema attaches so global options
  # (notably the disabled status bar) are already in effect on first paint.
  tmux -L "$socket" new-session -d -s rec -x "$COLUMNS" -y "$LINES" \
       "env TERM=xterm-256color bash --noprofile --norc"
  tmux -L "$socket" set-option -g status off
  tmux -L "$socket" set-option -g pane-border-status off
  tmux -L "$socket" set-option -g pane-border-format ""
  asciinema rec "$cast" --command "tmux -L $socket attach -t rec" \
      --overwrite --title "$title" --output-format asciicast-v2 &
  local rec_pid=$!
  sleep 1.0

  tmux -L "$socket" send-keys "cd $ROOT" Enter
  sleep 0.5

  # show banner inside the session
  TMUX="$socket" "$script"

  sleep "$PAUSE"
  # Detach the client first so asciinema stops on its own teardown path,
  # then kill the headless server; this keeps "[server exited]" out of the
  # captured pane. The pause above is fully inside the recording.
  tmux -L "$socket" detach-client -s rec >/dev/null 2>&1 || true
  wait "$rec_pid" 2>/dev/null || true
  tmux -L "$socket" kill-server 2>/dev/null || true
  echo "recorded: $cast"
}

cd "$ROOT"

# --- Segment 1: degenerate input detection --------------------------------
seg1() {
  local s="$TMUX"
  tmux -L "$s" send-keys "clear && printf '\\033[1;36m============ 分段 1/4：退化输入可检出 ============\\033[0m\\n'" Enter
  sleep 0.8
  tmux -L "$s" send-keys "grep -n 'GLM_VALIDATE_ZERO_VECTOR\\|GLM_VALIDATE_SINGULAR_MATRIX' include/cglm/validate.h | head -4" Enter
  sleep 1.2
  tmux -L "$s" send-keys "./build/tests 2>&1 | grep validate_" Enter
  sleep 2.0
}

# --- Segment 2: explicit error codes demo ---------------------------------
seg2() {
  local s="$TMUX"
  tmux -L "$s" send-keys "clear && printf '\\033[1;36m============ 分段 2/4：明确错误码，目标缓冲不被污染 ============\\033[0m\\n'" Enter
  sleep 0.8
  tmux -L "$s" send-keys "output/verify/artifacts/demo_on" Enter
  sleep 2.5
}

# --- Segment 3: zero overhead with switch off -----------------------------
seg3() {
  local s="$TMUX"
  tmux -L "$s" send-keys "clear && printf '\\033[1;36m============ 分段 3/4：开关关闭，零性能损失（汇编逐条一致）============\\033[0m\\n'" Enter
  sleep 0.8
  tmux -L "$s" send-keys "diff output/verify/artifacts/p.h_vec.norm output/verify/artifacts/c.h_vec.norm && echo h_vec_IDENTICAL" Enter
  sleep 1.5
  tmux -L "$s" send-keys "diff output/verify/artifacts/p.h_mat.norm output/verify/artifacts/c.h_mat.norm && echo h_mat_IDENTICAL" Enter
  sleep 1.5
}

# --- Segment 4: bitwise no regression -------------------------------------
seg4() {
  local s="$TMUX"
  tmux -L "$s" send-keys "clear && printf '\\033[1;36m============ 分段 4/4：正常输入零回归（20000 组逐位比对）============\\033[0m\\n'" Enter
  sleep 0.8
  tmux -L "$s" send-keys "output/verify/artifacts/bitexact_on && output/verify/artifacts/bitexact_off" Enter
  sleep 2.5
}

record_segment 1 "退化输入可检出" seg1
sleep "$PAUSE"
record_segment 2 "明确错误码" seg2
sleep "$PAUSE"
record_segment 3 "关闭开关零性能损失" seg3
sleep "$PAUSE"
record_segment 4 "正常输入零回归" seg4

# concatenate asciinema v2 casts: keep one header, join event streams
python3 - "$CAST_DIR" "$HERE/validation_demo.cast" <<'PY'
import json, sys, glob, os
cast_dir, out = sys.argv[1], sys.argv[2]
files = sorted(glob.glob(os.path.join(cast_dir, "seg_*.cast")))
header = None
events = []
t0 = 0.0
for path in files:
    with open(path) as fh:
        first = json.loads(fh.readline())
        if header is None:
            header = first
        local = 0.0
        kept = []
        for line in fh:
            line = line.strip()
            if not line:
                continue
            ev = json.loads(line)
            # drop client teardown noise (alt-buffer switches, detach text)
            if "detached" in ev[2] or ev[2] in ("0", "1"):
                continue
            t = float(ev[0])
            if "\x1b[?1049" in ev[2] or "\x1b[?7727" in ev[2] or "\x1b[?1l" in ev[2]:
                continue
            kept.append([t, ev[1], ev[2]])
            local = t
        # keep only events up to the last visible prompt line
        while kept and not kept[-1][2].strip():
            kept.pop()
        for ev in kept:
            events.append([ev[0] + t0, ev[1], ev[2]])
        if kept:
            local = kept[-1][0]
    t0 += local + 1.5          # inter-segment pause
with open(out, "w") as fh:
    fh.write(json.dumps(header) + "\n")
    for ev in events:
        fh.write(json.dumps(ev, ensure_ascii=False) + "\n")
print("wrote", out)
PY
