
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(somanetwork)

SET(LIBRARY_VERSION "1.0")
SET(LIBRARY_SOVERSION "0")


# boost requirements
find_package(Boost 1.37.0 COMPONENTS thread unit_test_framework program_options filesystem system REQUIRED)
find_package(PkgConfig)

pkg_check_modules(SIGC++ REQUIRED sigc++-2.0>=2.0)
pkg_check_modules(BENCHMARKS REQUIRED 
  sigc++-2.0>=2.0 
  glibmm-2.4>=2.13.0
  gtkmm-2.4>=2.10)

ENABLE_TESTING()
INCLUDE(CTest)

include_directories(.)

add_subdirectory(somanetwork)
add_subdirectory(tests)
add_subdirectory(benchmarks)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/somanetwork-1.0.pc.in 
  ${CMAKE_CURRENT_BINARY_DIR}/somanetwork-1.0.pc)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/somanetwork-1.0.pc 
  DESTINATION lib/pkgconfig)
