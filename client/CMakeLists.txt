cmake_minimum_required(VERSION 3.17)
project(RemotePSP)

set(CMAKE_CXX_STANDARD 17)

if(PSP)
    add_compile_definitions(BUILD_PLAT=2)
else()
    add_compile_definitions(BUILD_PLAT=1)
endif()

file(GLOB_RECURSE SRC_FILES src/*.cpp)
file(GLOB_RECURSE INC_FILES src/*.hpp)

add_executable(${PROJECT_NAME} ${SRC_FILES} ${INC_FILES})
add_subdirectory(ext/SDC Stardust)
add_compile_definitions(MATHFU_COMPILE_WITHOUT_SIMD_SUPPORT)
target_include_directories(${PROJECT_NAME} PRIVATE src ext/SDC/ext/mathfu/include)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
target_link_libraries(${PROJECT_NAME} PRIVATE Stardust-Celeste SC-Entry)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto -g3")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")

if(PSP)
    create_pbp_file(
            TARGET ${PROJECT_NAME}
            ICON_PATH NULL
            BACKGROUND_PATH NULL
            PREVIEW_PATH NULL
            TITLE ${PROJECT_NAME}
            BUILD_PRX
    )
endif()