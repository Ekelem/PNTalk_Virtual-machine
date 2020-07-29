#!/bin/bash

rm -rf build
mkdir build
echo "build protobuf directory cleared!"
for filename in v3/*.proto; do
	protoc -I ../protos/v3 --cpp_out=build ../protos/$filename
	protoc -I ../protos/v3 --grpc_out=build --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ../protos/$filename
	echo "$filename generated!"
done