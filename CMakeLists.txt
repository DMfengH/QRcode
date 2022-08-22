cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 14)

project(myQRcode)
#find_package(OpenCV REQUIRED)
#find_package(Sophus REQUIRED)
#find_package(Pangolin REQUIRED)
#
#
#include_directories(
#        ${OpenCV_INCLUDE_DIRS}
##        ${G2O_INCLUDE_DIRS}
##        ${Sophus_INCLUDE_DIRS}
#        "/usr/include/eigen3/"
##        ${Pangolin_INCLUDE_DIRS}
#)

# add_library()

#add_executable(Qrcode ./myQRcode.cpp)
#add_executable(AnalysisQR ./analysisQR.cpp)
add_executable(Qrcode ./test.cpp)

#target_link_libraries(AnalysisQR ${OpenCV_LIBS})