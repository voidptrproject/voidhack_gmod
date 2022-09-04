cmake_minimum_required (VERSION 3.8)

project ( "voidhack_gmod" )

set (CMAKE_CXX_STANDARD 23)
set (CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_subdirectory ( "thirdparty" )
add_subdirectory ( "gmod" )
