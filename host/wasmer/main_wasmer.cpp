#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <print>

#include <wasmer.h>

byte_t* module_exported_memory = nullptr;

wasm_func_t* find_module_func(const char* name, const wasm_exporttype_vec_t& export_types, const wasm_extern_vec_t& exports) {
    for (size_t i = 0; i < exports.size; i++) {
        const wasm_name_t* export_name = wasm_exporttype_name(export_types.data[i]);
        if (std::string { export_name->data, export_name->size } == name)
            return wasm_extern_as_func(exports.data[i]);
    }
    return nullptr;
}

// Use the last_error API to retrieve error messages
void print_wasmtime_error() {
    int error_len = wasmer_last_error_length();
    if (error_len > 0) {
        printf("Error len: `%d`\n", error_len);
        const auto error_str = (char*)malloc(error_len);
        wasmer_last_error_message(error_str, error_len);
        printf("Error str: `%s`\n", error_str);
    }
}

std::optional<int32_t> call_sum(const wasm_exporttype_vec_t& export_types, const wasm_extern_vec_t& exports, const int32_t a, const int32_t b) {
    constexpr auto fn_name = "sum";
    std::println("Calling plugin function {}...", fn_name);
    const auto fn = find_module_func(fn_name, export_types, exports);
    if (!fn) {
        std::println("ERROR: Failed to find plugin function");
        print_wasmtime_error();
        return std::nullopt;
    }
    wasm_val_t args_val[2] = { WASM_I32_VAL(a), WASM_I32_VAL(b) };
    wasm_val_t results_val[1] = { WASM_INIT_VAL };
    wasm_val_vec_t args = WASM_ARRAY_VEC(args_val);
    wasm_val_vec_t results = WASM_ARRAY_VEC(results_val);
    if (wasm_func_call(fn, &args, &results)) {
        std::println("ERROR: Failed to call plugin function");
        print_wasmtime_error();
        return std::nullopt;
    }
    return results_val[0].of.i32;
}

std::optional<int32_t> call_get_heap_allocated_string(const wasm_exporttype_vec_t& export_types, const wasm_extern_vec_t& exports) {
    constexpr auto fn_name = "get_heap_allocated_string";
    std::println("Calling plugin function {}...", fn_name);
    const auto fn = find_module_func(fn_name, export_types, exports);
    if (!fn) {
        std::println("ERROR: Failed to find plugin function");
        print_wasmtime_error();
        return std::nullopt;
    }
    wasm_val_vec_t args {};
    wasm_val_t result_values[1] = { WASM_INIT_VAL };
    wasm_val_vec_t results = WASM_ARRAY_VEC(result_values);
    if (wasm_func_call(fn, &args, &results)) {
        std::println("ERROR: Failed to call plugin function");
        print_wasmtime_error();
        return std::nullopt;
    }
    return result_values[0].of.i32;
}

bool call_free_heap_allocated_string(const wasm_exporttype_vec_t& export_types, const wasm_extern_vec_t& exports, const int32_t address) {
    constexpr auto fn_name = "free_heap_allocated_string";
    std::println("Calling plugin function {}...", fn_name);
    const auto fn = find_module_func(fn_name, export_types, exports);
    if (!fn) {
        std::println("ERROR: Failed to find plugin function");
        print_wasmtime_error();
        return false;
    }
    wasm_val_t arg_values[1] = { WASM_I32_VAL(address) };
    wasm_val_vec_t args = WASM_ARRAY_VEC(arg_values);
    wasm_val_vec_t results {};
    if (wasm_func_call(fn, &args, &results)) {
        std::println("ERROR: Failed to call plugin function");
        print_wasmtime_error();
        return false;
    }
    return true;
}

bool call_test_print(const wasm_exporttype_vec_t& export_types, const wasm_extern_vec_t& exports) {
    constexpr auto fn_name = "test_print";
    std::println("Calling plugin function {}...", fn_name);
    const wasm_func_t* stdout_test = find_module_func(fn_name, export_types, exports);
    if (!stdout_test) {
        std::println("ERROR: Failed to find plugin function");
        print_wasmtime_error();
        return false;
    }
    wasm_val_vec_t args {};
    wasm_val_vec_t results {};
    if (wasm_func_call(stdout_test, &args, &results)) {
        std::println("ERROR: Failed to call plugin function");
        print_wasmtime_error();
        return false;
    }
    return true;
}

