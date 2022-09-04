cmake_minimum_required (VERSION 3.8)

file(GLOB_RECURSE GAME_SDK_FILES "${CMAKE_SOURCE_DIR}/gmod/" "*.cpp" "*.hpp" "*.h")
message("GAME_SDK_FILES = ${GAME_SDK_FILES}")

add_library ( gmod SHARED ${GAME_SDK_FILES})

target_link_directories ( gmod PUBLIC ${directx_lib_path} )
target_include_directories ( gmod PUBLIC ${memory_signature_include_path} ${kiero_include_path} ${imgui_include_path} ${json_include_path} 
										${directx_include_path} ${stbi_include_path} )
target_link_libraries ( gmod PRIVATE minhook d3d9 kiero_lib imgui_lib d3dx9)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  target_compile_options(gmod PRIVATE "/ZI")
  target_link_options(gmod PRIVATE "/SAFESEH:NO" "/INCREMENTAL:YES")
endif()