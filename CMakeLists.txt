# NAME/CMakeLists.txt
# Original file by Robin Rowe 2020-05-01
# Extended by Phil Burk 2021-10-31
# License: BSD Zero

# To build pforth:
#
#    cmake .
#    make
#
# That will create the following files:
#    fth/pforth   # executable that loads pforth.dic (pforth.exe on Windows)
#    fth/pforth.dic
#    fth/pforth_standalone # executable that does not need a .dic file
#
# The build has several steps
# 1. Build pforth executable
# 2. Build pforth.dic by compiling system.fth
# 3. Create a pfdicdat.h header containing a precompiled dictionary
#    as C source code.
# 4.  Build pforth_standalone using the precompiled dictionary.

cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Put pforth in the fth folder so we can load the Forth code more easily.
set(PFORTH_FTH_DIR ${CMAKE_SOURCE_DIR}/pforth/fth)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PFORTH_FTH_DIR})

project(PForth)
message("Configuring ${PROJECT_NAME}...")
enable_testing()

if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    message("Warning: _CRT_SECURE_NO_WARNINGS")
    set(PFORTH_EXE Debug/pforth.exe)
	set(PFORTH_EXTRA_LIBS )
endif(WIN32)

if(UNIX OR APPLE)
    set(PFORTH_EXE pforth)
	set(PFORTH_EXTRA_LIBS m)
endif(UNIX OR APPLE)


################################
#
# csrc
#
################################

file(STRINGS sources.cmake SOURCES)
set(CSRC_DIRECTORY ${CMAKE_SOURCE_DIR}/pforth/csrc)

if(WIN32)
set(PLATFORM ${CSRC_DIRECTORY}/stdio/pf_fileio_stdio.c ${CSRC_DIRECTORY}/win32_console/pf_io_win32_console.c )
endif(WIN32)

if(UNIX OR APPLE)
set(PLATFORM ${CSRC_DIRECTORY}/posix/pf_io_posix.c ${CSRC_DIRECTORY}/stdio/pf_fileio_stdio.c)
endif(UNIX OR APPLE)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(
#        --std=c89
        -fsigned-char
        -fno-builtin
        -fno-unroll-loops
        -pedantic
        -Wcast-qual
        -Wall
        -Werror
        -Wwrite-strings
        -Winline
        -Wmissing-prototypes
        -Wmissing-declarations
        )
endif()

add_library(${PROJECT_NAME}_lib ${SOURCES} ${PLATFORM})
target_compile_definitions(${PROJECT_NAME}_lib PRIVATE PF_SUPPORT_FP)

# Compile the same library but with an option for the static dictionary.
add_library(${PROJECT_NAME}_lib_sd STATIC ${SOURCES} ${PLATFORM})
target_compile_definitions(${PROJECT_NAME}_lib_sd PRIVATE PF_STATIC_DIC)
target_compile_definitions(${PROJECT_NAME}_lib_sd PRIVATE PF_SUPPORT_FP)


################################
#
# current
#
################################

if(NOT WIN32 AND NOT APPLE)
	link_libraries(rt pthread)
endif(NOT WIN32 AND NOT APPLE)

option(UNISTD "Enable libunistd" false)
if(UNISTD)
	set(LIBUNISTD_PATH /code/github/libunistd)
	if(WIN32)
		include_directories(${LIBUNISTD_PATH}/unistd)
		link_directories(${LIBUNISTD_PATH}/build/unistd/Release)
		link_libraries(libunistd)
	endif(WIN32)
endif(UNISTD)

# 1. Build pforth executable
add_executable(pforth ${CSRC_DIRECTORY}/pf_main.c)
target_link_libraries(pforth ${PROJECT_NAME}_lib ${PFORTH_EXTRA_LIBS})

# 2. Build pforth.dic by compiling system.fth
set(PFORTH_DIC "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/pforth.dic")
add_custom_command(OUTPUT ${PFORTH_DIC}
  COMMAND ./${PFORTH_EXE} -i ${PFORTH_FTH_DIR}/system.fth
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  DEPENDS pforth
  COMMENT Building pforth.dic
  VERBATIM
  )
add_custom_target(pforth_dic DEPENDS ${PFORTH_DIC})

# 3. Create a pfdicdat.h header containing a precompiled dictionary
#    as C source code.
set(PFORTH_DIC_HEADER "csrc/pfdicdat.h")
add_custom_command(OUTPUT ${PFORTH_DIC_HEADER}
  COMMAND ./${PFORTH_EXE} ${PFORTH_FTH_DIR}/mkdicdat.fth
  COMMAND ${CMAKE_COMMAND} -E rename pfdicdat.h ../csrc/pfdicdat.h
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  DEPENDS pforth_dic
  COMMENT Building pfdicdat.h
  VERBATIM
  )
add_custom_target(pforth_dic_header DEPENDS ${PFORTH_DIC_HEADER})
add_dependencies(${PROJECT_NAME}_lib_sd pforth_dic_header)

# 4. Build pforth_standalone using the precompiled dictionary.
add_executable(pforth_standalone ${CSRC_DIRECTORY}/pf_main.c)
target_link_libraries(pforth_standalone ${PROJECT_NAME}_lib_sd ${PFORTH_EXTRA_LIBS})
target_compile_definitions(pforth_standalone PRIVATE PF_STATIC_DIC)
add_dependencies(pforth_standalone pforth_dic_header)