bool call_test_file_io(const wasm_exporttype_vec_t& export_types, const wasm_extern_vec_t& exports) {
    constexpr auto fn_name = "test_file_io";
    std::println("Calling plugin function {}...", fn_name);
    const wasm_func_t* stdout_test = find_module_func(fn_name, export_types, exports);
    if (!stdout_test) {
        std::println("ERROR: Failed to find plugin function");
        print_wasmtime_error();
        return false;
    }
    wasm_val_vec_t args {};
    wasm_val_vec_t results {};
    if (wasm_func_call(stdout_test, &args, &results)) {
        std::println("ERROR: Failed to call plugin function");
        print_wasmtime_error();
        return false;
    }
    return true;
}

bool call_test_host_fn(const wasm_exporttype_vec_t& export_types, const wasm_extern_vec_t& exports) {
    constexpr auto fn_name = "test_host_fn";
    std::println("Calling plugin function {}...", fn_name);
    const wasm_func_t* stdout_test = find_module_func(fn_name, export_types, exports);
    if (!stdout_test) {
        std::println("ERROR: Failed to find plugin function");
        print_wasmtime_error();
        return false;
    }
    wasm_val_vec_t args {};
    wasm_val_vec_t results {};
    if (wasm_func_call(stdout_test, &args, &results)) {
        std::println("ERROR: Failed to call plugin function");
        print_wasmtime_error();
        return false;
    }
    return true;
}

