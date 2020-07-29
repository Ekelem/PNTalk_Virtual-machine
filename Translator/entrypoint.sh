#!/bin/bash

echo "Starting PNTalk Translator..";

cd /Translator/protos
./build-protobuf.sh
cd /Translator
cmake .
make
./Translator2 -d 51899

sleep infinity;
