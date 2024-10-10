cmake_minimum_required(VERSION 3.8)
project(MMP_AV1 DESCRIPTION "Multi Media Plugin - AV1 Deserialize and Serialize")

option(MMP_AV1_DEBUG_MODE "Enable debug mode" ON)
option(ENBALE_MMP_AV1_SAMPLE "Enbale MMP AV1 Sampele" ON)

set(MMP_AV1_SRCS)
set(MMP_AV1_INCS)

list(APPEND MMP_AV1_INCS
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Common
list(APPEND MMP_AV1_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/AV1Common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AV1Common.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/AV1Deserialize.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AV1Deserialize.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AV1Ultis.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AV1Ultis.cpp
)

add_library(MMP_AV1 STATIC ${MMP_AV1_SRCS})
add_library(MMP::AV1 ALIAS MMP_AV1)
target_include_directories(MMP_AV1 PUBLIC ${MMP_AV1_INCS})
if (MMP_AV1_DEBUG_MODE)
    target_compile_definitions(MMP_AV1 PUBLIC MMP_AV1_DEBUG_MODE)
endif()

if (ENBALE_MMP_AV1_SAMPLE)
    add_executable(Sample ${MMP_AV1_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
    target_include_directories(Sample PUBLIC ${MMP_AV1_INCS})
    if (MMP_AV1_DEBUG_MODE)
        target_compile_definitions(Sample PUBLIC MMP_AV1_DEBUG_MODE)
    endif()
    if (UNIX)
        target_link_libraries(Sample asan)
        target_compile_options(Sample PUBLIC -fsanitize=address)
    endif()
endif()