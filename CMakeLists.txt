# 3.11 because of imported target "OpenMP::OpenMP_C"
cmake_minimum_required(VERSION 3.11)

project(fmdt)

# CMake options ---------------------------------------------------------------
# -----------------------------------------------------------------------------
option(FMDT_DETECT_EXE "compile the detection chain executable." ON)
option(FMDT_VISU_EXE "compile the visualization executable." ON)
option(FMDT_CHECK_EXE "compile the checking executable." ON)
option(FMDT_MAXRED_EXE "compile the max reduction executable." ON)
option(FMDT_DEBUG "build the project using debugging code" OFF)
option(FMDT_OPENCV_LINK "link with OpenCV library." OFF)
option(FMDT_AFF3CT_RUNTIME "link with AFF3CT for execution runtime." OFF)

if (FMDT_OPENCV_LINK OR FMDT_AFF3CT_RUNTIME)
	set(FMDT_CPP ON)
else()
	set(FMDT_CPP OFF)
endif()

# Print CMake options values --------------------------------------------------
# -----------------------------------------------------------------------------
message(STATUS "FMDT options: ")
message(STATUS "  * FMDT_DETECT_EXE: '${FMDT_DETECT_EXE}'")
message(STATUS "  * FMDT_VISU_EXE: '${FMDT_VISU_EXE}'")
message(STATUS "  * FMDT_CHECK_EXE: '${FMDT_CHECK_EXE}'")
message(STATUS "  * FMDT_MAXRED_EXE: '${FMDT_MAXRED_EXE}'")
message(STATUS "  * FMDT_DEBUG: '${FMDT_DEBUG}'")
message(STATUS "  * FMDT_OPENCV_LINK: '${FMDT_OPENCV_LINK}'")
message(STATUS "  * FMDT_AFF3CT_RUNTIME: '${FMDT_AFF3CT_RUNTIME}'")
message(STATUS "FMDT info: ")
message(STATUS "  * FMDT_CPP: '${FMDT_CPP}'")
message(STATUS "  * CMAKE_BUILD_TYPE: '${CMAKE_BUILD_TYPE}'")

# Check CMake options ---------------------------------------------------------
# -----------------------------------------------------------------------------
# empty...

# Define CMake variables ------------------------------------------------------
# -----------------------------------------------------------------------------
set(src_dir src)
set(inc_dir include)
set(exe_dir exe)
set(lib_dir lib)

# Compiler generic options ----------------------------------------------------
# -----------------------------------------------------------------------------
# enable C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
if (FMDT_CPP)
	# enable C++11
	set(CMAKE_CXX_STANDARD 11)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

# Specify the executable and lib output path ----------------------------------
# -----------------------------------------------------------------------------
set(EXECUTABLE_OUTPUT_PATH ${exe_dir})

# Check c-vector mandatory dependency -----------------------------------------
# -----------------------------------------------------------------------------
if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/c-vector/vec.h")
	message(FATAL_ERROR "'c-vector' can't be found, try to init the submodule with the following cmd:\n"
	                    "$ git submodule update --init -- ../lib/c-vector/")
endif()

# Declare source files to compile ---------------------------------------------
# -----------------------------------------------------------------------------
set(src_common_files
    ${src_dir}/common/args.c
    ${src_dir}/common/tools.c
    ${src_dir}/common/CCL/CCL_compute.c
    ${src_dir}/common/features/features_compute.c
    ${src_dir}/common/features/features_io.c
    ${src_dir}/common/image/image_compute.c
    ${src_dir}/common/image/image_io.c
    ${src_dir}/common/kNN/kNN_compute.c
    ${src_dir}/common/kNN/kNN_io.c
    ${src_dir}/common/motion/motion_compute.c
    ${src_dir}/common/motion/motion_io.c
    ${src_dir}/common/threshold/threshold_compute.c
    ${src_dir}/common/tracking/tracking_compute.c
    ${src_dir}/common/tracking/tracking_global.c
    ${src_dir}/common/tracking/tracking_io.c
    ${src_dir}/common/tracking/tracking_struct.c
    ${src_dir}/common/validation/validation_compute.c
    ${src_dir}/common/validation/validation_global.c
    ${src_dir}/common/validation/validation_io.c
    ${src_dir}/common/video/video_io.c
    ${lib_dir}/c-vector/vec.c)
