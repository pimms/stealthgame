cmake_minimum_required(VERSION 2.8)

LIST( APPEND CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

FILE(GLOB GAME_SRC "src/*.h" "src/*.cpp")
FILE(GLOB SHARE_SRC "src/net/*.h" "src/net/*.cpp")
FILE(GLOB SERV_SRC "src/serv/*.h" "src/serv/*.cpp")

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

ADD_EXECUTABLE(game ${GAME_SRC} ${SHARE_SRC})
TARGET_LINK_LIBRARIES(game "trutle")
TARGET_LINK_LIBRARIES(game "SDL2_image")
TARGET_LINK_LIBRARIES(game "SDL2_net")
TARGET_LINK_LIBRARIES(game "GLEW")
TARGET_LINK_LIBRARIES(game "GL")
TARGET_LINK_LIBRARIES(game "GLU")
TARGET_LINK_LIBRARIES(game "SDL2")
TARGET_LINK_LIBRARIES(game "png")


ADD_EXECUTABLE(serv ${SERV_SRC} ${SHARE_SRC})
TARGET_LINK_LIBRARIES(serv "trutle")
TARGET_LINK_LIBRARIES(serv "SDL2_image")
TARGET_LINK_LIBRARIES(serv "SDL2_net")
TARGET_LINK_LIBRARIES(serv "GLEW")
TARGET_LINK_LIBRARIES(serv "GL")
TARGET_LINK_LIBRARIES(serv "GLU")
TARGET_LINK_LIBRARIES(serv "SDL2")
TARGET_LINK_LIBRARIES(serv "png")