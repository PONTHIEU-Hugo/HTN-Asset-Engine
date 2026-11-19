# CMakeList.txt : projet CMake pour AssetEngine, incluez la source et définissez
# la logique spécifique au projet ici.
#

# Ajoutez une source à l'exécutable de ce projet.
add_library(AssetManager STATIC
   "AssetManager.cpp" "AssetManager.h"
   "HTNCustomData.h" "HTNBmpData.h" "HTNBmpData.cpp"
   "TgaLoader.cpp" "TgaLoader.h"
)

target_link_libraries(AssetManager PUBLIC htn)

target_compile_definitions(AssetManager PUBLIC
    ASSET_MANAGER_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}/"
)

add_executable (AssetEngine "AssetEngine.cpp" "AssetEngine.h")

target_link_libraries(AssetEngine PRIVATE AssetManager)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET AssetEngine PROPERTY CXX_STANDARD 20)
endif()
