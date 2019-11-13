cmake_minimum_required(VERSION 3.9)
project("linkino")
execute_process(COMMAND sdl2-config --libs 2>/dev/null OUTPUT_VARIABLE STATUS_SDL_LIBRARY)
execute_process(COMMAND sdl2-config --cflags 2>/dev/null OUTPUT_VARIABLE STATUS_SDL_INCLUDE)
string(REGEX REPLACE "\n$" "" STATUS_SDL_LIBRARY "${STATUS_SDL_LIBRARY}")
string(REGEX REPLACE "\n$" "" STATUS_SDL_INCLUDE "${STATUS_SDL_INCLUDE}")

# Required for YCM
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_executable(linkino
        linkino.c
        loader.obj.c
        loader.obj.h
        miranda.c
        miranda.h
        connector.obj.h
        connector.obj.c
        connector_factory.obj.h
        connector_factory.obj.c 
        connectable.obj.h 
        connectable.obj.c 
        connectable_factory.obj.h 
        connectable_factory.obj.c 
        director.obj.c 
        director.obj.h 
        packet.obj.c 
        packet.obj.h
        packet_factory.obj.h
        packet_factory.obj.c)

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image)
PKG_SEARCH_MODULE(SDL2MIXER REQUIRED SDL2_mixer)

INCLUDE_DIRECTORIES(${STATUS_SDL_INCLUDE} ${SDL2_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDL2MIXER_INCLUDE_DIRS} /usr/local/include)

target_link_libraries(linkino ${STATUS_SDL_LIBRARY} ${SDL2_LIBRARIES} ${SDL2TTF_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2MIXER_LIBRARIES}
        pthread
        miranda_ground miranda_objects
        miranda_geometry miranda_io
        miranda_network miranda_media
        miranda_cameras miranda_ui
        m)

# We need to transfer the 'data' directory into the executable file (symbolic link)
set (source "${CMAKE_SOURCE_DIR}/data")
set (destination "${CMAKE_CURRENT_BINARY_DIR}/data")
add_custom_command(
        TARGET linkino POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination}
        DEPENDS ${destination}
        COMMENT "symbolic link resources folder from ${source} to ${destination}"
)
