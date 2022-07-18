# 3.11 because of imported target "OpenMP::OpenMP_C"
cmake_minimum_required(VERSION 3.11)

project(tau-h)

# CMake options ---------------------------------------------------------------
# -----------------------------------------------------------------------------
option(TH_BALLON_EXE "Compile the detection chain executable." ON)
option(TH_TRACKING_EXE "compile the tracking executable." ON)

# Print CMake options values --------------------------------------------------
# -----------------------------------------------------------------------------
message(STATUS "Tau-h options: ")
message(STATUS "  * TH_BALLON_EXE: '${TH_BALLON_EXE}'")
message(STATUS "  * TH_TRACKING_EXE: '${TH_TRACKING_EXE}'")

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
    ${src_dir}/nrutil.c
    ${src_dir}/Tracking.c
    ${src_dir}/Validation.c
    ${src_dir}/Video.c)

# Create binaries -------------------------------------------------------------
# -----------------------------------------------------------------------------
# object
add_library(th-common-obj OBJECT ${src_common_files})
# executables
if(TH_BALLON_EXE)
	set(src_ballon_files
	    ${src_dir}/Ballon.c
	    ${src_dir}/CCL_LSL.c
	    ${src_dir}/KPPV.c
	    ${src_dir}/Threshold.c
	    ${src_dir}/Main.c)
	add_executable(th-ballon-exe $<TARGET_OBJECTS:th-common-obj> ${src_ballon_files})
	set_target_properties(th-ballon-exe PROPERTIES OUTPUT_NAME ballon)
endif()
if(TH_TRACKING_EXE)
	set(src_tracking_files
	    ${src_dir}/VideoTrack.c)
	add_executable(th-tracking-exe $<TARGET_OBJECTS:th-common-obj> ${src_tracking_files})
	set_target_properties(th-tracking-exe PROPERTIES OUTPUT_NAME tracking)
endif()

# Add and compile sub-projects ------------------------------------------------
# -----------------------------------------------------------------------------
# FFmpeg-IO
option(FFMPEG-IO_EXE "Forced option from '${CMAKE_PROJECT_NAME}'." OFF)
if (NOT TARGET ffmpeg-io-slib)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/ffmpeg-io/)
endif()

# Add definitions -------------------------------------------------------------
# -----------------------------------------------------------------------------
# empty...

# Set include directory -------------------------------------------------------
# -----------------------------------------------------------------------------
target_include_directories(th-common-obj PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${inc_dir}/)
if(TH_BALLON_EXE)
	target_include_directories(th-ballon-exe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${inc_dir}/)
endif()
if(TH_TRACKING_EXE)
	target_include_directories(th-tracking-exe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${inc_dir}/)
endif()

# Link with dependencies ------------------------------------------------------
# -----------------------------------------------------------------------------
target_link_libraries(th-common-obj PRIVATE ffmpeg-io-slib)
if(TH_BALLON_EXE)
	target_link_libraries(th-ballon-exe PRIVATE ffmpeg-io-slib)
endif()
if(TH_TRACKING_EXE)
	target_link_libraries(th-tracking-exe PRIVATE ffmpeg-io-slib)
endif()

