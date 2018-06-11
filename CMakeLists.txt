cmake_minimum_required( VERSION 3.9 )
project( HotLoading )

set(EXTERNALS C:/Home/Workspace/Externals)
set(TYPEINFO ${EXTERNALS}/GenMetadata)
set(SDL2 ${EXTERNALS}/SDL2-2.0.5)
set(GLAD ${EXTERNALS}/GLAD)
set(IMGUI ${EXTERNALS}/imgui)
set(ASSIMP ${EXTERNALS}/ASSIMP)
set(OPENGL "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.15063.0/um/x64")

include_directories( 
    ./src/
    ./generated/
    ./mathlib/
    ${TYPEINFO}/include
    ${SDL2}/include
    ${GLAD}/include
    ${IMGUI}
    ${ASSIMP}/include )

set(LIBRARY_OUTPUT_PATH ../bin)
set(EXECUTABLE_OUTPUT_PATH ../bin)

set(GLAD_SRC ${GLAD}/src/glad.c)
set(IMGUI_SRC ${IMGUI}/imgui.cpp
              ${IMGUI}/imgui_demo.cpp
              ${IMGUI}/imgui_draw.cpp)
set(COMMONSRC src/basics.cpp generated/type_db.cpp)

set(LIBSRC ${COMMONSRC}
            ${IMGUI_SRC} 
            ${GLAD_SRC}
            src/dll.cpp
            src/resource.cpp
            src/file_parser.cpp
            src/shader.cpp
            src/mesh.cpp )

set(EXECSRC ${COMMONSRC}
         src/main.cpp )

set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /PDBALTPATH:HotLoadingDLL_loaded.pdb")

add_library(HotLoadingDLL SHARED ${LIBSRC})
add_executable(HotLoading ${EXECSRC})

target_link_libraries(HotLoadingDLL
                        ${OPENGL}/opengl32.lib
                        ${SDL2}/lib/x64/SDL2main.lib
                        ${SDL2}/lib/x64/SDL2.lib
                        ${ASSIMP}/lib/assimp-vc140-mt.lib )

target_link_libraries(HotLoading
                        ${SDL2}/lib/x64/SDL2main.lib
                        ${SDL2}/lib/x64/SDL2.lib )
