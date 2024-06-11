#!/bin/bash
source .env

pushd graphchi-cpp

$GRAPHCHI file $GRAPHFILE niters 10 membudget_mb 1024
rm -rf large_twitch_edges.csv.*.bin
rm -rf large_twitch_edges.csv.*.vout
rm -rf large_twitch_edges.csv.deltalog
rm -rf large_twitch_edges.csv.*.adj
rm -rf large_twitch_edges.csv.*.adjidx
rm -rf large_twitch_edges.csv.*.size
rm -rf large_twitch_edges.csv.numvertices
popd