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

# Declare source files to compile ---------------------------------------------
# -----------------------------------------------------------------------------
set(src_common_files
    ${src_dir}/common/args.c
    ${src_dir}/common/features.c
    ${src_dir}/common/tools.c
    ${src_dir}/common/tracking.c
    ${src_dir}/common/validation.c
    ${src_dir}/common/video.c)
list(APPEND fmdt_src_list ${src_common_files})

# Create binaries -------------------------------------------------------------
# -----------------------------------------------------------------------------
# objects
add_library(fmdt-common-obj OBJECT ${src_common_files})
list(APPEND fmdt_targets_list fmdt-common-obj)

# executables
if(FMDT_DETECT_EXE)
	set(src_detect_common_files
	    ${src_dir}/detect/CCL.c
	    ${src_dir}/detect/KPPV.c
	    ${src_dir}/detect/threshold.c)
	list(APPEND fmdt_src_list ${src_detect_common_files})
	add_library(fmdt-detect-common-obj OBJECT ${src_detect_common_files})
	list(APPEND fmdt_targets_list fmdt-detect-common-obj)
	set(src_detect_files
	    ${src_dir}/detect/main.c)
	list(APPEND fmdt_src_list ${src_detect_files})
	add_executable(fmdt-detect-exe $<TARGET_OBJECTS:fmdt-common-obj> $<TARGET_OBJECTS:fmdt-detect-common-obj> ${src_detect_files})
	list(APPEND fmdt_targets_list fmdt-detect-exe)
	set_target_properties(fmdt-detect-exe PROPERTIES OUTPUT_NAME fmdt-detect)
	if (FMDT_AFF3CT_RUNTIME)
		set(src_detectrt_common_files
		    ${src_dir}/runtime/CCL_LSL/CCL_LSL.cpp
		    ${src_dir}/runtime/Features/Features_extractor.cpp
		    ${src_dir}/runtime/Features/Features_merger.cpp
		    ${src_dir}/runtime/Features/Features_motion.cpp
		    ${src_dir}/runtime/KNN_matcher/KNN_matcher.cpp
		    ${src_dir}/runtime/Logger/Logger_frame.cpp
		    ${src_dir}/runtime/Logger/Logger_KNN.cpp
		    ${src_dir}/runtime/Logger/Logger_motion.cpp
		    ${src_dir}/runtime/Logger/Logger_ROI.cpp
		    ${src_dir}/runtime/Logger/Logger_track.cpp
		    ${src_dir}/runtime/Threshold/Threshold.cpp
		    ${src_dir}/runtime/Tracking/Tracking.cpp)
		list(APPEND fmdt_src_list ${src_detectrt_common_files})
        add_library(fmdt-detectrt-common-obj OBJECT ${src_detectrt_common_files})
        list(APPEND fmdt_targets_list fmdt-detectrt-common-obj)

        set(src_detectrt1_files
            ${src_dir}/runtime/Video/Video.cpp
            ${src_dir}/detect/main_rt.cpp)
		list(APPEND fmdt_src_list ${src_detectrt1_files})
        add_executable(fmdt-detect-rt-exe 
            $<TARGET_OBJECTS:fmdt-common-obj> 
            $<TARGET_OBJECTS:fmdt-detect-common-obj> 
            $<TARGET_OBJECTS:fmdt-detectrt-common-obj> 
            ${src_detectrt1_files})
        list(APPEND fmdt_targets_list fmdt-detect-rt-exe)
        set_target_properties(fmdt-detect-rt-exe PROPERTIES OUTPUT_NAME fmdt-detect-rt)
        
        set(src_detectrt2_files
            ${src_dir}/runtime/Video/Video2.cpp
            ${src_dir}/detect/main_rt2.cpp)
		list(APPEND fmdt_src_list ${src_detectrt2_files})
        add_executable(fmdt-detect-rt2-exe 
            $<TARGET_OBJECTS:fmdt-common-obj> 
            $<TARGET_OBJECTS:fmdt-detect-common-obj> 
            $<TARGET_OBJECTS:fmdt-detectrt-common-obj> 
            ${src_detectrt2_files})
		list(APPEND fmdt_targets_list fmdt-detect-rt2-exe)
		set_target_properties(fmdt-detect-rt2-exe PROPERTIES OUTPUT_NAME fmdt-detect-rt2)
	endif()
endif()
if(FMDT_VISU_EXE)
	set(src_visu_files
	    ${src_dir}/visu/main.c)
	list(APPEND fmdt_src_list ${src_visu_files})
	add_executable(fmdt-visu-exe $<TARGET_OBJECTS:fmdt-common-obj> ${src_visu_files})
	list(APPEND fmdt_targets_list fmdt-visu-exe)
	set_target_properties(fmdt-visu-exe PROPERTIES OUTPUT_NAME fmdt-visu)
endif()
if(FMDT_CHECK_EXE)
	set(src_check_files
	    ${src_dir}/check/main.c)
	list(APPEND fmdt_src_list ${src_check_files})
	add_executable(fmdt-check-exe $<TARGET_OBJECTS:fmdt-common-obj> ${src_check_files})
	list(APPEND fmdt_targets_list fmdt-check-exe)
	set_target_properties(fmdt-check-exe PROPERTIES OUTPUT_NAME fmdt-check)
endif()
if(FMDT_MAXRED_EXE)
	set(src_maxred_files
	    ${src_dir}/maxred/main.c)
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

# force C++ language when linking with OpenCV
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
		add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/aff3ct-core/)
	else()
		message(FATAL_ERROR "'aff3ct-core' can't be found, try to init the submodule with the following cmd:\n"
		                    "$ git submodule update --init --recursive -- ../lib/aff3ct-core/")
	endif()
endif()

if (FMDT_OPENCV_LINK)
	find_package(OpenCV REQUIRED)
endif()

# Add definitions -------------------------------------------------------------
# -----------------------------------------------------------------------------
macro(fmdt_target_compile_definitions targets privacy dir)
	foreach(_target IN ITEMS ${targets})
		target_compile_definitions(${_target} ${privacy} ${dir} )
	endforeach()
endmacro()

if (FMDT_DEBUG)
	fmdt_target_compile_definitions("${fmdt_targets_list}" PUBLIC ENABLE_DEBUG)
endif()
if (FMDT_OPENCV_LINK)
	fmdt_target_compile_definitions("${fmdt_targets_list}" PUBLIC OPENCV_LINK)
endif()
if (FMDT_AFF3CT_RUNTIME)
	fmdt_target_compile_definitions("${fmdt_targets_list}" PUBLIC AFF3CT_LINK)
endif()

# Set include directory -------------------------------------------------------
# -----------------------------------------------------------------------------
macro(fmdt_target_include_directories targets privacy dir)
	foreach(_target IN ITEMS ${targets})
		target_include_directories(${_target} ${privacy} ${dir})
	endforeach()
endmacro()

fmdt_target_include_directories("${fmdt_targets_list}" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${inc_dir}/common)
fmdt_target_include_directories("${fmdt_targets_list}" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${inc_dir}/detect)
if (FMDT_OPENCV_LINK)
	include_directories(${OpenCV_INCLUDE_DIRS})
endif()
if (FMDT_AFF3CT_RUNTIME)
	fmdt_target_include_directories("${fmdt_targets_list}" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${inc_dir}/runtime)
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
