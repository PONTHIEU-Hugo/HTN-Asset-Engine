# CMakeList.txt : projet CMake pour AssetEngine, incluez la source et définissez
# la logique spécifique au projet ici.
#

#add the src cpp files here
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/zlib")

add_library(htn STATIC
	"src/compression.cpp"
	"src/compression.h"
    "src/headers.h"
    "src/archive_header.h"
    "src/crypt.h"
    "src/crypt.cpp"
    "src/CommandLine.cpp"
    "src/Mat4x4i.hpp")

target_include_directories(htn PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    "${CMAKE_CURRENT_SOURCE_DIR}/zlib"
)

target_link_libraries(htn PUBLIC zlibstatic)

add_executable(HtnE "htn.cpp"
    "src/compression.cpp"
	"src/compression.h"
    "src/headers.h"
    "src/archive_header.h"
    "src/crypt.h"
    "src/crypt.cpp"
    "src/CommandLine.cpp"
    "src/Mat4x4i.hpp"
) 

target_link_libraries(HtnE PUBLIC htn)