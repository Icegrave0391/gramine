#!/bin/bash

source .env
prompt_file=$(realpath `pwd`/prompt2.txt)
result_file=$(realpath `pwd`/result.txt)
log_file=$(realpath `pwd`/log)

test_prompt="how are you"

pushd llama.cpp
     ./main -m $MODEL -p "$test_prompt"
    #./main -m $MODEL -f $prompt_file --log-enable --log-file $log_file > /dev/null
popd
