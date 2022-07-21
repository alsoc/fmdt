# 3.11 because of imported target "OpenMP::OpenMP_C"
cmake_minimum_required(VERSION 3.11)

project(tau-h)

# CMake options ---------------------------------------------------------------
# -----------------------------------------------------------------------------
option(TAH_BALLON_EXE "Compile the detection chain executable." ON)
option(TAH_TRACKING_EXE "compile the tracking executable." ON)
option(TAH_VALIDATION_EXE "compile the tracking executable." ON)

# Print CMake options values --------------------------------------------------
# -----------------------------------------------------------------------------
message(STATUS "Tau-h options: ")
message(STATUS "  * TAH_BALLON_EXE: '${TAH_BALLON_EXE}'")
message(STATUS "  * TAH_TRACKING_EXE: '${TAH_TRACKING_EXE}'")
message(STATUS "  * TAH_VALIDATION_EXE: '${TAH_VALIDATION_EXE}'")

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

# Specify the executable and lib output path ----------------------------------
# -----------------------------------------------------------------------------
set(EXECUTABLE_OUTPUT_PATH ${exe_dir})

# Declare source files to compile ---------------------------------------------
# -----------------------------------------------------------------------------
set(src_common_files
    ${src_dir}/Args.c
    ${src_dir}/Features.c
    ${src_dir}/DebugUtil.c
    ${src_dir}/Tracking.c
    ${src_dir}/Validation.c
    ${src_dir}/Video.c)

# Create binaries -------------------------------------------------------------
# -----------------------------------------------------------------------------
# objects
add_library(tah-common-obj OBJECT ${src_common_files})
list(APPEND tah_targets_list tah-common-obj)

# executables
if(TAH_BALLON_EXE)
	set(src_ballon_files
	    ${src_dir}/Ballon.c
	    ${src_dir}/CCL_LSL.c
	    ${src_dir}/KPPV.c
	    ${src_dir}/Threshold.c
	    ${src_dir}/Main.c)
	add_executable(tah-ballon-exe $<TARGET_OBJECTS:tah-common-obj> ${src_ballon_files})
	list(APPEND tah_targets_list tah-ballon-exe)
	set_target_properties(tah-ballon-exe PROPERTIES OUTPUT_NAME ballon)
endif()
if(TAH_TRACKING_EXE)
	set(src_tracking_files
	    ${src_dir}/VideoTrack.c)
	add_executable(tah-tracking-exe $<TARGET_OBJECTS:tah-common-obj> ${src_tracking_files})
	list(APPEND tah_targets_list tah-tracking-exe)
	set_target_properties(tah-tracking-exe PROPERTIES OUTPUT_NAME tracking)
endif()
if(TAH_VALIDATION_EXE)
	set(src_validation_files
	    ${src_dir}/main_validation.c)
	add_executable(tah-validation-exe $<TARGET_OBJECTS:tah-common-obj> ${src_validation_files})
	list(APPEND tah_targets_list tah-validation-exe)
	set_target_properties(tah-validation-exe PROPERTIES OUTPUT_NAME validation)
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

# Add definitions -------------------------------------------------------------
# -----------------------------------------------------------------------------
macro(tah_target_compile_definitions targets privacy dir)
	foreach(_target IN ITEMS ${targets})
		target_compile_definitions(${_target} ${privacy} ${dir})
	endforeach()
endmacro()

# empty...

# Set include directory -------------------------------------------------------
# -----------------------------------------------------------------------------
macro(tah_target_include_directories targets privacy dir)
	foreach(_target IN ITEMS ${targets})
		target_include_directories(${_target} ${privacy} ${dir})
	endforeach()
endmacro()

tah_target_include_directories("${tah_targets_list}" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${inc_dir}/)

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
