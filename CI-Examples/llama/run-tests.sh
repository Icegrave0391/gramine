#!/bin/bash

source .env

gramine-encos llama -m llama.cpp/llama-2-7b-chat.Q4_K_M.gguf -p $prompt
