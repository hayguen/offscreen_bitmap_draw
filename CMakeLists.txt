cmake_minimum_required(VERSION 3.0)
project(OFFSCREEN_BITMAP_DRAW)

option(DISABLE_LINK_WITH_M "Disables linking with m library to build with clangCL from MSVC" OFF)

######################################################

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
include(cmake/compiler_warnings.cmake)


if (NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
   message(STATUS "Build type not specified: defaulting to release.")
endif(NOT CMAKE_BUILD_TYPE)

if ( CMAKE_C_COMPILER_ID MATCHES "MSVC" )
  # using Visual Studio C++
  message(STATUS "INFO: detected MSVC: will not link math lib m")
  set(MATHLIB "")
  # add_definitions("/D_CRT_SECURE_NO_WARNINGS")
  # set(MSVC_DISABLED_WARNINGS_LIST "C4996")
else()
  if(DISABLE_LINK_WITH_M)
  else()
    message(STATUS "INFO: detected NO MSVC: ${CMAKE_C_COMPILER_ID}: will link math lib m")
    set(MATHLIB "m")
  endif()
endif()

set(STDCXXLIB "")
if (MINGW)
  set(STDCXXLIB "stdc++")
endif()

################################################

set( OFFSCR_BMP_DRW_HEADERS
  include/offscr_bmp_drw/bitmap_image.hpp
  include/offscr_bmp_drw/cartesian_canvas.hpp
  include/offscr_bmp_drw/checkered_pattern.hpp
  include/offscr_bmp_drw/colormaps.hpp
  include/offscr_bmp_drw/colors.hpp
  include/offscr_bmp_drw/convert.hpp
  include/offscr_bmp_drw/image_drawer.hpp
  include/offscr_bmp_drw/misc.hpp
  include/offscr_bmp_drw/plasma.hpp
  include/offscr_bmp_drw/response_image.hpp
  include/offscr_bmp_drw/sobel.hpp
)

######################################################

install( FILES ${OFFSCR_BMP_DRW_HEADERS}  DESTINATION include/offscr_bmp_drw )

add_custom_target(uninstall
    "${CMAKE_COMMAND}" -P "${CMAKE_SOURCE_DIR}/cmake/uninstall.cmake"
)

######################################################

foreach (tt "all")
  add_executable(bitmap_test_${tt} EXCLUDE_FROM_ALL test/bitmap_test_${tt}.cpp )
  target_activate_cxx_compiler_warnings( bitmap_test_${tt} )
  target_include_directories( bitmap_test_${tt} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/" )
  target_link_libraries( bitmap_test_${tt} ${STDCXXLIB} ${MATHLIB} )
endforeach()

