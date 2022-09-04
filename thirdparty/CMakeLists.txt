cmake_minimum_required (VERSION 3.8)

set ( memory_signature_include_path "${CMAKE_SOURCE_DIR}/thirdparty/memory_signature/include/" CACHE STRING "Path for memory_signature module" )
set ( kiero_include_path "${CMAKE_SOURCE_DIR}/thirdparty/kiero/" CACHE STRING "Path for kiero module" )
set ( imgui_include_path "${CMAKE_SOURCE_DIR}/thirdparty/imgui/" CACHE STRING "Path for imgui module" )
set ( json_include_path  "${CMAKE_SOURCE_DIR}/thirdparty/json/" CACHE STRING "Path for json modue")
set ( directx_include_path "${CMAKE_SOURCE_DIR}/thirdparty/directx_sdk/include/" CACHE STRING "Path for dx9 sdk" )
set ( directx_lib_path "${CMAKE_SOURCE_DIR}/thirdparty/directx_sdk/lib/" CACHE STRING "Libs for dx9 sdk" )
set ( stbi_include_path "${CMAKE_SOURCE_DIR}/thirdparty/stbi/" CACHE STRING "Path for stbi libs" )

add_library ( kiero_lib "kiero/kiero.cpp" "kiero/kiero.h" )

add_subdirectory ( minhook )

file ( GLOB IMGUIFILES "imgui/*.cpp" "imgui/*.h" "imgui/*.hpp" )
add_library ( imgui_lib ${IMGUIFILES} )