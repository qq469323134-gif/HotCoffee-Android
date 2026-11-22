
#!/usr/bin/env bash
set -euo pipefail

# Source and output files
SRC="${1:-hotcoffee.cpp}"
OUT="${2:-HotCoffee_F2.asi}"

# Compiler
CXX="${CXX:-g++}"

# Step 1: Generate assembly
"$CXX" -O2 -fno-exceptions -fno-rtti \
    -S -o hotcoffee_multi.s "$SRC"

# Step 2: Fix assembly formatting
python3 - <<'PY'
import re

p = "hotcoffee_multi.s"

with open(p, encoding="utf-8") as f:
    lines = f.read().splitlines()

fixed = 0

for i, line in enumerate(lines):
    if re.match(r'^\s*(call|jmp)\s+"?', line) and line.rstrip().endswith('"'):
        lines[i] = line.rstrip() + '"'
        fixed += 1

with open("hotcoffee_multi_fixed.s", "w", encoding="utf-8") as f:
    f.write("\n".join(lines) + "\n")

print("Quote-fixed lines:", fixed)
PY

# Step 3: Link ASI using the C++ compiler driver
"$CXX" -O2 -shared -nostartfiles \
    -static-libgcc -static-libstdc++ \
    -Wl,--entry,DllMain \
    -Wl,--subsystem,windows \
    -o "$OUT" \
    hotcoffee_multi_fixed.s libshv.a

# Step 4: Verify output
if [ ! -f "$OUT" ]; then
    echo "ERROR: ASI output was not generated!"
    exit 1
fi

ls -lh "$OUT"
echo "BUILD SUCCESS: $OUT"
