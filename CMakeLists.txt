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

set(CMAKE_AUTOUIC_SEARCH_PATHS ./gui_files)

#===========================================================
function(CHECK_SET_LIB_PATH LIB_DIR_PATH COMPILER_NAME TORCH_NAME CV_NAME)
message("[ TRY LIB DIR ] ${LIB_DIR_PATH}")

if(NOT EXISTS ${LIB_DIR_PATH})
message("DIR NOT EXISTS: ${LIB_DIR_PATH}")
return()
endif()
        set(LIBRARY_DIR ${LIB_DIR_PATH} CACHE INTERNAL "${LIBRARY_DIR}")
        set(COMPILER ${COMPILER_NAME} CACHE INTERNAL "${COMPILER}")
        set(TORCH_VERSION ${TORCH_NAME} CACHE INTERNAL "${TORCH_VERSION}")

        set(PYTORCH_DIR "${LIBRARY_DIR}/NN_LIBRARY/LIBTORCH/${TORCH_VERSION}/" CACHE INTERNAL "${PYTORCH_DIR}")
        set(Torch_DIR ${PYTORCH_DIR} CACHE INTERNAL "${Torch_DIR}")

if(NOT EXISTS "${LIBRARY_DIR}/IMAGE_PROCESSING/OPENCV/${CV_NAME}")
message("DIR NOT EXISTS: ${LIBRARY_DIR}/IMAGE_PROCESSING/OPENCV/${CV_NAME}")
return()
endif()

        set(CV_VERSION ${CV_NAME} CACHE INTERNAL "${CV_VERSION}")
        set(OPENCV_LIB_DIR "${LIBRARY_DIR}/IMAGE_PROCESSING/OPENCV/${CV_VERSION}/" CACHE INTERNAL "${OPENCV_LIB_DIR}")

        set(QT_LIB_DIR "${LIBRARY_DIR}/Qt653" CACHE INTERNAL "${QT_LIB_DIR}")
        if(EXISTS "${LIBRARY_DIR}/Qt653/6.5.3/${COMPILER}/lib")
        set(QT_LIB_DIR "${LIBRARY_DIR}/Qt653/6.5.3/${COMPILER}" CACHE INTERNAL "${QT_LIB_DIR}")
        endif()


message("=============================================")
message("SET QT     LIBRARY - ${QT_LIB_DIR}")
message("SET OPENCV LIBRARY - ${LIBRARY_DIR}/IMAGE_PROCESSING/OPENCV/${CV_VERSION}")
message("SET TORCH  LIBRARY - ${PYTORCH_DIR}")
message("=============================================")

endfunction()

function(LINK_QT_LIBS PROJECT_NAME)
        target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
        target_link_libraries(${PROJECT_NAME} Qt6::Widgets)
        target_link_libraries(${PROJECT_NAME} Qt6::Network)
        target_link_libraries(${PROJECT_NAME} Qt6::Core)
        target_link_libraries(${PROJECT_NAME} Qt6::PrintSupport)
        message("[ LINK ] QT WIDGETS NETWORK CORE PRINT_SUPPORT")
endfunction()

function(LINK_ARAVIS_AARCH64 PROJECT_NAME)
        include_directories("/usr/local/include/aravis-0.8")
        include_directories("/usr/include/glib-2.0")
        include_directories("/usr/lib/x86_64-linux-gnu/glib-2.0/include")

        #target_link_libraries(ImageProcProject /usr/lib/aarch64-linux-gnu/libglib-2.0.so)
        #target_link_libraries(ImageProcProject /usr/lib/aarch64-linux-gnu/libgobject-2.0.so)
        #target_link_libraries(ImageProcProject /usr/local/lib/aarch64-linux-gnu/libaravis-0.8.so)
        #target_link_libraries(ImageProcProject /usr/lib/aarch64-linux-gnu/libusb-1.0.so)
        message("[ LINK ] ARAVIS AARCH64")
endfunction()

function(INCLUDE_ARAVIS_HEADERS PROJECT_NAME)
        include_directories("/usr/local/include/aravis-0.8")
        include_directories("/usr/include/glib-2.0")
        include_directories("/usr/lib/x86_64-linux-gnu/glib-2.0/include")
endfunction()

