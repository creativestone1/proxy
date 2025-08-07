cmake_minimum_required(VERSION 3.10)
project(proxy VERSION 0.1.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
#----SOURCES----
set(NET_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/net/ConnectionCreator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/net/ConnectionHandler.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/net/IOMultiplexer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/net/MessageProcessors.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/net/ProxySessionHandler.cpp	
	${CMAKE_CURRENT_SOURCE_DIR}/src/net/SocketEventProcessor.cpp
)
set(ADDITIONAL_DEPENDENCIES_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/additional_dependencies/SockUtils.cpp
)
set(SOURCES
	${NET_SOURCES}
	${ADDITIONAL_DEPENDENCIES_SOURCES}
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

#----INCLUDE_DIRS----
set(INCLUDE_DIRS
	${CMAKE_CURRENT_SOURCE_DIR}/include/net
	${CMAKE_CURRENT_SOURCE_DIR}/include/additional_dependencies
	${CMAKE_CURRENT_SOURCE_DIR}/include/config
)

#----linking----
add_executable(proxy ${SOURCES})
target_link_options(proxy PRIVATE "-static")
target_include_directories(proxy PRIVATE ${INCLUDE_DIRS})

