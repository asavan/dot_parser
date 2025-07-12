cmake_minimum_required(VERSION 3.16)
get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId})
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_DISABLE_PRECOMPILE_HEADERS ON)

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_library(${ProjectId} Tokens.hpp)
target_sources(${ProjectId}
    PRIVATE
        Graph.cpp
		GraphFactory.cpp
		Parser.cpp
		Scanner.cpp
        Tree.cpp		
    PUBLIC
		Graph.hpp
		GraphFactory.hpp
		Parser.hpp
		Scanner.hpp
        Tree.hpp		
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${ProjectId})