function(LINK_ARAVIS_X86 PROJECT_NAME)
        include_directories("/usr/local/include/aravis-0.8")
        include_directories("/usr/include/glib-2.0")
        include_directories("/usr/lib/x86_64-linux-gnu/glib-2.0/include")

        #target_link_libraries(ImageProcProject /lib/x86_64-linux-gnu/libglib-2.0.so)
        #target_link_libraries(ImageProcProject /lib/x86_64-linux-gnu/libgobject-2.0.so)
        #target_link_libraries(ImageProcProject /usr/local/lib/x86_64-linux-gnu/libaravis-0.8.so)
        message("[ LINK ] ARAVIS X86")
endfunction()

function(LINK_TORCH PROJECT_NAME)
        target_link_libraries(ImageProcProject ${TORCH_PROCESS_MODULE})
        if (WIN32)
        target_link_libraries(ImageProcProject "${PYTORCH_DIR}/lib/torch_cpu.lib")
        target_link_libraries(ImageProcProject "${PYTORCH_DIR}/lib/torch.lib")
        target_link_libraries(ImageProcProject "${PYTORCH_DIR}/lib/c10.lib")
        endif (WIN32)

        if (UNIX)
        target_link_libraries(ImageProcProject "${PYTORCH_DIR}/lib/libtorch_cpu.so")
        target_link_libraries(ImageProcProject "${PYTORCH_DIR}/lib/libtorch.so")
        target_link_libraries(ImageProcProject "${PYTORCH_DIR}/lib/libc10.so")
        endif (UNIX)

        include_directories("${PYTORCH_DIR}/include")
        include_directories("${PYTORCH_DIR}/include/torch/csrc/api/include")

        #link_directories("${LIBRARY_DIR}/NN_LIBRARY/libtorch/lib")
        #target_link_libraries(ImageProcProject "/home/broms/PROGRAM_LIBRARY/NN_LIBRARY/libtorch/lib/libtorch_cpu.so")
        #target_link_libraries(ImageProcProject "/home/broms/PROGRAM_LIBRARY/NN_LIBRARY/libtorch/lib/libtorch.so")
        #target_link_libraries(ImageProcProject "/home/broms/PROGRAM_LIBRARY/NN_LIBRARY/libtorch/lib/libc10.so")

        #====================SUBPROJECTS=============================
        #add_subdirectory(PYTORCH_PROCESSING)
        if( UNIX )
        SET(TORCH_PROCESS_MODULE "${CMAKE_CURRENT_SOURCE_DIR}/build/PYTORCH_PROCESSING/libTorchProcessingModule.so") 
        endif()

        if( WIN32 )
        SET(TORCH_PROCESS_MODULE "${CMAKE_CURRENT_SOURCE_DIR}/build/PYTORCH_PROCESSING/TorchProcessingModule.lib") 
        endif()
        #============================================================

        message("[ LINK ] TORCH")
endfunction()

set(HIK_LIB_DIR "/home/broms/LIBRARY/CAMERA_LIBRARY/HIKROBOT/lib/64/")
link_directories(${HIK_LIB_DIR})
message("LINK LIBS: ${HIK_LIB_DIR}")

function(LINK_HIK_VISION PROJECT_NAME)

        target_link_libraries(ImageProcProject libMvCameraControl.so)
        target_link_libraries(ImageProcProject libMvUsb3vTL.so)
        target_link_libraries(ImageProcProject libMvSDKVersion.so)
        target_link_libraries(ImageProcProject libMediaProcess.so)

        include_directories("/home/broms/LIBRARY/CAMERA_LIBRARY/HIKROBOT/include")
message("[ LINK ] HIK CAMERA")

endfunction()

function(LINK_ROTATE_OPTIMIZATION PROJECT_NAME)
        target_link_libraries(ImageProcProject "${CMAKE_CURRENT_SOURCE_DIR}/build/ROTATE_OPTIMIZATION/RotationContainerLib/libRotateOptimization.so")

#=========================SUBPROJECTS============================
add_subdirectory("ROTATE_OPTIMIZATION/RotationContainerLib")
include_directories("${PROJECT_SOURCE_DIR}/ROTATE_OPTIMIZATION/RotationContainerLib")
include_directories("${PROJECT_SOURCE_DIR}/ROTATE_OPTIMIZATION")
#==============================================================

        message("[ LINK ] ROTATE OPTIMIZATION")
endfunction()

