cmake_minimum_required(VERSION 3.22)
project(dn)

set(CMAKE_CXX_STANDARD 17)

set(ENET_DIR thirdparty/enet/cmake)
set(ENET_LIBRARY enet)
set(ENET_LIBRARIES ${ENET_LIBRARY})

find_package(ENET REQUIRED)

include_directories(include)
include_directories(${ENET_DIR})
include_directories(thirdparty/enet/include)

link_directories(thirdparty/enet/lib)

add_executable(${PROJECT_NAME}
	include/dn.h
	include/config.h
	include/olcPixelGameEngine.h
	include/core/client/client.h
	include/core/context/context.h
	include/core/detail/packet_parser.h
	include/core/packet/packet.h
	include/core/server/server.h

	src/client.cpp
	src/context.cpp
	src/packet.cpp
	src/packet_parser.cpp
	src/server.cpp
	src/test.cpp

        )

target_link_libraries(${PROJECT_NAME} PRIVATE ${ENET_LIBRARIES} winmm ws2_32)