cmake_minimum_required(VERSION 3.15)
project(strictc LANGUAGES C CXX)

# Require C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# LLVM support
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Include headers
include_directories(${CMAKE_SOURCE_DIR}/include)

# Sources
set(SOURCES
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/ast.cpp
    src/codegen_llvm.cpp
    src/dgm_translator.cpp
    src/dgm_emitter.cpp
    src/runtime.c
)

# Executable
add_executable(strictc ${SOURCES})

# Link LLVM libraries
llvm_map_components_to_libnames(llvm_libs
    support
    core
    irreader
    executionengine
    mc
    native
    option
    passes
    target
    transformutils
)

target_link_libraries(strictc ${llvm_libs})

# Install rule
install(TARGETS strictc RUNTIME DESTINATION bin)

# Tests
enable_testing()
add_test(NAME HelloStrict
         COMMAND strictc ${CMAKE_SOURCE_DIR}/examples/hello.strict -o hello.out)
