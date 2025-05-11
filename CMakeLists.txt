cmake_minimum_required(VERSION 3.5.0)
project(SDLCards VERSION 0.1.0 LANGUAGES C CXX)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(WIN32)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(CURL REQUIRED)

endif()

add_executable(SDLCards
                app.cpp
                card.cpp
                exception.cpp
                main.cpp
                paciencia.cpp
                render.cpp
                thread.cpp
                imgui/imgui_demo.cpp
                imgui/imgui_draw.cpp
                imgui/imgui_tables.cpp
                imgui/imgui_widgets.cpp
                imgui/imgui.cpp
                imgui/backends/imgui_impl_sdl2.cpp
                imgui/backends/imgui_impl_sdlrenderer2.cpp
               )

if(MSVC)

set_target_properties(SDLCards PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

endif()

if(WIN32)
    target_link_libraries(SDLCards
        PRIVATE
        CURL::libcurl
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
        $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
    )
elseif(LINUX)
    target_link_libraries(SDLCards SDL2 SDL2_image SDL2_ttf curl)
endif()

target_include_directories(SDLCards PRIVATE imgui imgui/backends /usr/include/SDL2)