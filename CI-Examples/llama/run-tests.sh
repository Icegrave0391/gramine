#!/bin/bash

source .env

test_prompt="how are you?"

gramine-encos llama -m llama.cpp/llama-2-7b-chat.Q4_K_M.gguf -p "$test_prompt" -n 50
