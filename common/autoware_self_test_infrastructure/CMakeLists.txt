cmake_minimum_required(VERSION 3.8)
project(autoware_self_test_infrastructure)

find_package(ament_cmake REQUIRED)
find_package(autoware_self_test_types REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_srvs REQUIRED)

add_library(${PROJECT_NAME} SHARED
  src/self_test_registry.cpp
  src/test_run_pipeline/test_executor.cpp
  src/test_run_pipeline/evaluator.cpp
  src/test_run_pipeline/reporter.cpp
  src/self_test_controller.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

ament_target_dependencies(${PROJECT_NAME}
  autoware_self_test_types
)

# Install headers
install(
  DIRECTORY include/
  DESTINATION include
)

# Install library
install(
  TARGETS ${PROJECT_NAME}
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

# Export for downstream packages
ament_export_include_directories(include)
ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_export_dependencies(autoware_self_test_types rclcpp std_srvs)

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(test_${PROJECT_NAME}
    test/test_self_test_registry.cpp
    test/test_test_executor.cpp
    test/test_evaluator.cpp
    test/test_reporter.cpp
    test/test_self_test_controller.cpp
  )

  target_link_libraries(test_${PROJECT_NAME} ${PROJECT_NAME})

  ament_target_dependencies(test_${PROJECT_NAME} autoware_self_test_types)
endif()

ament_package()
