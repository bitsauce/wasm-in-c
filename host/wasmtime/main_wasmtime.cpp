#include <filesystem>
#include <format>
#include <fstream>
#include <vector>
#include <print>

#include <wasmtime.h>

void print_wasmtime_error(const wasmtime_error_t& error) {
    wasm_name_t message;
    wasmtime_error_message(&error, &message);
    std::println("wasmtime error: {}", std::string { message.data, message.size });
}

std::optional<int32_t> call_sum(const wasmtime_linker_t* linker, wasmtime_context_t* context, const int32_t a, const int32_t b) {
    constexpr auto fn_name = "sum";
    std::println("Calling plugin function {}...", fn_name);
    wasmtime_extern_t fn;
    if (!wasmtime_linker_get(linker, context, "", 0, fn_name, strlen(fn_name), &fn)) {
        std::println("ERROR: Failed to find plugin function");
        return std::nullopt;
    }
    if (fn.kind != WASMTIME_EXTERN_FUNC) {
        std::println("ERROR: Symbol {} is not a function", fn_name);
        return std::nullopt;
    }
    const wasmtime_val_t args[2] = { WASM_I32_VAL(a), WASM_I32_VAL(b) };
    wasmtime_val_t results[1];
    if (auto error = wasmtime_func_call(context, &fn.of.func, args, 2, results, 1, nullptr)) {
        std::println("ERROR: Failed to call function. Exiting...");
        print_wasmtime_error(*error);
        return std::nullopt;
    }
    return results[0].of.i32;
}

std::optional<int32_t> call_get_heap_allocated_string(const wasmtime_linker_t* linker, wasmtime_context_t* context) {
    constexpr auto fn_name = "get_heap_allocated_string";
    std::println("Calling plugin function {}...", fn_name);
    wasmtime_extern_t fn;
    if (!wasmtime_linker_get(linker, context, "", 0, fn_name, strlen(fn_name), &fn)) {
        std::println("ERROR: Failed to find plugin function");
        return std::nullopt;
    }
    if (fn.kind != WASMTIME_EXTERN_FUNC) {
        std::println("ERROR: Symbol {} is not a function", fn_name);
        return std::nullopt;
    }
    wasmtime_val_t results[1];
    if (auto error = wasmtime_func_call(context, &fn.of.func, nullptr, 0, results, 1, nullptr)) {
        std::println("ERROR: Failed to call function. Exiting...");
        print_wasmtime_error(*error);
        return std::nullopt;
    }
    return results[0].of.i32;
}

bool call_free_heap_allocated_string(const wasmtime_linker_t* linker, wasmtime_context_t* context, const int32_t address) {
    constexpr auto fn_name = "free_heap_allocated_string";
    std::println("Calling plugin function {}...", fn_name);
    wasmtime_extern_t fn;
    if (!wasmtime_linker_get(linker, context, "", 0, fn_name, strlen(fn_name), &fn)) {
        std::println("ERROR: Failed to find plugin function");
        return false;
    }
    if (fn.kind != WASMTIME_EXTERN_FUNC) {
        std::println("ERROR: Symbol {} is not a function", fn_name);
        return false;
    }
    const wasmtime_val_t args[1] = { WASM_I32_VAL(address) };
    if (auto error = wasmtime_func_call(context, &fn.of.func, args, 1, nullptr, 0, nullptr)) {
        std::println("ERROR: Failed to call function. Exiting...");
        print_wasmtime_error(*error);
        return false;
    }
    return true;
}

bool call_test_print(const wasmtime_linker_t* linker, wasmtime_context_t* context) {
    constexpr auto fn_name = "test_print";
    std::println("Calling plugin function {}...", fn_name);
    wasmtime_extern_t fn;
    if (!wasmtime_linker_get(linker, context, "", 0, fn_name, strlen(fn_name), &fn)) {
        std::println("ERROR: Failed to find plugin function");
        return false;
    }
    if (fn.kind != WASMTIME_EXTERN_FUNC) {
        std::println("ERROR: Symbol {} is not a function", fn_name);
        return false;
    }
    if (auto error = wasmtime_func_call(context, &fn.of.func, nullptr, 0, nullptr, 0, nullptr)) {
        std::println("ERROR: Failed to call function. Exiting...");
        print_wasmtime_error(*error);
        return false;
    }
    return true;
}

bool call_test_file_io(const wasmtime_linker_t* linker, wasmtime_context_t* context) {
    constexpr auto fn_name = "test_file_io";
    std::println("Calling plugin function {}...", fn_name);
    wasmtime_extern_t fn;
    if (!wasmtime_linker_get(linker, context, "", 0, fn_name, strlen(fn_name), &fn)) {
        std::println("ERROR: Failed to find plugin function");
        return false;
    }
    if (fn.kind != WASMTIME_EXTERN_FUNC) {
        std::println("ERROR: Symbol {} is not a function", fn_name);
        return false;
    }
    if (auto error = wasmtime_func_call(context, &fn.of.func, nullptr, 0, nullptr, 0, nullptr)) {
        std::println("ERROR: Failed to call function. Exiting...");
        print_wasmtime_error(*error);
        return false;
    }
    return true;
}

bool call_test_host_fn(const wasmtime_linker_t* linker, wasmtime_context_t* context) {
    constexpr auto fn_name = "test_host_fn";
    std::println("Calling plugin function {}...", fn_name);
    wasmtime_extern_t fn;
    if (!wasmtime_linker_get(linker, context, "", 0, fn_name, strlen(fn_name), &fn)) {
        std::println("ERROR: Failed to find plugin function");
        return false;
    }
    if (fn.kind != WASMTIME_EXTERN_FUNC) {
        std::println("ERROR: Symbol {} is not a function", fn_name);
        return false;
    }
    if (auto error = wasmtime_func_call(context, &fn.of.func, nullptr, 0, nullptr, 0, nullptr)) {
        std::println("ERROR: Failed to call function. Exiting...");
        print_wasmtime_error(*error);
        return false;
    }
    return true;
}

