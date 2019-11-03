cmake_minimum_required(VERSION 3.13)
project(BP)

set(CMAKE_CXX_STANDARD 14)

add_executable(Translator2 Translator/src/main.cpp Translator/src/lexAnalyzator.cpp Translator/lib/lexAnalyzator.h Translator/lib/global.h Translator/src/error.cpp Translator/lib/error.h Translator/src/parse.cpp Translator/lib/parse.h Translator/lib/semantic.h Translator/src/semantic.cpp)
add_executable(VM2 VM/src/main.cpp VM/lib/vm.h VM/src/vm.cpp VM/src/object.cpp VM/lib/object.h VM/src/method.cpp VM/lib/method.h VM/src/place.cpp VM/lib/place.h VM/src/trans.cpp VM/lib/trans.h VM/lib/global.h VM/src/error.cpp VM/lib/error.h VM/src/instance.cpp VM/lib/instance.h)