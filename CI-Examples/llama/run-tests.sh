#!/bin/bash

source .env

prompt_file=$(realpath `pwd`/prompt.txt)
gramine-encos llama -m llama.cpp/llama-2-7b-chat.Q4_K_M.gguf -f $prompt_file -n 128
