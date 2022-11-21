cmake_minimum_required(VERSION 3.21)
project(runcc)

include(../cmake/ccsys.cmake)

add_executable(runcc
	main.c
	../sys/lib/libqlibc.a
)

target_link_libraries(runcc
	libqlibc.a
	pthread
)

install(TARGETS runcc DESTINATION bin)