function(LINK_ONNX PROJECT_NAME)
        include_directories("${LIBRARY_DIR}/NN_LIBRARY/LIB_TORCH/onnxruntime/include/")
        target_link_libraries(ImageProcProject onnxruntime.so)
        message("[ LINK ] ONNX")
endfunction()


function(INCLUDE_PROJECT_DIRS)
        include_directories("./gui_sources")
        include_directories("./gui_sources/GRAPHICS_WINDOW")
        include_directories("${PROJECT_SOURCE_DIR}/CV_IMAGE_PROCESSING") 
        include_directories("${PROJECT_SOURCE_DIR}/CV_IMAGE_PROCESSING/TRACKER_ROBUST") 
        include_directories("${PROJECT_SOURCE_DIR}/CV_AIM_IMITATION")
        include_directories("${PROJECT_SOURCE_DIR}/UTILITES") 
        include_directories("${PROJECT_SOURCE_DIR}/COMMON") 
        include_directories("${PROJECT_SOURCE_DIR}/AIMING_MODULES") 
        include_directories("${PROJECT_SOURCE_DIR}/DEVICES") 
        include_directories("${PROJECT_SOURCE_DIR}/DEVICES/LaserPointerControlLib") 
        include_directories("${PROJECT_SOURCE_DIR}/DEVICES/MotorImitatorLib") 
        include_directories("${PROJECT_SOURCE_DIR}/TRANSMISSION/DATA_STRUCTURES") 
        include_directories("${PROJECT_SOURCE_DIR}/TRANSMISSION/EMBEDED_RING_BUFFER") 
        include_directories("${PROJECT_SOURCE_DIR}/TRANSMISSION/GENERIC_MESSAGES") 
        include_directories("${PROJECT_SOURCE_DIR}/TRANSMISSION/TRANSMISSION_MODULES") 
        message("[ INCLUDE ] PROJECT DIRS")
endfunction()

function(INCLUDE_STANDART_DIRS)
include_directories("/usr/include/c++/11")
#include_directories("/home/orangepi/LIBRARY/LIB_STD_INCLUDE/")
#include_directories("/home/orangepi/LIBRARY/LIB_STD/")
#include_directories("${LIBRARY_DIR}/COMMON_LIBRARRY/boost_1_68_0/boost_1_68_0")
#link_directories("${LIBRARY_DIR}/NN_LIBRARY/ONNX/onnxruntime/lib/")
#include_directories("${PROJECT_SOURCE_DIR}/PYTORCH_PROCESSING") #SUBPROJECT PYTORCH_PROCESSING
include_directories("${LIBRARY_DIR}/MATH_LIBRARY/Eigen")
include_directories("${LIBRARY_DIR}/MATH_LIBRARY/")
endfunction()

#===========================================================




#===========================================================
        set(GUI_FILES_LIST "")
list(APPEND GUI_FILES_LIST "./gui_files/widget_processing_image_control.ui")
list(APPEND GUI_FILES_LIST "./gui_files/widget_processing_image_generic.ui")
list(APPEND GUI_FILES_LIST "./gui_files/widget_processing_image_control.ui")
list(APPEND GUI_FILES_LIST "./gui_files/widget_camera_control.ui")
list(APPEND GUI_FILES_LIST "./gui_files/widget_imitator_control.ui")
list(APPEND GUI_FILES_LIST "./gui_files/widget_container_group.ui")
list(APPEND GUI_FILES_LIST "./gui_files/widget_table_group.ui")
list(APPEND GUI_FILES_LIST "./gui_files/widget_main_window.ui")
list(APPEND GUI_FILES_LIST "./gui_files/widget_output_image_display.ui")
list(APPEND GUI_FILES_LIST "./gui_files/widget_output_text_display.ui")
list(APPEND GUI_FILES_LIST "./gui_files/widget_scanator_control.ui")
list(APPEND GUI_FILES_LIST "./gui_files/widget_aiming_control.ui")
list(APPEND GUI_FILES_LIST "./gui_files/pictures.qrc")

        set(GUI_SOURCES_LIST "")
