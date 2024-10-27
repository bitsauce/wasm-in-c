## About

This repo contains an implementation of a simple plugin system that loads WASI compatible WASM modules as plugins.

The host application is written in C/C++, and use either `wasmtime` or `wasmer` as the wasm engine, while the plugins themselves are written various other languages  (C, C++, Zig, etc.) 

## Building the host

The host application is build using a simple CMake setup

```bash
# Ensure that paths to wasmtime and/or wasmer is set
export WASMTIME_PATH=<path-to-wasmtime>
export WASMER_PATH=<path-to-wasmer>

# Configure the build
cmake --preset windows-debug

# Build the hosts (one for each wasm engine)
cmake --build build/windows-debug
```

See the other presets for more options

## Building the plugins

### C/C++

Install the `wasi-sdk`. The deb package seems to work fine

Then, compile the plugin with clang:

```bash
cd plugins/c
/opt/wasi-sdk/bin/clang -target wasm32-wasi -Wl,--export-all -Wl,--no-entry -o plugin.wasm plugin.c
```

### Zig

The following command seem to work to output a plugin-like wasm module from Zig:

```bash
cd plugins/zig
zig build-exe plugin.zig -target wasm32-wasi -fno-entry --export=sum --export=get_heap_allocated_string --export=free_heap_allocated_string --export=test_print --export=test_file_io --export=test_host_fn
```

Unlike the C/C++ plugin, it seems that we have to list all the exports manually

## Inspecting the WASM modules

There are several ways to inspect a compiled WASM module (useful for debugging purposes)

One way is to use the `wasm-tools` CLI (downloaded separately)

```bash
# Listing imports
wasm-tools dump <path-to-wasm-file> | grep import

# Listing exports
wasm-tools dump <path-to-wasm-file> | grep export
```

Another approach is to use `wasmer inspect`

```bash
wasmer inspect <path-to-wasm-file>
```
