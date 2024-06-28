#!/bin/bash

source .env


gramine-direct llama -m llama.cpp/llama-2-7b-chat.Q4_K_M.gguf -p "$test_prompt" -n 50
