#!/bin/bash
source .env

pushd graphchi-cpp

$GRAPHCHI file $GRAPHFILE niters 1000 cachesize_mb 2000  membudget_mb 3000
rm -rf large_twitch_edges.csv.*.bin
rm -rf large_twitch_edges.csv.*.vout
rm -rf large_twitch_edges.csv.deltalog
rm -rf large_twitch_edges.csv.*.adj
rm -rf large_twitch_edges.csv.*.adjidx
rm -rf large_twitch_edges.csv.*.size
rm -rf large_twitch_edges.csv.numvertices
popd
