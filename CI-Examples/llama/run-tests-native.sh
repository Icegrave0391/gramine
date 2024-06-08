#!/bin/bash

source .env
prompt_file=$(realpath `pwd`/prompt.txt)
result_file=$(realpath `pwd`/result.txt)
log_file=$(realpath `pwd`/log)


pushd llama.cpp
    # ./main -m $MODEL -f $prompt_file --log-enable --log-file $log_file > /dev/null
    ./main -m $MODEL -f $prompt_file -n 128
popd