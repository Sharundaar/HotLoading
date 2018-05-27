project( HotLoading )
cmake_minimum_required(VERSION 3.9)

set(EXTERNALS C:/Home/Workspace/Externals)
set(TYPEINFO ${EXTERNALS}/GenMetadata)
include_directories( 
    ./src/
    ./generated/
    ${TYPEINFO}/include )

set(LIBRARY_OUTPUT_PATH ../bin)
set(EXECUTABLE_OUTPUT_PATH ../bin)

set( COMMONSRC src/basics.cpp )

set( LIBSRC ${COMMONSRC}
            src/dll.cpp
            generated/type_db.cpp )

set( SRC ${COMMONSRC}
         src/main.cpp )

set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /PDBALTPATH:HotLoadingDLL_loaded.pdb")

add_library( HotLoadingDLL SHARED ${LIBSRC} )
add_executable( HotLoading ${SRC} )
