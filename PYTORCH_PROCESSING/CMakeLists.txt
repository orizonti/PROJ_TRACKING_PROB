cmake_minimum_required(VERSION 3.14)

project(TorchProcessingModule LANGUAGES CXX)

message("[ PYTORCH PROCESSING LIBRARY ADD ]")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#===========================================================

set(LIBRARY_DIR "D:/LIBRARY")
set(COMPILER "msvc2019_64")
set(TORCH_VERSION "libtorch113")

if(EXISTS "/home/broms/LIBRARY")
set(LIBRARY_DIR "/home/broms/LIBRARY")
set(COMPILER "gcc_64")
set(TORCH_VERSION "libtorch")
endif()

set(PYTORCH_DIR "${LIBRARY_DIR}/NN_LIBRARY/LIBTORCH/${TORCH_VERSION}")
set(Torch_DIR ${PYTORCH_DIR})
set(QT_LIB_DIR "${LIBRARY_DIR}/Qt653/6.5.3/${COMPILER}")


if(NOT EXISTS ${QT_LIB_DIR})
message("QT LIBRARY NOT FOUND IN: ${LIBRARY_DIR}")
endif()

if(NOT EXISTS ${PYTORCH_DIR})
message("PYTORCH LIBRARY NOT FOUND IN: ${LIBRARY_DIR}/NN_LIBRARY")
endif()
#===========================================================

cmake_path(GET PROJECT_SOURCE_DIR PARENT_PATH SOURCE_DIR)
message("MAIN PROJECT SOURCE: "${SOURCE_DIR})


set(CMAKE_PREFIX_PATH ${QT_LIB_DIR})
find_package(Qt6 COMPONENTS Core REQUIRED)

set(CMAKE_PREFIX_PATH ${PYTORCH_DIR})
find_package(Torch REQUIRED)

add_library(TorchProcessingModule SHARED
  TorchProcessing_global.h
  TorchProcessing.cpp
  TorchProcessing.h
)

include_directories("${PYTORCH_DIR}/include")
include_directories("${PYTORCH_DIR}/include/torch/csrc/api/include")
include_directories("${SOURCE_DIR}")
#=====================================================================
link_directories("${PYTORCH_DIR}/lib")
if (WIN32)
target_link_libraries(TorchProcessingModule "${PYTORCH_DIR}/lib/torch_cpu.lib")
target_link_libraries(TorchProcessingModule "${PYTORCH_DIR}/lib/torch.lib")
target_link_libraries(TorchProcessingModule "${PYTORCH_DIR}/lib/c10.lib")
endif (WIN32)

if (UNIX)
target_link_libraries(TorchProcessingModule "${PYTORCH_DIR}/lib/libtorch_cpu.so")
target_link_libraries(TorchProcessingModule "${PYTORCH_DIR}/lib/libtorch.so")
target_link_libraries(TorchProcessingModule "${PYTORCH_DIR}/lib/libc10.so")
endif (UNIX)
target_link_libraries(TorchProcessingModule Qt6::Core)
#=====================================================================

target_compile_definitions(TorchProcessingModule PRIVATE TORCH_PROCESSING_MODULE_LIBRARY)


#set(CMAKE_PROJECT_INCLUDE_BEFORE "D:/DEVELOPMENT/PROJECTS/CONTROL_PROJECTS/DC_MOTOR_CONTROL_VSCODE/.qtc/package-manager/auto-setup.cmake")