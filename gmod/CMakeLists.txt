cmake_minimum_required (VERSION 3.8)

add_library ( gmod SHARED "gmod.cpp"  "memory.hpp"  "hooks.hpp" "hooks.cpp")

target_include_directories ( gmod PUBLIC ${memory_signature_include_path} )
target_link_libraries ( gmod PRIVATE minhook )

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  target_compile_options(gmod PRIVATE "/ZI")
  target_link_options(gmod PRIVATE "/SAFESEH:NO")
endif()