# Top-Level CMakeList.txt

cmake_minimum_required (VERSION 2.6)
project ( Project )

add_executable( Project skeleton.cpp)

find_package (SDL)

if ( NOT SDL_FOUND )
   message ( FATAL_ERROR "SDL not found!" )
endif ( NOT SDL_FOUND )

if(SDL_FOUND)
	include_directories(
		${SDL_INCLUDE_DIR}
		${PROJECT_SOURCE_DIR}/glm
	)
	#link_libraries(${SDL_LIBRARY})
	target_link_libraries(Project ${SDL_LIBRARY})
endif(SDL_FOUND)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)

add_definitions( -DMAGICKCORE_QUANTUM_DEPTH=16 )
add_definitions( -DMAGICKCORE_HDRI_ENABLE=0 )
find_package(ImageMagick COMPONENTS Magick++)
include_directories(${ImageMagick_INCLUDE_DIRS})
target_link_libraries(Project ${ImageMagick_LIBRARIES})