list(APPEND GUI_SOURCES_LIST "./gui_sources/widget_process_controller.cpp")
list(APPEND GUI_SOURCES_LIST "./gui_sources/widget_processing_image_generic.cpp")
list(APPEND GUI_SOURCES_LIST "./gui_sources/widget_imitator_control.cpp")
list(APPEND GUI_SOURCES_LIST "./gui_sources/widget_container_group.cpp")
list(APPEND GUI_SOURCES_LIST "./gui_sources/widget_table_group.cpp")
list(APPEND GUI_SOURCES_LIST "./gui_sources/widget_processing_image_control.cpp")
list(APPEND GUI_SOURCES_LIST "./gui_sources/widget_camera_control.cpp")
list(APPEND GUI_SOURCES_LIST "./gui_sources/widget_adjustable.cpp")
list(APPEND GUI_SOURCES_LIST "./gui_sources/widget_scene_node_interface.cpp")
list(APPEND GUI_SOURCES_LIST "./gui_sources/labelimage.cpp")
list(APPEND GUI_SOURCES_LIST "./gui_sources/link_line.cpp")
list(APPEND GUI_SOURCES_LIST "./gui_sources/node.cpp")
list(APPEND GUI_SOURCES_LIST "./gui_sources/widget_main_window.cpp")
list(APPEND GUI_SOURCES_LIST "./gui_sources/widget_output_image_display.cpp")
list(APPEND GUI_SOURCES_LIST "./gui_sources/widget_output_text_display.cpp")
list(APPEND GUI_SOURCES_LIST "./gui_sources/qcustomplot.cpp")
list(APPEND GUI_SOURCES_LIST "./gui_sources/widget_scanator_control.cpp")
list(APPEND GUI_SOURCES_LIST "./gui_sources/widget_aiming_control.cpp")
list(APPEND GUI_SOURCES_LIST "./gui_sources/GRAPHICS_WINDOW/widget_graphics_plot.cpp")
list(APPEND GUI_SOURCES_LIST "./gui_sources/GRAPHICS_WINDOW/widget_graphics_plot.ui")

        set(IMAGE_PROCESSING_SOURCES "")
list(APPEND IMAGE_PROCESSING_SOURCES "./CV_IMAGE_PROCESSING/image_processing_node.cpp")
list(APPEND IMAGE_PROCESSING_SOURCES "./CV_IMAGE_PROCESSING/tracker_centroid.cpp")
list(APPEND IMAGE_PROCESSING_SOURCES "./CV_IMAGE_PROCESSING/tracker_template.cpp")
list(APPEND IMAGE_PROCESSING_SOURCES "./CV_IMAGE_PROCESSING/tracker_neural.cpp")
list(APPEND IMAGE_PROCESSING_SOURCES "./CV_IMAGE_PROCESSING/image_finder.cpp")
list(APPEND IMAGE_PROCESSING_SOURCES "./CV_IMAGE_PROCESSING/filter_blotch.cpp")
list(APPEND IMAGE_PROCESSING_SOURCES "./CV_IMAGE_PROCESSING/contour_processing.cpp")
list(APPEND IMAGE_PROCESSING_SOURCES "./CV_IMAGE_PROCESSING/image_estimations.cpp")
list(APPEND IMAGE_PROCESSING_SOURCES "./CV_IMAGE_PROCESSING/optimization_threshold.cpp")
list(APPEND IMAGE_PROCESSING_SOURCES "./CV_IMAGE_PROCESSING/TRACKER_ROBUST/Tracker.cpp")

        set(TRANSMISSION_MODULE_SOURCES "")
list(APPEND TRANSMISSION_MODULE_SOURCES "./TRANSMISSION/DATA_STRUCTURES/message_command_structures.cpp")
list(APPEND TRANSMISSION_MODULE_SOURCES "./TRANSMISSION/DATA_STRUCTURES/engine_type_register.cpp")
list(APPEND TRANSMISSION_MODULE_SOURCES "./TRANSMISSION/TRANSMISSION_MODULES/engine_udp_interface.cpp")
list(APPEND TRANSMISSION_MODULE_SOURCES "./TRANSMISSION/TRANSMISSION_MODULES/engine_tcp_interface.cpp")

        set(UTILITIES_SOURCES "")
list(APPEND UTILITIES_SOURCES "./UTILITES/debug_output_filter.cpp")
list(APPEND UTILITIES_SOURCES "./UTILITES/register_settings.cpp")
list(APPEND UTILITIES_SOURCES "./UTILITES/engine_statistics.cpp")
list(APPEND UTILITIES_SOURCES "./COMMON/interface_pass_coord.cpp")
list(APPEND UTILITIES_SOURCES "./COMMON/interface_image_source.cpp")


        set(SIGNAL_IMITATOR_SOURCES "")
