#!/usr/bin/env bash
set -euo pipefail
SRC="${1:-hotcoffee.cpp}"
OUT="${2:-HotCoffee_F2.asi}"
CXX="${CXX:-g++}"
CC="${CC:-gcc}"
"$CXX" -O2 -fno-exceptions -fno-rtti -S -o hotcoffee_multi.s "$SRC"
python3 - <<'PYCODE'
import re
p='hotcoffee_multi.s'
s=open(p, encoding='utf-8').read()
lines=s.split('\n')
fixed=0
for i, line in enumerate(lines):
    if re.match(r'^\s*(call|jmp)\s+"\?', line) and not line.rstrip().endswith('"'):
        lines[i]=line.rstrip()+'"'
        fixed += 1
with open('hotcoffee_multi_fixed.s','w',encoding='utf-8') as f:
    f.write('\n'.join(lines))
print('quote-fixed lines:', fixed)
PYCODE
"$CC" -O2 -shared -nostdlib -nostartfiles \
  -Wl,--entry,DllMain -Wl,--subsystem,windows \
  -o "$OUT" hotcoffee_multi_fixed.s libshv.a -lkernel32 -luser32
ls -la "$OUT"
