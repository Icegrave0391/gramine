#!/bin/bash

source .env

pushd graphchi-cpp
    make example_apps/pagerank
popd