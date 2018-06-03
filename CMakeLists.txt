cmake_minimum_required( VERSION 3.9 )
project( HotLoading )

set(EXTERNALS C:/Home/Workspace/Externals)
set(TYPEINFO ${EXTERNALS}/GenMetadata)
set(SDL2 ${EXTERNALS}/SDL2-2.0.5)
set(GLAD ${EXTERNALS}/GLAD)
set(OPENGL "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.15063.0/um/x64")

include_directories( 
    ./src/
    ./generated/
    ${TYPEINFO}/include
    ${SDL2}/include
    ${GLAD}/include )

set(LIBRARY_OUTPUT_PATH ../bin)
set(EXECUTABLE_OUTPUT_PATH ../bin)

set(GLAD_SRC ${GLAD}/src/glad.c)
set(COMMONSRC ${GLAD_SRC} src/basics.cpp)

set(LIBSRC ${COMMONSRC}
            src/dll.cpp
            generated/type_db.cpp)

set(EXECSRC ${COMMONSRC}
         src/main.cpp)

set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /PDBALTPATH:HotLoadingDLL_loaded.pdb")

add_library(HotLoadingDLL SHARED ${LIBSRC})
add_executable(HotLoading ${EXECSRC})

target_link_libraries(HotLoadingDLL
                        ${OPENGL}/opengl32.lib
                        ${SDL2}/lib/x64/SDL2main.lib
                        ${SDL2}/lib/x64/SDL2.lib )

target_link_libraries(HotLoading
                        ${OPENGL}/opengl32.lib
                        ${SDL2}/lib/x64/SDL2main.lib
                        ${SDL2}/lib/x64/SDL2.lib )
