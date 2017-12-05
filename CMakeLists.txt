cmake_minimum_required(VERSION 3.2)
project(amazoom)

set(CMAKE_CXX_STANDARD 11)
find_package( Threads REQUIRED )  # Multithreading package required

# add library directory
include_directories(library/include)

# copy data files
FILE(COPY data DESTINATION "${CMAKE_BINARY_DIR}")
FILE(COPY data DESTINATION "${CMAKE_BINARY_DIR}/Debug")

#===================================================
#  Amazoom Warehouse
#===================================================

set(AMAZOOM_HEADERS
	src/warehouse_common.h
	src/truck.h
	src/JsonConverter.h
	src/JsonWarehouseApi.h
	src/Message.h
	src/MusicLibrary.h
	src/WarehouseApi.h)

# child
add_executable(warehouse_ui src/warehouse_ui.cpp ${AMAZOOM_HEADERS})
target_link_libraries(warehouse_ui Threads::Threads)

# parent
add_executable(warehouse_main src/warehouse_main.cpp ${AMAZOOM_HEADERS})
target_link_libraries(warehouse_main Threads::Threads)

# parent
add_executable(robot src/robot.cpp ${AMAZOOM_HEADERS})
target_link_libraries(robot Threads::Threads)

# parent
add_executable(receivingtruck src/receivingtruck.cpp ${AMAZOOM_HEADERS})
target_link_libraries(receivingtruck Threads::Threads)

# parent
add_executable(shippingtruck src/shippingtruck.cpp ${AMAZOOM_HEADERS})
target_link_libraries(shippingtruck Threads::Threads)

# parent
add_executable(webserver src/webserver.cpp ${AMAZOOM_HEADERS})
target_link_libraries(webserver Threads::Threads)

# parent
add_executable(client src/client.cpp ${AMAZOOM_HEADERS})
target_link_libraries(client Threads::Threads)

# link with unix realtime library
if(UNIX AND NOT APPLE)
    target_link_libraries(warehouse_main rt)
    target_link_libraries(warehouse_ui rt)
    target_link_libraries(robot rt)
    target_link_libraries(shippingtruck rt)
    target_link_libraries(receivingtruck rt)
endif()

# set working directories
if (${CMAKE_VERSION} VERSION_GREATER "3.8.0" OR ${CMAKE_VERSION} VERSION_EQUAL "3.8.0")
    set_target_properties(warehouse_main PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/$(CONFIGURATION)")
    set_target_properties(robot PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/$(CONFIGURATION)")
    set_target_properties(shippingtruck PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/$(CONFIGURATION)")
    set_target_properties(receivingtruck PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/$(CONFIGURATION)")
    set_target_properties(webserver PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/$(CONFIGURATION)")
    set_target_properties(client PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/$(CONFIGURATION)")
endif()