list(APPEND fmdt_src_list ${src_common_files})

# Create binaries -------------------------------------------------------------
# -----------------------------------------------------------------------------
# objects
add_library(fmdt-common-obj OBJECT ${src_common_files})
list(APPEND fmdt_targets_list fmdt-common-obj)

# executables
if(FMDT_DETECT_EXE)
	set(src_detect_files
	    ${src_dir}/mains/detect.c)
	list(APPEND fmdt_src_list ${src_detect_files})
	add_executable(fmdt-detect-exe $<TARGET_OBJECTS:fmdt-common-obj> ${src_detect_files})
	list(APPEND fmdt_targets_list fmdt-detect-exe)
	set_target_properties(fmdt-detect-exe PROPERTIES OUTPUT_NAME fmdt-detect)
	if (FMDT_AFF3CT_RUNTIME)
		set(src_detectrt_common_files
			${src_dir}/common/aff3ct_wrapper/Video/Video.cpp
			${src_dir}/common/aff3ct_wrapper/Video/Video2.cpp
		    ${src_dir}/common/aff3ct_wrapper/CCL_LSL/CCL_LSL.cpp
		    ${src_dir}/common/aff3ct_wrapper/Features/Features_extractor.cpp
		    ${src_dir}/common/aff3ct_wrapper/Features/Features_merger_CCL_HI.cpp
		    ${src_dir}/common/aff3ct_wrapper/Features/Features_magnitude.cpp
		    ${src_dir}/common/aff3ct_wrapper/kNN_matcher/kNN_matcher.cpp
		    ${src_dir}/common/aff3ct_wrapper/Motion/Motion.cpp
		    ${src_dir}/common/aff3ct_wrapper/Logger/Logger_frame.cpp
		    ${src_dir}/common/aff3ct_wrapper/Logger/Logger_kNN.cpp
		    ${src_dir}/common/aff3ct_wrapper/Logger/Logger_motion.cpp
		    ${src_dir}/common/aff3ct_wrapper/Logger/Logger_RoIs.cpp
		    ${src_dir}/common/aff3ct_wrapper/Logger/Logger_tracks.cpp
		    ${src_dir}/common/aff3ct_wrapper/Threshold/Threshold.cpp
		    ${src_dir}/common/aff3ct_wrapper/Tracking/Tracking.cpp)
		list(APPEND fmdt_src_list ${src_detectrt_common_files})
        add_library(fmdt-detect-rt-common-obj OBJECT ${src_detectrt_common_files})
        list(APPEND fmdt_targets_list fmdt-detect-rt-common-obj)

        set(src_detectrt1_files
            ${src_dir}/mains/detect_rt.cpp)
		list(APPEND fmdt_src_list ${src_detectrt1_files})
        add_executable(fmdt-detect-rt-seq-exe
            $<TARGET_OBJECTS:fmdt-common-obj>
            $<TARGET_OBJECTS:fmdt-detect-rt-common-obj>
            ${src_detectrt1_files})
        list(APPEND fmdt_targets_list fmdt-detect-rt-seq-exe)
        set_target_properties(fmdt-detect-rt-seq-exe PROPERTIES OUTPUT_NAME fmdt-detect-rt-seq)

        add_executable(fmdt-detect-rt-pip-exe
            $<TARGET_OBJECTS:fmdt-common-obj> 
            $<TARGET_OBJECTS:fmdt-detect-rt-common-obj>
            ${src_detectrt1_files})
        list(APPEND fmdt_targets_list fmdt-detect-rt-pip-exe)
        set_target_properties(fmdt-detect-rt-pip-exe PROPERTIES OUTPUT_NAME fmdt-detect-rt-pip)
        target_compile_definitions(fmdt-detect-rt-pip-exe PUBLIC FMDT_ENABLE_PIPELINE)
        
        set(src_detectrt2_files
            ${src_dir}/mains/detect_rt2.cpp)
		list(APPEND fmdt_src_list ${src_detectrt2_files})
        add_executable(fmdt-detect-rt2-exe 
            $<TARGET_OBJECTS:fmdt-common-obj> 
            $<TARGET_OBJECTS:fmdt-detect-rt-common-obj>
            ${src_detectrt2_files})
		list(APPEND fmdt_targets_list fmdt-detect-rt2-exe)
		set_target_properties(fmdt-detect-rt2-exe PROPERTIES OUTPUT_NAME fmdt-detect-rt2)
	endif()
