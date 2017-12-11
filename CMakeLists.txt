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
	src/MusicLibrary.h
	src/WarehouseApi.h
	src/JsonConverter.h
	src/JsonWarehouseApi.h
	src/CartItem.h
	src/Message.h)


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

# server
add_executable(webserver src/webserver.cpp ${AMAZOOM_HEADERS})
target_link_libraries(webserver Threads::Threads)

# client
add_executable(client src/client.cpp ${AMAZOOM_HEADERS})
target_link_libraries(client Threads::Threads)

# client
add_executable(central_computer src/central_computer.cpp ${AMAZOOM_HEADERS})
target_link_libraries(central_computer Threads::Threads)

# link with unix realtime library
if(UNIX AND NOT APPLE)
    target_link_libraries(warehouse_main rt)
    target_link_libraries(warehouse_ui rt)
    target_link_libraries(robot rt)
    target_link_libraries(shippingtruck rt)
    target_link_libraries(receivingtruck rt)
    target_link_libraries(webserver rt)
    target_link_libraries(client rt)
    target_link_libraries(central_computer rt)
endif()

#