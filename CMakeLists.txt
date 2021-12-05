cmake_minimum_required(VERSION 3.0.2)
project(us_node)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES us_node
  CATKIN_DEPENDS roscpp std_msgs
  DEPENDS system_lib
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

add_executable(us_node src/us_node.cpp)

add_dependencies(us_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

target_link_libraries(us_node
  ${catkin_LIBRARIES}
  pigpiod_if2
)
