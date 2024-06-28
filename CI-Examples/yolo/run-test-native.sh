#!/bin/bash

DIR=/home/tdx/gramine/CI-Examples/yolo/yolov5ncnn/build

pushd $DIR
./yolov5s ../zidane.jpg
popd

