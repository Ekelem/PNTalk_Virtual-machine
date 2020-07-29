cmake_minimum_required(VERSION 3.7)
project(BP)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG  "-g")

include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/protos/build)
link_directories(${CMAKE_SOURCE_DIR}/include)
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
file(GLOB VM_SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB VM_PROTOS_FILES ${CMAKE_SOURCE_DIR}/protos/build/*.cc)
add_executable(VM2 ${VM_SRC_FILES} ${VM_PROTOS_FILES})
target_link_libraries(VM2 libyaml-cpp.a grpc++ protobuf gpr)