int main() {
    std::println("Creating wasm engine and store...");
    auto engine = wasm_engine_new();
    if (!engine) {
        std::println("ERROR: Failed to create WASM engine. Exiting...");
        return 1;
    }
    auto store = wasmtime_store_new(engine, nullptr, nullptr);
    if (!store) {
        std::println("ERROR: Failed to create wasmtime store. Exiting...");
        return 1;
    }
    auto context = wasmtime_store_context(store);
    if (!context) {
        std::println("ERROR: Failed to create wasmtime store context. Exiting...");
        return 1;
    }
    auto linker = wasmtime_linker_new(engine);
    if (!linker) {
        std::println("ERROR: Failed to create wasmtime linker. Exiting...");
        return 1;
    }
    if (auto error = wasmtime_linker_define_wasi(linker)) {
        std::println("ERROR: Failed to define WASI symbols in the linker. Exiting...");
        print_wasmtime_error(*error);
        return 1;
    }

    wasmtime_module_t* module;
    {
        const std::filesystem::path plugin_path = "../../../../plugins/zig/plugin.wasm";
        std::println("Loading plugin \"{}\"...", plugin_path.string());
        std::vector<uint8_t> wasm_binary;
        if (std::ifstream file(plugin_path, std::ios::binary); file) {
            file.seekg(0, std::ios::end);
            size_t file_size = file.tellg();
            file.seekg(0, std::ios::beg);
            wasm_binary.resize(file_size);
            file.read(reinterpret_cast<char*>(wasm_binary.data()), static_cast<std::streamsize>(wasm_binary.size()));
        }
        else {
            std::println("ERROR: Failed to read plugin file. Exiting...");
            return 1;
        }
        if (const auto error = wasmtime_module_new(engine, wasm_binary.data(), wasm_binary.size(), &module)) {
            std::println("ERROR: Failed to load WASM module. Exiting...");
            print_wasmtime_error(*error);
            return 1;
        }
    }

    std::println("Setting up WASI...");
    {
        const auto config = wasi_config_new();
        const auto map_dir = [&](const char* alias, const std::filesystem::path& path) {
            std::println(R"(Mapping WASI path "{}" to physical path "{}")", alias, path.string());
            std::filesystem::create_directories(path);
            wasi_config_preopen_dir(config, std::filesystem::absolute(path).string().c_str(), alias);
        };
        wasi_config_inherit_stdout(config);
        wasi_config_inherit_stderr(config);
        map_dir(".", "data");
        if (const auto error = wasmtime_context_set_wasi(context, config)) {
            std::println("ERROR: Failed to create WASI env. Exiting...");
            print_wasmtime_error(*error);
            return 1;
        }
    }

    std::println("Creating imported function \"host_func\"...");
    {
        auto host_func_type = wasm_functype_new_0_1(wasm_valtype_new_i32()); // 0 parameters, 1 return value (i32)
        if (!host_func_type) {
            std::println("ERROR: Failed to create \"host_func\" function type. Exiting...");
            return 1;
        }
        if (auto error = wasmtime_linker_define_func(linker, "env", strlen("env"), "host_fn", strlen("host_fn"), host_func_type,
            [](void* env, wasmtime_caller_t* caller, const wasmtime_val_t* args, size_t nargs, wasmtime_val_t* results, size_t nresults) -> wasm_trap_t* {
                std::println("Running \"host_fn\" on host...");
                results[0] = WASM_I32_VAL(42);
                return nullptr;
            }, nullptr, nullptr)) {
            std::println("ERROR: Failed to define \"host_fn\" function in the linker. Exiting...");
            print_wasmtime_error(*error);
            return 1;
            }
        wasm_functype_delete(host_func_type);
    }

    std::println("Linking module...");
    if (auto error = wasmtime_linker_module(linker, context, "", 0, module)) {
        std::println("ERROR: Failed to link module. Exiting...");
        print_wasmtime_error(*error);
        return 1;
    }

    uint8_t* module_memory;
    {
        wasmtime_extern_t memory;
        if (!wasmtime_linker_get(linker, context, "", 0, "memory", strlen("memory"), &memory)) {
            std::println("ERROR: Failed to get exported memory. Exiting...");
            return 1;
        }
        if (memory.kind != WASMTIME_EXTERN_MEMORY) {
            std::println(R"(ERROR: Expected "memory" to be of type "extern memory". Exiting...)");
            return 1;
        }
        if (module_memory = wasmtime_memory_data(context, &memory.of.memory); !module_memory) {
            std::println("ERROR: Failed to get exported memory. Exiting...");
            return 1;
        }
    }

    const auto sum_result = call_sum(linker, context, 7, 3);
    std::println("Sum result 7 + 3 = {}", *sum_result);

    call_test_print(linker, context);

    int32_t address = *call_get_heap_allocated_string(linker, context);
    auto retval = (char*)(module_memory + address);
    std::println("Heap str: {}", retval);
    call_free_heap_allocated_string(linker, context, address);

    call_test_file_io(linker, context);
    call_test_host_fn(linker, context);

    wasmtime_module_delete(module);
    wasmtime_linker_delete(linker);
    wasmtime_store_delete(store);
    wasm_engine_delete(engine);

    return 0;
}

