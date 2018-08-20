cmake_minimum_required(VERSION 3.5)
project(CSCIFinal)

set(CMAKE_CXX_STANDARD 11)

find_package(Threads)

add_definitions(-DDEBUG=true)

add_executable(CSCIFinal main.cpp tcplib/tcplib.h tcplib/Socket.cpp tcplib/Socket.h tcplib/Conn.cpp tcplib/Conn.h httplib/Http.cpp httplib/Http.h httplib/Request.cpp httplib/Request.h httplib/Url.cpp httplib/Url.h httplib/httplib.h httplib/Handler.cpp httplib/Handler.h)
target_link_libraries(CSCIFinal ${CMAKE_THREAD_LIBS_INIT})