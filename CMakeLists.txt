cmake_minimum_required(VERSION 2.7)
project(bmp_func)

set(CMAKE_CXX_STANDARD 14)

add_executable(bmp_func main.cpp pic_file.cc pic_file.h bmp_pic.cc bmp_pic.h)
