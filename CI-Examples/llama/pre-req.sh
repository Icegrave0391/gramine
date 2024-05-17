#!/bin/bash

source .env

pushd ../../../scripts
    source .env
popd

# update submodules
git submodule update --init --recursive

# link llama2.cpp dir here
ln -s $(realpath `pwd`/../../../llama.cpp/) llama.cpp

# build llama2
pushd llama.cpp
    make
popd

# install model
pushd llama.cpp
    if [ ! -f $MODEL ]; then
        echo "Downloading $MODEL..."
        wget https://huggingface.co/TheBloke/Llama-2-7B-Chat-GGUF/resolve/main/llama-2-7b-chat.Q4_K_M.gguf
    fi
popd
