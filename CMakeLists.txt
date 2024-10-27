cmake_minimum_required(VERSION 3.30)
project(wasm_plugins_in_c)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(host/wasmer)
add_subdirectory(host/wasmtime)
