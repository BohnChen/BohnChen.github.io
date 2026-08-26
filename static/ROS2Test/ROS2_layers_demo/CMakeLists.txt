cmake_minimum_required(VERSION 3.8)
project(layers_demo)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rcl REQUIRED)
find_package(rmw REQUIRED)
find_package(std_msgs REQUIRED)
find_package(rosidl_typesupport_c REQUIRED)

# 01: rclcpp 层
add_executable(rclcpp_talker src/01_rclcpp_talker.cpp)
ament_target_dependencies(rclcpp_talker rclcpp std_msgs)

# 02: rcl 层（C API）
add_executable(rcl_talker src/02_rcl_talker.c)
ament_target_dependencies(rcl_talker rcl std_msgs rosidl_typesupport_c)

# 03: rmw 层探针
add_executable(rmw_probe src/03_rmw_probe.cpp)
ament_target_dependencies(rmw_probe rclcpp rmw)

install(TARGETS
  rclcpp_talker
  rcl_talker
  rmw_probe
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