int main() {
    std::println("Creating wasm engine and store...");
    auto engine = wasm_engine_new();
    if (!engine) {
        std::println("ERROR: Failed to create WASM engine. Exiting...");
        print_wasmtime_error();
        return 1;
    }

    auto store = wasm_store_new(engine);
    if (!store) {
        std::println("ERROR: Failed to create WASM store. Exiting...");
        print_wasmtime_error();
        return 1;
    }

    std::println("Creating WASI environment...");
    wasi_env_t* wasi_env = nullptr;
    {
        auto config = wasi_config_new("");
        const auto map_dir = [&](const char* alias, const std::filesystem::path& path) {
            std::println(R"(Mapping WASI path "{}" to physical path "{}")", alias, path.string());
            std::filesystem::create_directories(path);
            wasi_config_mapdir(config, alias, std::filesystem::absolute(path).string().c_str());
        };
        map_dir(".", "data");
        wasi_env = wasi_env_new(store, config);
        if (!wasi_env) {
            std::println("ERROR: Failed to create WASI env. Exiting...");
            print_wasmtime_error();
            return 1;
        }
    }

    const std::filesystem::path plugin_path = "../../../../plugins/zig/plugin.wasm";
    std::println("Loading plugin \"{}\"...", plugin_path.string().c_str());
    wasm_byte_vec_t wasm_binary;
    if (std::ifstream file(plugin_path, std::ios::binary); file) {
        file.seekg(0, std::ios::end);
        size_t file_size = file.tellg();
        file.seekg(0, std::ios::beg);
        wasm_byte_vec_new_uninitialized(&wasm_binary, file_size);
        file.read(wasm_binary.data, static_cast<std::streamsize>(wasm_binary.size));
    }
    else {
        std::println("ERROR: Failed to read plugin file. Exiting...");
        return 1;
    }
    auto module = wasm_module_new(store, &wasm_binary);
    wasm_byte_vec_delete(&wasm_binary);
    if (!module) {
        std::println("ERROR: Failed to load WASM module. Exiting...");
        print_wasmtime_error();
        return 1;
    }

    std::println("Creating imported function \"host_func\"...");
    auto host_func_type = wasm_functype_new_0_1(wasm_valtype_new_i32()); // 0 parameters, 1 return value (i32)
    if (!host_func_type) {
        std::println("ERROR: Failed to create \"host_func\" function type. Exiting...");
        print_wasmtime_error();
        return 1;
    }
    auto host_func = wasm_func_new(store, host_func_type, [](const wasm_val_vec_t* args, wasm_val_vec_t* results) -> wasm_trap_t* {
        std::println("Running \"host_fn\" on host...");
        wasm_val_t value = WASM_I32_VAL(42);
        wasm_val_copy(&results->data[0], &value);
        return nullptr;
    });
    if (!host_func) {
        std::println("ERROR: Failed to create \"host_func\" function. Exiting...");
        print_wasmtime_error();
        return 1;
    }
    wasm_functype_delete(host_func_type);

    std::println("Gathering imports...");
    std::unordered_map<std::string, wasm_extern_t*> imports_map;
    {
        wasmer_named_extern_vec_t wasi_imports;
        if (wasi_get_unordered_imports(wasi_env, module, &wasi_imports)) {
            for (size_t i = 0; i < wasi_imports.size; i++) {
                auto named_extern = wasi_imports.data[i];
                auto module_name = wasmer_named_extern_module(named_extern);
                auto import_name = wasmer_named_extern_name(named_extern);
                std::string module_string { module_name->data, module_name->size };
                std::string import_string { import_name->data, import_name->size };
                const auto import_key = std::format(R"("{}"."{}")", module_string, import_string);
                if (imports_map.contains(import_key)) {
                    std::println("ERROR: WASM module contains duplicate import \"{}\". Exiting...", import_key);
                    return 1;
                }
                imports_map[import_key] = const_cast<wasm_extern_t*>(wasmer_named_extern_unwrap(named_extern));
            }
        }
        else {
            std::println("ERROR: Failed to get WASI imports. Exiting...");
            print_wasmtime_error();
            return 1;
        }
    }
    imports_map[R"("env"."host_fn")"] = wasm_func_as_extern(host_func);
    std::println("{} imports found", imports_map.size());

    wasm_importtype_vec_t module_import_types;
    wasm_module_imports(module, &module_import_types);
    if (imports_map.size() < module_import_types.size) {
        std::println("ERROR: Number of found imports is less than number of imports in the module. Exiting...");
        print_wasmtime_error();
        return 1;
    }

    wasm_extern_vec_t imports;
    wasm_extern_vec_new_uninitialized(&imports, module_import_types.size);
    for (size_t i = 0; i < module_import_types.size; i++) {
        auto module = wasm_importtype_module(module_import_types.data[i]);
        std::string moduleStr { module->data, module->size };
        auto name = wasm_importtype_name(module_import_types.data[i]);
        std::string nameStr { name->data, name->size };

        auto extern_type = wasm_importtype_type(module_import_types.data[i]);
        const auto type = wasm_externtype_kind(extern_type);

        switch (type) {
            case WASM_EXTERN_FUNC: {
                const auto import_key = std::format(R"("{}"."{}")", moduleStr, nameStr);
                if (const auto itr = imports_map.find(import_key); itr != imports_map.end()) {
                    imports.data[i] = itr->second;
                }
                else {
                    std::println("ERROR: Module import {} not found. Exiting...", import_key);
                    return 1;
                }
                break;
            }
            case WASM_EXTERN_GLOBAL: {
                auto global_type = wasm_externtype_as_globaltype_const(extern_type);
                wasm_val_t val = WASM_I32_VAL(0);
                auto global = wasm_global_new(store, global_type, &val);
                imports.data[i] = wasm_global_as_extern(global);
                break;
            }
            case WASM_EXTERN_TABLE: {
                auto table_type = wasm_externtype_as_tabletype_const(extern_type);
                // auto table = wasm_table_new(store, table_type, nullptr);
                break;
            }
            case WASM_EXTERN_MEMORY: {
                auto memory_type = wasm_externtype_as_memorytype_const(extern_type);
                auto memory = wasm_memory_new(store, memory_type);
                imports.data[i] = wasm_memory_as_extern(memory);
                wasm_memory_pages_t pages = wasm_memory_size(memory);
                size_t data_size = wasm_memory_data_size(memory);
                printf("Memory size (pages): %d\n", pages);
                printf("Memory size (bytes): %d\n", (int)data_size);
                printf("Growing memory...\n");
                if (!wasm_memory_grow(memory, 2)) {
                    printf("> Error growing memory!\n");
                    return 1;
                }
                wasm_memory_pages_t new_pages = wasm_memory_size(memory);
                printf("New memory size (pages): %d\n", new_pages);
                break;
            }
        }

        const auto type_as_string = [&] {
            switch (type) {
                case WASM_EXTERN_FUNC: return "FUNC";
                case WASM_EXTERN_GLOBAL: return "GLOBAL";
                case WASM_EXTERN_TABLE: return "TABLE";
                case WASM_EXTERN_MEMORY: return "MEMORY";
            }
            return "";
        }();

        printf("Import %s.%s (%s)\n", moduleStr.c_str(), nameStr.c_str(), type_as_string);
    }

    std::println("Instantiating module...");
    auto instance = wasm_instance_new(store, module, &imports, nullptr);
    if (!instance) {
        std::println("ERROR: Failed to create instance. Exiting...");
        print_wasmtime_error();
        return 1;
    }
    if (!wasi_env_initialize_instance(wasi_env, store, instance)) {
        std::println("ERROR: Failed to initialize WASI environment. Exiting...");
        print_wasmtime_error();
        return 1;
    }

    std::println("Retrieving exports...");
    wasm_extern_vec_t exports;
    wasm_instance_exports(instance, &exports);
    if (exports.size == 0) {
        std::println("ERROR: Failed to retrieve instance exports (or instance has no exports). Exiting...");
        print_wasmtime_error();
        return 1;
    }
    std::println("Found {} instance exports...", exports.size);

    wasm_exporttype_vec_t export_types;
    wasm_module_exports(module, &export_types);
    std::println("Found {} instance exports...", export_types.size);
    if (export_types.size == 0) {
        std::println("ERROR: Failed to retrieve module exports (or module has no exports). Exiting...");
        print_wasmtime_error();
        return 1;
    }

    if (exports.size != export_types.size) {
        std::println("ERROR: Module and instance have different number of exports. Exiting...");
        print_wasmtime_error();
        return 1;
    }


    for (size_t i = 0; i < export_types.size; i++) {
        auto export_name = wasm_exporttype_name(export_types.data[i]);
        std::string export_name_string { export_name->data, export_name->size };

        auto extern_type = wasm_exporttype_type(export_types.data[i]);
        const auto type = wasm_externtype_kind(extern_type);

        switch (type) {
            case WASM_EXTERN_FUNC: {
                break;
            }
            case WASM_EXTERN_GLOBAL: {
                break;
            }
            case WASM_EXTERN_TABLE: {
                break;
            }
            case WASM_EXTERN_MEMORY: {
                auto memory_type = wasm_externtype_as_memorytype_const(extern_type);
                auto memory = wasm_extern_as_memory(exports.data[i]);
                module_exported_memory = wasm_memory_data(memory);
                break;
            }
        }

        const auto type_as_string = [&] {
            switch (type) {
                case WASM_EXTERN_FUNC: return "FUNC";
                case WASM_EXTERN_GLOBAL: return "GLOBAL";
                case WASM_EXTERN_TABLE: return "TABLE";
                case WASM_EXTERN_MEMORY: return "MEMORY";
            }
            return "";
        }();

        printf("Export %s (%s)\n", export_name_string.c_str(), type_as_string);
    }

    const auto sum_result = call_sum(export_types, exports, 7, 3);
    std::println("Sum result 7 + 3 = {}", *sum_result);

    call_test_print(export_types, exports);

    int32_t address = *call_get_heap_allocated_string(export_types, exports);
    auto retval = (char*)(module_exported_memory + address);
    std::println("Heap str: {}", retval);
    call_free_heap_allocated_string(export_types, exports, address);

    call_test_file_io(export_types, exports);
    call_test_host_fn(export_types, exports);

    wasm_module_delete(module);
    wasm_instance_delete(instance);
    wasm_extern_vec_delete(&exports);
    wasm_exporttype_vec_delete(&export_types);
    wasm_extern_vec_delete(&imports);
    wasm_store_delete(store);
    wasm_engine_delete(engine);

    return 0;
}

