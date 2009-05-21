
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(somanetwork)

# boost requirements
find_package(Boost 1.37.0 COMPONENTS thread unit_test_framework program_options system REQUIRED)
find_package(PkgConfig)

pkg_check_modules(SIGC++ REQUIRED sigc++-2.0>=2.0)
pkg_check_modules(BENCHMARKS REQUIRED 
  sigc++-2.0>=2.0 
  glibmm-2.4>=2.13.0
  gtkmm-2.4>=2.10)

ENABLE_TESTING()


include_directories(.)

add_subdirectory(somanetwork)
add_subdirectory(tests)
add_subdirectory(benchmarks)

