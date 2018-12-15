cmake_minimum_required(VERSION 2.6)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}				src_server)
aux_source_directory(${PROJECT_SOURCE_DIR}/Source/Room   	src_room)
aux_source_directory(${PROJECT_SOURCE_DIR}/../Common/Network	src_network)


add_executable(RCServer
    ${src_server}
    ${src_room}
    ${src_network}
)

target_link_libraries(RCServer logger enet pthread rt)
