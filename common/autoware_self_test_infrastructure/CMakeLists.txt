cmake_minimum_required(VERSION 3.8)
project(autoware_self_test_infrastructure)

find_package(ament_cmake REQUIRED)
find_package(autoware_self_test_types REQUIRED)

# TODO(jlippss): add all *.cpp files
add_library(${PROJECT_NAME} SHARED
  src/self_test_registry.cpp
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
ament_export_dependencies(autoware_self_test_types)

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)

  # TODO(jlippss): add all test files
  ament_add_gtest(test_self_test_registry
    test/test_self_test_registry.cpp
  )

  target_link_libraries(test_self_test_registry
    ${PROJECT_NAME}
  )

  ament_target_dependencies(test_self_test_registry
    autoware_self_test_types
  )
endif()

ament_package()