endif()
if(FMDT_VISU_EXE)
	set(src_visu_files
	    ${src_dir}/mains/visu.c)
	list(APPEND fmdt_src_list ${src_visu_files})
	add_executable(fmdt-visu-exe $<TARGET_OBJECTS:fmdt-common-obj> ${src_visu_files})
	list(APPEND fmdt_targets_list fmdt-visu-exe)
	set_target_properties(fmdt-visu-exe PROPERTIES OUTPUT_NAME fmdt-visu)
endif()
if(FMDT_CHECK_EXE)
	set(src_check_files
	    ${src_dir}/mains/check.c)
	list(APPEND fmdt_src_list ${src_check_files})
	add_executable(fmdt-check-exe $<TARGET_OBJECTS:fmdt-common-obj> ${src_check_files})
	list(APPEND fmdt_targets_list fmdt-check-exe)
	set_target_properties(fmdt-check-exe PROPERTIES OUTPUT_NAME fmdt-check)
endif()
if(FMDT_MAXRED_EXE)
	set(src_maxred_files
	    ${src_dir}/mains/maxred.c)
	list(APPEND fmdt_src_list ${src_maxred_files})
	add_executable(fmdt-maxred-exe $<TARGET_OBJECTS:fmdt-common-obj> ${src_maxred_files})
	list(APPEND fmdt_targets_list fmdt-maxred-exe)
	set_target_properties(fmdt-maxred-exe PROPERTIES OUTPUT_NAME fmdt-maxred)
endif()

macro(fmdt_set_source_files_properties files key value)
	foreach(_file IN ITEMS ${files})
		set_source_files_properties(${_file} PROPERTIES ${key} ${value})
	endforeach()
endmacro()

# force C++ language when linking with OpenCV and/or AFF3CT
if (FMDT_CPP)
	fmdt_set_source_files_properties("${fmdt_src_list}" LANGUAGE CXX)
endif()

# Add and compile sub-projects ------------------------------------------------
# -----------------------------------------------------------------------------
# ffmpeg-io
if (NOT TARGET ffmpeg-io-slib)
	if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/ffmpeg-io/include/constants.h")
		option(FFMPEG-IO_EXE "Forced option from '${CMAKE_PROJECT_NAME}'." OFF)
		add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/ffmpeg-io/)
	else()
		message(FATAL_ERROR "'ffmpeg-io' can't be found, try to init the submodule with the following cmd:\n"
		                    "$ git submodule update --init -- ../lib/ffmpeg-io/")
	endif()
endif()

# nrc
if (NOT TARGET nrc-slib)
	if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/nrc2/include/nrc.h")
		add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/nrc2/)
	else()
		message(FATAL_ERROR "'nrc2' can't be found, try to init the submodule with the following cmd:\n"
		                    "$ git submodule update --init -- ../lib/nrc2/")
	endif()
endif()

