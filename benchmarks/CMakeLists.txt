
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${SIGC++_INCLUDE_DIRS})
include_directories(${BENCHMARKS_INCLUDE_DIRS})

add_executable(soma-netbench
  glibbench.cc
  range.cc)

add_executable(soma-netbench-tx
  synthtx.cc
  range.cc
  eventsynthtx.cc
  audiogen.cc
  digitaloutgen.cc 
  periodicpattern.cc
)

target_link_libraries(soma-netbench-tx
  somanetwork_test
   ${Boost_LIBRARIES}
   ${BENCHMARKS_LIBRARIES})

 target_link_libraries(soma-netbench
   somanetwork_test
   ${Boost_LIBRARIES} 
   ${BENCHMARKS_LIBRARIES})

