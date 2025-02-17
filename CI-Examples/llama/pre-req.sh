#!/bin/bash

source .env

pushd ../../../scripts
    source .env
popd

# build llama2
pushd llama.cpp
    make clean
    make -j$(shell nproc)
popd

# install model
pushd llama.cpp
    if [ ! -f $MODEL ]; then
        echo "Copying $MODEL..."
        cp ~/llama-2-7b-chat.Q4_K_M.gguf ./
    fi
popd
