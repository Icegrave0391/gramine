#!/bin/bash

source .env
prompt_file=$(realpath `pwd`/demo_prompt.txt)
result_file=$(realpath `pwd`/result.txt)
log_file=$(realpath `pwd`/log)

INPUT_CHANNEL="/sys/kernel/debug/encos-IO-emulate/in"

# Use the debugfs to emulate the sandbox input channel
cat $prompt_file | sudo tee $INPUT_CHANNEL > /dev/null

# Start erebor libOS based llama.cpp
gramine-encos llama -m llama.cpp/llama-2-7b-chat.Q4_K_M.gguf --erebor-ioctl -n 100
