# 3.11 because of imported target "OpenMP::OpenMP_C"
cmake_minimum_required(VERSION 3.11)

project(tau-h)

# CMake options ---------------------------------------------------------------
# -----------------------------------------------------------------------------
option(TAH_DETECT_EXE "compile the detection chain executable." ON)
option(TAH_VISU_EXE "compile the visualization executable." ON)
option(TAH_CHECK_EXE "compile the checking executable." ON)
option(TAH_MAXRED_EXE "compile the max reduction executable." ON)
option(TAH_DEBUG "build the project using debugging code" OFF)
option(TAH_OPENCV_LINK "link with OpenCV library." OFF)

# Print CMake options values --------------------------------------------------
# -----------------------------------------------------------------------------
message(STATUS "Tau-h options: ")
message(STATUS "  * TAH_DETECT_EXE: '${TAH_DETECT_EXE}'")
message(STATUS "  * TAH_VISU_EXE: '${TAH_VISU_EXE}'")
message(STATUS "  * TAH_CHECK_EXE: '${TAH_CHECK_EXE}'")
message(STATUS "  * TAH_MAXRED_EXE: '${TAH_MAXRED_EXE}'")
message(STATUS "  * TAH_DEBUG: '${TAH_DEBUG}'")
message(STATUS "  * TAH_OPENCV_LINK: '${TAH_OPENCV_LINK}'")
message(STATUS "Tau-h info: ")
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
# enable C99
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
if (TAH_OPENCV_LINK)
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
list(APPEND tah_src_list ${src_common_files})

# Create binaries -------------------------------------------------------------
# -----------------------------------------------------------------------------
# objects
add_library(tah-common-obj OBJECT ${src_common_files})
list(APPEND tah_targets_list tah-common-obj)

# executables
if(TAH_DETECT_EXE)
	set(src_detect_files
	    ${src_dir}/detect/ballon.c
	    ${src_dir}/detect/CCL.c
	    ${src_dir}/detect/KPPV.c
	    ${src_dir}/detect/threshold.c
	    ${src_dir}/detect/main.c)
	list(APPEND tah_src_list ${src_detect_files})
	add_executable(tah-detect-exe $<TARGET_OBJECTS:tah-common-obj> ${src_detect_files})
	list(APPEND tah_targets_list tah-detect-exe)
	set_target_properties(tah-detect-exe PROPERTIES OUTPUT_NAME meteor-detect)
endif()
if(TAH_VISU_EXE)
	set(src_visu_files
	    ${src_dir}/visu/main.c)
	list(APPEND tah_src_list ${src_visu_files})
	add_executable(tah-visu-exe $<TARGET_OBJECTS:tah-common-obj> ${src_visu_files})
	list(APPEND tah_targets_list tah-visu-exe)
	set_target_properties(tah-visu-exe PROPERTIES OUTPUT_NAME meteor-visu)
endif()
if(TAH_CHECK_EXE)
	set(src_check_files
	    ${src_dir}/check/main.c)
	list(APPEND tah_src_list ${src_check_files})
	add_executable(tah-check-exe $<TARGET_OBJECTS:tah-common-obj> ${src_check_files})
	list(APPEND tah_targets_list tah-check-exe)
	set_target_properties(tah-check-exe PROPERTIES OUTPUT_NAME meteor-check)
endif()
if(TAH_MAXRED_EXE)
	set(src_maxred_files
	    ${src_dir}/maxred/main.c)
	list(APPEND tah_src_list ${src_maxred_files})
	add_executable(tah-maxred-exe $<TARGET_OBJECTS:tah-common-obj> ${src_maxred_files})
	list(APPEND tah_targets_list tah-maxred-exe)
	set_target_properties(tah-maxred-exe PROPERTIES OUTPUT_NAME meteor-maxred)
endif()

macro(tah_set_source_files_properties files key value)
	foreach(_file IN ITEMS ${files})
		set_source_files_properties(${_file} PROPERTIES ${key} ${value})
	endforeach()
endmacro()

# force C++ language when linking with OpenCV
if (TAH_OPENCV_LINK)
	tah_set_source_files_properties("${tah_src_list}" LANGUAGE CXX)
endif()

# Add and compile sub-projects ------------------------------------------------
# -----------------------------------------------------------------------------
# FFmpeg-IO
option(FFMPEG-IO_EXE "Forced option from '${CMAKE_PROJECT_NAME}'." OFF)
if (NOT TARGET ffmpeg-io-slib)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/ffmpeg-io/)
endif()

if (NOT TARGET nrc-slib)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/nrc2/)
endif()

if (TAH_OPENCV_LINK)
	find_package(OpenCV REQUIRED)
endif()

# Add definitions -------------------------------------------------------------
# -----------------------------------------------------------------------------
macro(tah_target_compile_definitions targets privacy dir)
	foreach(_target IN ITEMS ${targets})
		target_compile_definitions(${_target} ${privacy} ${dir} )
	endforeach()
endmacro()

if (TAH_DEBUG)
	tah_target_compile_definitions("${tah_targets_list}" PUBLIC ENABLE_DEBUG)
endif()
if (TAH_OPENCV_LINK)
	tah_target_compile_definitions("${tah_targets_list}" PUBLIC OPENCV_LINK)
endif()

# Set include directory -------------------------------------------------------
# -----------------------------------------------------------------------------
macro(tah_target_include_directories targets privacy dir)
	foreach(_target IN ITEMS ${targets})
		target_include_directories(${_target} ${privacy} ${dir})
	endforeach()
endmacro()

tah_target_include_directories("${tah_targets_list}" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${inc_dir}/common)
tah_target_include_directories("${tah_targets_list}" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${inc_dir}/detect)
if (TAH_OPENCV_LINK)
	include_directories(${OpenCV_INCLUDE_DIRS})
endif()

# Link with dependencies ------------------------------------------------------
# -----------------------------------------------------------------------------
macro(tah_target_link_libraries targets privacy lib)
	foreach(_target IN ITEMS ${targets})
		target_link_libraries(${_target} ${privacy} ${lib})
	endforeach()
endmacro()

tah_target_link_libraries("${tah_targets_list}" PUBLIC ffmpeg-io-slib)
tah_target_link_libraries("${tah_targets_list}" PUBLIC m)
tah_target_link_libraries("${tah_targets_list}" PUBLIC nrc-slib)
if (TAH_OPENCV_LINK)
	tah_target_link_libraries("${tah_targets_list}" PUBLIC "${OpenCV_LIBS}")
endif()
