cmake_minimum_required(VERSION 2.8)

project(PROOF)

include_directories(${PROOF_SOURCE_DIR} headers axioms)

set(SOURCE_EXE main.cpp
               parser.cpp
               deduction.cpp
               checkOnFalse.cpp)


add_executable(main ${SOURCE_EXE})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -pedantic -Wextra -Wno-unused-result -D_GLIBCXX_DEBUG -g -fsanitize=address")
  
endif()

target_link_libraries(main)

