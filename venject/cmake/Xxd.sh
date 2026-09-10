#!/usr/bin/env bash

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

cd "$ROOT_DIR/scripts"
make
./asar_pack require.js "$ROOT_DIR/embedded/app.asar"

mkdir -p "$ROOT_DIR/src/gen/embedded"

cd "$ROOT_DIR"

for file in embedded/*; do
    name=$(basename "$file")
    symbol="embedded_${name//./_}"

    echo " XXD    $file"

    xxd -i "$file" \
        | sed -E \
            -e "s/^unsigned char .*\[\] =/static const unsigned char ${symbol}[] =/" \
            -e "s/^unsigned int .*_len =/static const size_t ${symbol}_len =/" \
        > "src/gen/embedded/${name%.*}.h"
done
