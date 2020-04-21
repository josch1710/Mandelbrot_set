cmake_minimum_required(VERSION 3.5)

set(CMAKE_CURRENT_BINDING_DIR ${CMAKE_CURRENT_SOURCE_DIR}/.)
project(Mandelbrot)
set(CMAKE_CXX_STANDARD 11)
include_directories(/Users/jochen/Downloads/lib3mf_sdk_v2.0.0/Bindings/Cpp)
set(LSUFFIX "dylib")
set(LSUFFIXOUT ".2.dylib")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")
link_directories(/Users/jochen/Downloads/lib3mf_sdk_v2.0.0/Bin)
link_libraries(3mf)
link_directories(${CMAKE_CURRENT_BINARY_DIR})

function(CopySharedLibrary MyTarget)
	add_custom_command(TARGET ${MyTarget} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "/Users/jochen/Downloads/lib3mf_sdk_v2.0.0/Bin/lib3mf.${LSUFFIX}" $<TARGET_FILE_DIR:${MyTarget}>/lib3mf${LSUFFIXOUT})
endfunction(CopySharedLibrary)

add_executable(mandelbrot "${CMAKE_CURRENT_SOURCE_DIR}/mandel.cpp")
CopySharedLibrary(mandelbrot)

target_sources(mandelbrot PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp)
target_link_libraries(mandelbrot ${CMAKE_CURRENT_SOURCE_DIR}/lib3mf.dylib)
target_include_directories(mandelbrot PRIVATE ${CMAKE_CURRENT_BINDING_DIR})
