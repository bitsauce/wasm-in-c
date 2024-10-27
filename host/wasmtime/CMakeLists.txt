cmake_minimum_required(VERSION 3.30)
project(wasmtime_as_host)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE main_wasmtime.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
		$ENV{WASMTIME_PATH}/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
		$ENV{WASMTIME_PATH}/lib/wasmtime.dll.lib
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $ENV{WASMTIME_PATH}/lib/wasmtime.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
