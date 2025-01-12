#!/bin/bash

if [ "$#" -ne 2 ]; then
  echo "Usage: $0 <input_file> <output_file>"
  exit 1
fi

in="$1"
out="$2"

if [ ! -f "$in" ]; then
  echo "ERROR: Input file does not exist!"
  exit 1
fi

# remove disassembly from file, and then convert windows line endings to unix:
sed 's/^\(.\{14\}\).\{32\}\(.*\)$/\1\2/' "$in" | tr -d '\r' > "$out"

#sed 's/^\(.\{42\}\).\{12\}\(.*\)$/\1\2/' > "$out" # remove ppu cycles

echo "Wrote to '$out'"
