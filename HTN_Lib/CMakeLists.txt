# CMakeList.txt : projet CMake pour AssetEngine, incluez la source et définissez
# la logique spécifique au projet ici.
#

#add the src cpp files here
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/zlib")

add_library(htn STATIC
    "src/Defines.h"
    "src/CommandLine.cpp"
    "src/headers.h" "src/archive_header.h"
    "src/compression.h" "src/compression.cpp"
    "src/CompressionInfo.h"
    "src/Mat4x4i.hpp"
    "src/crypt.h" "src/crypt.cpp"
    "src/Archive.h" "src/Archive.cpp"
    "src/crc.h" "src/crc.cpp"
    "src/File.h" "src/File.cpp" "src/File.inl"
)

target_include_directories(htn PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    "${CMAKE_CURRENT_SOURCE_DIR}/zlib"
)

target_compile_definitions(htn PUBLIC
    HTN_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}/"
)

target_link_libraries(htn PUBLIC zlibstatic)

add_executable(HtnE "htn.cpp"
) 

target_link_libraries(HtnE PUBLIC htn)

