#!/usr/bin/env sh

set -eu

cd scripts
make
./asar_pack require.js ../embedded/app.asar
cd ..

for file in embedded/*; do
    name=$(basename "$file")
    symbol="embedded_${name//[^a-zA-Z0-9_]/_}"

    echo " XXD    $file"

    xxd -i "$file" \
        | sed -E \
            -e "s/^unsigned char .*\[\] =/static const unsigned char ${symbol}[] =/" \
            -e "s/^unsigned int .*_len =/static const size_t ${symbol}_len =/" \
        > "src/gen/embedded/${name%.*}.h"
done
