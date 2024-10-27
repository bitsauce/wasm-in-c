cmake_minimum_required(VERSION 3.30)
project(wasm_plugins_in_c)

set(CMAKE_CXX_STANDARD 23)

if (DEFINED WASMER_PATH)
	add_subdirectory(host/wasmer)
endif ()

if (DEFINED WASMTIME_PATH)
	add_subdirectory(host/wasmtime)
endif ()