# aff3ct
if (FMDT_AFF3CT_RUNTIME)
	if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/aff3ct-core/include/aff3ct-core.hpp")
		option(AFF3CT_CORE_COMPILE_STATIC_LIB "" ON)
		option(AFF3CT_CORE_COMPILE_SHARED_LIB "" OFF)
		option(AFF3CT_CORE_TESTS "" OFF)
		add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/aff3ct-core/)
	else()
		message(FATAL_ERROR "'aff3ct-core' can't be found, try to init the submodule with the following cmd:\n"
		                    "$ git submodule update --init --recursive -- ../lib/aff3ct-core/")
	endif()
endif()

if (FMDT_OPENCV_LINK)
	find_package(OpenCV REQUIRED)
endif()

# LSL
if (NOT TARGET lsl-slib)
	if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/lsl/include/flsl.h")
		add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/lsl/)
	else()
		message(FATAL_ERROR "'lsl' can't be found, try to init the submodule with the following cmd:\n"
		                    "$ git submodule update --init -- ../lib/lsl/")
	endif()
endif()

# Add definitions -------------------------------------------------------------
# -----------------------------------------------------------------------------
macro(fmdt_target_compile_definitions targets privacy dir)
	foreach(_target IN ITEMS ${targets})
		target_compile_definitions(${_target} ${privacy} ${dir})
	endforeach()
endmacro()

if (FMDT_DEBUG)
	fmdt_target_compile_definitions("${fmdt_targets_list}" PUBLIC FMDT_ENABLE_DEBUG)
endif()
if (FMDT_OPENCV_LINK)
	fmdt_target_compile_definitions("${fmdt_targets_list}" PUBLIC FMDT_OPENCV_LINK)
endif()
if (FMDT_AFF3CT_RUNTIME)
	fmdt_target_compile_definitions("${fmdt_targets_list}" PUBLIC FMDT_AFF3CT_LINK)
endif()

# Set include directory -------------------------------------------------------
# -----------------------------------------------------------------------------
macro(fmdt_target_include_directories targets privacy dir)
	foreach(_target IN ITEMS ${targets})
		target_include_directories(${_target} ${privacy} ${dir})
	endforeach()
endmacro()

fmdt_target_include_directories("${fmdt_targets_list}" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${inc_dir}/c)
fmdt_target_include_directories("${fmdt_targets_list}" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${lib_dir}/c-vector)
if (FMDT_OPENCV_LINK)
	fmdt_target_include_directories("${fmdt_targets_list}" PUBLIC ${OpenCV_INCLUDE_DIRS})
endif()
if (FMDT_AFF3CT_RUNTIME)
	fmdt_target_include_directories("${fmdt_targets_list}" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${inc_dir}/cpp)
endif()

# Link with dependencies ------------------------------------------------------
# -----------------------------------------------------------------------------
macro(fmdt_target_link_libraries targets privacy lib)
	foreach(_target IN ITEMS ${targets})
		target_link_libraries(${_target} ${privacy} ${lib})
	endforeach()
endmacro()

fmdt_target_link_libraries("${fmdt_targets_list}" PUBLIC ffmpeg-io-slib)
fmdt_target_link_libraries("${fmdt_targets_list}" PUBLIC m)
fmdt_target_link_libraries("${fmdt_targets_list}" PUBLIC nrc-slib)
if (FMDT_OPENCV_LINK)
	fmdt_target_link_libraries("${fmdt_targets_list}" PUBLIC "${OpenCV_LIBS}")
endif()
if (FMDT_AFF3CT_RUNTIME)
	fmdt_target_link_libraries("${fmdt_targets_list}" PUBLIC aff3ct-core-static-lib)
endif()
if (TARGET lsl-simd-slib)
	fmdt_target_link_libraries("${fmdt_targets_list}" PRIVATE lsl-simd-slib)
else()
	fmdt_target_link_libraries("${fmdt_targets_list}" PRIVATE lsl-slib)
endif()
