cmake_minimum_required (VERSION 3.8)

set ( memory_signature_include_path "${CMAKE_SOURCE_DIR}/thirdparty/memory_signature/include/" CACHE STRING "Path for memory_signature module" )
set ( kiero_include_path "${CMAKE_SOURCE_DIR}/thirdparty/kiero/" CACHE STRING "Path for kiero module" )
set ( imgui_include_path "${CMAKE_SOURCE_DIR}/thirdparty/imgui/" CACHE STRING "Path for imgui module" )
set ( json_include_path  "${CMAKE_SOURCE_DIR}/thirdparty/json/" CACHE STRING "Path for json modue")

add_library ( kiero_lib "kiero/kiero.cpp" "kiero/kiero.h" )

add_subdirectory ( minhook )

file ( GLOB IMGUIFILES "imgui/*.cpp" "imgui/*.h" "imgui/*.hpp" )
add_library ( imgui_lib ${IMGUIFILES} )