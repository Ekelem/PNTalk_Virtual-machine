#!/bin/bash

echo "Starting PNTalk Virtual Machine..";

cd /VM/protos
./build-protobuf.sh
cd /VM
cmake .
make
./VM2 -d 51898

sleep infinity;
