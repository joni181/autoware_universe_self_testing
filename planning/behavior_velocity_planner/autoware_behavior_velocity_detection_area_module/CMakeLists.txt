cmake_minimum_required(VERSION 3.14)
project(autoware_behavior_velocity_detection_area_module)

find_package(autoware_cmake REQUIRED)
autoware_package()
pluginlib_export_plugin_description_file(autoware_behavior_velocity_planner plugins.xml)

find_package(autoware_self_test_types REQUIRED)
find_package(autoware_self_test_infrastructure REQUIRED)

ament_auto_add_library(${PROJECT_NAME} SHARED
  DIRECTORY src
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  ament_add_ros_isolated_gtest(test_${PROJECT_NAME}
    test/test_utils.cpp
    test/test_node_interface.cpp
    test/test_detection_area_testable_smoke.cpp
    test/test_tester_component.cpp
  )

  target_include_directories(test_${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
  )

  ament_target_dependencies(test_${PROJECT_NAME}
    autoware_self_test_types
    autoware_self_test_infrastructure
  )

  target_link_libraries(test_${PROJECT_NAME} ${PROJECT_NAME})
endif()

ament_auto_package(INSTALL_TO_SHARE config)
