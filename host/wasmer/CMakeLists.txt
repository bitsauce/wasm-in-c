cmake_minimum_required(VERSION 3.30)
project(wasmer_as_host)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE main_wasmer.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
		${WASMER_PATH}/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
		${WASMER_PATH}/lib/wasmer.dll.lib
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${WASMER_PATH}/lib/wasmer.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
