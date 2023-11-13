cmake_minimum_required(VERSION 3.24.0)
project(pim_index)

if(NOT NR_TASKLETS)
    set(NR_TASKLETS 17)
    message(WARNING "NR_TASKLETS is " ${NR_TASKLETS})
endif()

set(UPMEM_SDK_DIR "$ENV{UPMEM_SDK_DIR}")
if(NOT UPMEM_SDK_DIR)
    message(FATAL_ERROR "environment variable UPMEM_SDK_DIR is not set")
    exit()
endif()

set(UPMEM_C_COMPILER ${UPMEM_SDK_DIR}/bin/dpu-upmem-dpurte-clang)

if(NOT EXECUTABLE_OUTPUT_PATH)
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
endif()

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/dpu DPU_SOURCES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/util UTIL_SOURCES)

## build dpu binary
file(GLOB DPU_HEADERS "dpu/*.h")
file(GLOB UTIL_HEADERS "util/*.h")

set(DPU_TASK_NAME dpu_task)
add_custom_target(${DPU_TASK_NAME} ALL
    COMMAND ${UPMEM_C_COMPILER} -O3 -fgnu89-inline
            -DNR_TASKLETS=${NR_TASKLETS}
            -DSTACK_SIZE_DEFAULT=1024
            -I${CMAKE_CURRENT_SOURCE_DIR}/dpu
            -I${CMAKE_CURRENT_SOURCE_DIR}/util
            ${DPU_SOURCES}
            ${UTIL_SOURCES}
            -o ${EXECUTABLE_OUTPUT_PATH}/${DPU_TASK_NAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${DPU_SOURCES} ${UTIL_SOURCES} 
            ${DPU_HEADERS} ${UTIL_HEADERS}
    VERBATIM)

install(PROGRAMS ${EXECUTABLE_OUTPUT_PATH}/${DPU_TASK_NAME} DESTINATION bin)

## build util lib
unset(CMAKE_C_FLAGS)
set(PIM_INDEX_UTIL_LIB pim-index-util)
add_library(${PIM_INDEX_UTIL_LIB} SHARED ${UTIL_SOURCES})
target_include_directories(${PIM_INDEX_UTIL_LIB} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/util)

set_target_properties(${PIM_INDEX_UTIL_LIB} PROPERTIES COMPILE_FLAGS "${UTIL_FLAG}")
install(TARGETS ${PIM_INDEX_UTIL_LIB} LIBRARY DESTINATION lib)

get_target_property(C_FLAGS ${PIM_INDEX_UTIL_LIB} COMPILE_OPTIONS)
list(REMOVE_ITEM C_FLAGS "-fvisibility=hidden")
set_target_properties(${PIM_INDEX_UTIL_LIB} PROPERTIES COMPILE_OPTIONS ${C_FLAGS})

# build host
set(UPMEM_HOST_NAME host_app)
set(C_OPTIONS -O2 -std=gnu99 -fgnu89-inline -DNR_TASKLETS=${NR_TASKLETS} -pthread)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/host HOST_SOURCES)
add_executable(${UPMEM_HOST_NAME} ${HOST_SOURCES})
set_target_properties(${UPMEM_HOST_NAME} PROPERTIES COMPILE_OPTIONS "${C_OPTIONS}")

target_include_directories(${UPMEM_HOST_NAME} 
                PRIVATE ${UPMEM_SDK_DIR}/include/dpu
                        ${CMAKE_CURRENT_SOURCE_DIR}/util)
target_link_directories(${UPMEM_HOST_NAME} PRIVATE ${UPMEM_SDK_DIR/lib})
target_link_libraries(${UPMEM_HOST_NAME} dpu dpuverbose pim-index-util)
install(TARGETS ${UPMEM_HOST_NAME} RUNTIME DESTINATION bin)

# build host as lib
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/host HOST_LIB_SOURCES)
list(REMOVE_ITEM HOST_LIB_SOURCES main.c)

set(PIM_INDEX_LIB_NAME pim-index)

add_library(${PIM_INDEX_LIB_NAME} SHARED ${HOST_LIB_SOURCES})
set_target_properties(${PIM_INDEX_LIB_NAME} PROPERTIES COMPILE_OPTIONS "${C_OPTIONS}")
target_include_directories(${PIM_INDEX_LIB_NAME}
                            PUBLIC ${UPMEM_SDK_DIR}/include/dpu
                                   ${CMAKE_CURRENT_SOURCE_DIR}/util)
target_link_directories(${PIM_INDEX_LIB_NAME} PUBLIC ${UPMEM_SDK_DIR}/lib)
target_link_libraries(${PIM_INDEX_LIB_NAME} dpu dpuverbose pim-index-util)
install(TARGETS ${PIM_INDEX_LIB_NAME} LIBRARY DESTINATION lib)