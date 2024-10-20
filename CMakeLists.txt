cmake_minimum_required(VERSION 3.14)
project(ImageProcProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS ./ui_files)
#===========================================================

set(LIBRARY_DIR "D:/LIBRARY")
set(COMPILER "msvc2019_64")
set(TORCH_VERSION "libtorch113")
set(CV_VERSION "opencv410")

if(EXISTS "/home/broms/LIBRARY_LOC")
set(LIBRARY_DIR "/home/broms/LIBRARY_LOC")
set(COMPILER "gcc_64")
set(TORCH_VERSION "libtorch")
endif()

if(EXISTS "${LIBRARY_DIR}/IMAGE_PROCESSING/OPENCV/opencv451")
set(CV_VERSION "opencv451")
endif()

if(EXISTS "${LIBRARY_DIR}/IMAGE_PROCESSING/OPENCV/opencv410")
set(CV_VERSION "opencv410")
endif()

set(OPENCV_LIB_DIR "${LIBRARY_DIR}/IMAGE_PROCESSING/OPENCV/${CV_VERSION}/")
set(QT_LIB_DIR "${LIBRARY_DIR}/Qt653/6.5.3/${COMPILER}")
set(PYTORCH_DIR "${LIBRARY_DIR}/NN_LIBRARY/LIBTORCH/${TORCH_VERSION}/")
set(Torch_DIR ${PYTORCH_DIR})

set(Qt6_DIR "${QT_LIB_DIR}/lib/cmake/Qt6")
set(Qt6Widgets_DIR "${QT_LIB_DIR}/lib/cmake/Qt6Widgets")
set(Qt6WidgetsTools_DIR "${QT_LIB_DIR}/lib/cmake/Qt6WidgetsTools")
set(Qt6SerialPort_DIR "${QT_LIB_DIR}/lib/cmake/Qt6SerialPort")
set(Qt6PrintSupport_DIR "${QT_LIB_DIR}/lib/cmake/Qt6PrintSupport")
set(Qt6Network_DIR "${QT_LIB_DIR}/lib/cmake/Qt6Network")
set(Qt6Gui_DIR "${QT_LIB_DIR}/lib/cmake/Qt6Gui")
set(Qt6GuiTools_DIR "${QT_LIB_DIR}/lib/cmake/Qt6GuiTools")
set(Qt6EntryPointPrivate_DIR "${QT_LIB_DIR}/lib/cmake/Qt6EntryPointPrivate")
set(Qt6Core_DIR "${QT_LIB_DIR}/lib/cmake/Qt6Core")
set(Qt6CoreTools_DIR "${QT_LIB_DIR}/lib/cmake/Qt6CoreTools")

message("SET QT     LIBRARY - ${Qt6_DIR}")
message("SET OPENCV LIBRARY - ${LIBRARY_DIR}/IMAGE_PROCESSING/OPENCV/${CV_VERSION}")
message("SET TORCH  LIBRARY - ${PYTORCH_DIR}")
#====================SUBPROJECTS=============================
#add_subdirectory(PYTORCH_PROCESSING)

if( UNIX )
SET(TORCH_PROCESS_MODULE "${CMAKE_CURRENT_SOURCE_DIR}/build/PYTORCH_PROCESSING/libTorchProcessingModule.so") 
endif()

if( WIN32 )
SET(TORCH_PROCESS_MODULE "${CMAKE_CURRENT_SOURCE_DIR}/build/PYTORCH_PROCESSING/TorchProcessingModule.lib") 
endif()

#============================================================
#=========================FIND LIBRARIES=====================

if(NOT EXISTS ${QT_LIB_DIR})
message("QT LIBRARY NOT FOUND IN: ${LIBRARY_DIR}")
endif()

if(NOT EXISTS ${OPENCV_LIB_DIR})
message("OPENCV LIBRARY NOT FOUND IN: ${OPENCV_LIB_DIR}")
endif()

if(NOT EXISTS ${PYTORCH_DIR})
message("PYTORCH LIBRARY NOT FOUND IN: ${LIBRARY_DIR}/NN_LIBRARY")
endif()

list(APPEND CMAKE_PREFIX_PATH ${QT_LIB_DIR})
list(APPEND CMAKE_PREFIX_PATH ${OPENCV_LIB_DIR})
list(APPEND CMAKE_PREFIX_PATH ${PYTORCH_DIR})

find_package(Qt6 COMPONENTS Widgets Network Core SerialPort PrintSupport REQUIRED)
find_package(OpenCV REQUIRED )
#find_package(Torch REQUIRED)

message("OPENCV LIBS "${OpenCV_LIBS})
#=============================================================

#=========================INCLUDES============================
#include_directories("${PYTORCH_DIR}/include")
#include_directories("${PYTORCH_DIR}/include/torch/csrc/api/include")
include_directories("./gui_sources")
#include_directories("${PROJECT_SOURCE_DIR}/PYTORCH_PROCESSING") #SUBPROJECT PYTORCH_PROCESSING
include_directories("${PROJECT_SOURCE_DIR}/CV_IMAGE_PROCESSING") #SUBPROJECT PYTORCH_PROCESSING
include_directories("${PROJECT_SOURCE_DIR}/CV_AIM_IMITATION") #SUBPROJECT PYTORCH_PROCESSING
include_directories("${PROJECT_SOURCE_DIR}/UTILITES") #SUBPROJECT PYTORCH_PROCESSING

