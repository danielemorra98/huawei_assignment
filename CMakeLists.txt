cmake_minimum_required(VERSION 3.0.2)
project(huawei_assignment)


find_package(catkin_simple REQUIRED)

catkin_simple(ALL_DEPS_REQUIRED)

cs_add_executable(open_cv_crop_node src/open_cv_crop_node.cpp)

cs_install()
cs_export()