
#!/usr/bin/env bash
set -euo pipefail

# Source and output files
SRC="${1:-hotcoffee.cpp}"
OUT="${2:-HotCoffee_F2.asi}"

# Compiler
CXX="${CXX:-g++}"
CC="${CC:-gcc}"

# Step 1: Generate assembly
"$CXX" -O2 -fno-exceptions -fno-rtti \
    -S -o hotcoffee_multi.s "$SRC"

# Step 2: Link ASI directly
# Do not modify GCC-generated assembly.
# Keep standard runtime libraries enabled.
"$CC" -O2 -shared -nostartfiles \
    -Wl,--entry,DllMain \
    -Wl,--subsystem,windows \
    -o "$OUT" \
    hotcoffee_multi.s \
    libshv.a \
    -lkernel32 -luser32

# Step 3: Verify output
if [ ! -f "$OUT" ]; then
    echo "ERROR: ASI file was not generated!"
    exit 1
fi

ls -lh "$OUT"
echo "BUILD SUCCESS: $OUT"
