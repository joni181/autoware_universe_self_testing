cmake_minimum_required(VERSION 3.8)
project(autoware_self_test_types)

# Optional but common: warnings for GCC/Clang (matches ros2 pkg create template)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)

# ---- header-only package: install + export headers ----
install(
  DIRECTORY include/
  DESTINATION include
)

ament_export_include_directories(include)

# ---- tests ----
if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(test_autoware_self_test_types test/test_types.cpp)
  target_include_directories(test_autoware_self_test_types PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  )

  # Optional: linters (from ros2 template). Enable if you want lint checks.
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