list(APPEND SIGNAL_IMITATOR_SOURCES "./AIM_IMAGE_IMITATION/imitator_image_aim.cpp")
list(APPEND SIGNAL_IMITATOR_SOURCES "./AIM_IMAGE_IMITATION/sinus_generator_class.cpp")
list(APPEND SIGNAL_IMITATOR_SOURCES "./AIM_IMAGE_IMITATION/widget_sinus_source.cpp")
list(APPEND SIGNAL_IMITATOR_SOURCES "./AIM_IMAGE_IMITATION/widget_sinus_source.ui")

         set(DEVICE_CONTROL_SOURCES "")
list(APPEND DEVICE_CONTROL_SOURCES "./DEVICES/scanator_control_class.cpp")
list(APPEND DEVICE_CONTROL_SOURCES "./CAMERA_INTERFACE/interface_camera_hik.cpp")
#list(APPEND DEVICE_CONTROL_SOURCES "./CAMERA_INTERFACE/camera_interface_class.cpp")

#=========================FIND LIBRARIES=====================

        message("=============================================")
        #CHECK_SET_LIB_PATH("/home/orangepi/LIBRARY"  "gcc_64" "libtorch" "opencv411")
        #CHECK_SET_LIB_PATH("/home/broms/LIBRARY"     "gcc_64" "libtorch" "opencv411")
        #CHECK_SET_LIB_PATH("D:/LIBRARY" "msvc2019_64" "libtorch113" "opencv411")
        CHECK_SET_LIB_PATH("/home/broms/LIBRARY" "gcc_64" "libtorch" "opencv412")
        message("=============================================")

        list(APPEND CMAKE_PREFIX_PATH ${QT_LIB_DIR})
        list(APPEND CMAKE_PREFIX_PATH ${OPENCV_LIB_DIR})
        list(APPEND CMAKE_PREFIX_PATH ${PYTORCH_DIR})

        find_package(Qt6 COMPONENTS Widgets Network Core PrintSupport REQUIRED)
        find_package(OpenCV REQUIRED )
        #find_package(Torch REQUIRED)

#=============================================================
        INCLUDE_PROJECT_DIRS()
        INCLUDE_STANDART_DIRS()
#======================LINK PROJECT===========================
        add_executable (ImageProcProject
                "main.cpp"
                "controller_process_class.cpp"

                "./AIMING_MODULES/aiming_class.cpp"
                "./AIMING_MODULES/pid_class.cpp"
                "./AIMING_MODULES/rotate_vector_class.cpp"
                "./ROTATE_OPTIMIZATION/rotation_find_executor.cpp"

                ${GUI_FILES_LIST}
                ${GUI_SOURCES_LIST}
                ${IMAGE_PROCESSING_SOURCES}
                ${TRANSMISSION_MODULE_SOURCES}
                ${UTILITIES_SOURCES}
                ${SIGNAL_IMITATOR_SOURCES}
                ${DEVICE_CONTROL_SOURCES}
                )

        message("=============================================")
        LINK_QT_LIBS(ImageProcProject)
        LINK_TORCH(ImageProcProject)
        LINK_HIK_VISION(ImageProcProject)
        LINK_ROTATE_OPTIMIZATION(ImageProcProject)
        #LINK_ONNX(ImageProcProject)
        #LINK_ARAVIS_AARCH64(ImageProcProject)
        #LINK_ARAVIS_X86(ImageProcProject)
        INCLUDE_ARAVIS_HEADERS(ImageProcProject)
        message("=============================================")
        message("OPENCV LIBS "${OpenCV_LIBS})
#==========================================================

#target_link_libraries(ImageProcProject /usr/local/lib/aarch64-linux-gnu/libaravis-0.10.so)
#target_link_libraries(ImageProcProject "${CMAKE_CURRENT_SOURCE_DIR}/DEVICES/LaserPointerControlLib/libLaserControlLib.so")
#target_link_libraries(ImageProcProject "${CMAKE_CURRENT_SOURCE_DIR}/DEVICES/MotorImitatorLib/libMotorControlLib.so")