cmake_minimum_required(VERSION 3.10)
project(RingBufferCommunication)

set(CMAKE_C_STANDARD 99)

add_executable(sender sender.c common.c)
add_executable(receiver receiver.c common.c)
