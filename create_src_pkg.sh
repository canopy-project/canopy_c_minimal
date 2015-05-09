#!/bin/bash
if [[ -z $1 ]]; then
    echo 'Usage: ./create_src_pkg <VERSION>'
    exit
fi
mkdir canopy-c-minimal-$1
cp README.md Makefile canopy-c-minimal-$1
cp -r example canopy-c-minimal-$1
cp -r include canopy-c-minimal-$1
cp -r src canopy-c-minimal-$1
cp -r tests canopy-c-minimal-$1
tar -czvf canopy-c-minimal_${1}.src.tar.gz canopy-c-minimal-$1
rm -r canopy-c-minimal-$1
