# CMakeList.txt : projet CMake pour AssetEngine, incluez la source et définissez
# la logique spécifique au projet ici.
#

#add the src cpp files here
add_library(htn STATIC
	"src/compress.cpp"
	"src/compress.h"
    "src/headers.h"
    "src/archive_header.h"
    "src/crypt.h"
    "src/crypt.cpp"
    "src/CommandLine.cpp"
    "src/Mat4x4i.hpp")

target_include_directories(htn PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(ZLIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../zlib")

add_library(zlib_build STATIC IMPORTED)

set_target_properties(zlib_build PROPERTIES
    IMPORTED_LOCATION_DEBUG   "${ZLIB_DIR}/lib/Debug/libz-static.lib"
    IMPORTED_LOCATION_RELEASE "${ZLIB_DIR}/lib/Release/libz-static.lib"
)

target_include_directories(zlib_build INTERFACE
    "${ZLIB_DIR}/include"
)

target_link_libraries(htn PUBLIC zlib_build)