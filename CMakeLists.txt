cmake_minimum_required(VERSION 3.20)

project(chatroom)

find_package(SDL2_net CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

file(GLOB_RECURSE HEADERS ./*.hpp)
aux_source_directory(. SRC)
add_executable(server)
target_sources(server PRIVATE ${SRC} ${HEADERS})
target_compile_features(server PRIVATE cxx_std_17)
target_link_libraries(server PRIVATE $<IF:$<TARGET_EXISTS:SDL2_net::SDL2_net>,SDL2_net::SDL2_net,SDL2_net::SDL2_net-static>)
target_link_libraries(server
    PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)