include_directories("${LIBRARY_DIR}/IMAGE_PROCESSING/opencv470/build/include")
include_directories("C:/HEADERS_STD")

#include_directories("${LIBRARY_DIR}/MATH_LIBRARY")
#include_directories("${LIBRARY_DIR}/COMMON_LIBRARRY/boost_1_68_0/boost_1_68_0")

#include_directories("${LIBRARY_DIR}/NN_LIBRARY/LIB_TORCH/onnxruntime/include/")
#link_directories("${LIBRARY_DIR}/NN_LIBRARY/ONNX/onnxruntime/lib/")

#include_directories("../${PROJECT_SOURCE_DIR}")

#=========================ARAVIS CAMRA LIBRARY ============================
include_directories("/usr/local/include/aravis-0.8")
include_directories("/usr/include/glib-2.0")
include_directories("/usr/lib/x86_64-linux-gnu/glib-2.0/include")

#=========================SUBPROJECTS============================
#add_subdirectory(rotate_optimization)
#include_directories("${PROJECT_SOURCE_DIR}/rotate_optimization")
#==============================================================

#======================LINK PROJECT===========================
add_executable (ImageProcProject
        "main.cpp"
        "./AIM_IMAGE_IMITATION/AimImageImitatorClass.cpp"
        "./AIM_IMAGE_IMITATION/AimImageImitatorClass.h"
        "./CV_IMAGE_PROCESSING/CVImageProcessing.cpp"
        "./CV_IMAGE_PROCESSING/CVImageProcessing.h"
        "./CAMERA_INTERFACE/CameraInterfaceClass.cpp"
        "./CAMERA_INTERFACE/CameraInterfaceClass.h"

        "./UTILITES/DebugOutputFilter.cpp"
        "./UTILITES/SettingsEngine.cpp"

        "./gui_sources/DisplayWidget.cpp"
        "./gui_sources/AdjustableWidget.cpp"
        "./gui_sources/graphicwidgetnode.cpp"
        "./gui_sources/labelimage.cpp"
        "./gui_sources/link_line.cpp"
        "./gui_sources/node.cpp"
        "./gui_sources/mainwindowclass.cpp"
        "./gui_sources/OutputImageDisplayClass.cpp"
        "./gui_sources/OutputTextDisplayClass.cpp"

        "./ui_files/DisplayWidget.ui"
        "./ui_files/mainwindowclass.ui"
        "./ui_files/mainblockwindow.ui"
        "./ui_files/OutputImageDisplay.ui"
        "./ui_files/OutputTextDisplay.ui"
        )
target_link_libraries(ImageProcProject ${OpenCV_LIBS})
target_link_libraries(ImageProcProject Qt6::Widgets)
target_link_libraries(ImageProcProject Qt6::Network)
target_link_libraries(ImageProcProject Qt6::Core)
target_link_libraries(ImageProcProject Qt6::PrintSupport)

target_link_libraries(ImageProcProject /lib/x86_64-linux-gnu/libglib-2.0.so)
target_link_libraries(ImageProcProject /lib/x86_64-linux-gnu/libgobject-2.0.so)
target_link_libraries(ImageProcProject /usr/local/lib/x86_64-linux-gnu/libaravis-0.8.so)


#target_link_libraries(ImageProcProject ${TORCH_PROCESS_MODULE})
#if (WIN32)
#target_link_libraries(ImageProcProject "${PYTORCH_DIR}/lib/torch_cpu.lib")
#target_link_libraries(ImageProcProject "${PYTORCH_DIR}/lib/torch.lib")
#target_link_libraries(ImageProcProject "${PYTORCH_DIR}/lib/c10.lib")
#endif (WIN32)
#
#if (UNIX)
#target_link_libraries(ImageProcProject "${PYTORCH_DIR}/lib/libtorch_cpu.so")
#target_link_libraries(ImageProcProject "${PYTORCH_DIR}/lib/libtorch.so")
#target_link_libraries(ImageProcProject "${PYTORCH_DIR}/lib/libc10.so")
#endif (UNIX)


#target_link_libraries(ImageProcProject onnxruntime.so)
#target_link_libraries(ImageProcProject "${CMAKE_CURRENT_SOURCE_DIR}/build/rotate_optimization/libRotateOptimization.a")

#link_directories("${LIBRARY_DIR}/NN_LIBRARY/libtorch/lib")
#target_link_libraries(ImageProcProject "/home/broms/PROGRAM_LIBRARY/NN_LIBRARY/libtorch/lib/libtorch_cpu.so")
#target_link_libraries(ImageProcProject "/home/broms/PROGRAM_LIBRARY/NN_LIBRARY/libtorch/lib/libtorch.so")
#target_link_libraries(ImageProcProject "/home/broms/PROGRAM_LIBRARY/NN_LIBRARY/libtorch/lib/libc10.so")

#message("ROTATE LIB: ${CMAKE_CURRENT_SOURCE_DIR}/build/libRotateOptimization.a")
#==========================================================
