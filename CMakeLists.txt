cmake_minimum_required(VERSION 3.5)

project(loconvertor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Abs path should probably be fixed
set(LOROOT /home/ubuntu/Desktop/libreoffice)

add_executable(loconvertor main.cpp)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/com/sun/star
    ${LOROOT}/instdir/sdk/include
    ${LOROOT}/include
    ${LOROOT}/config_host
)
target_link_directories(loconvertor PRIVATE
    ${LOROOT}/instdir/program
    ${LOROOT}/instdir/sdk/lib
    )
target_link_libraries(loconvertor
    -luno_sal
    -luno_cppu
    -luno_cppuhelpergcc3
    -luno_salhelpergcc3
    -lunoidllo
    -lxmlreaderlo
    -lreglo
    )

add_definitions(-DLINUX)
execute_process(COMMAND ${LOROOT}/instdir/sdk/bin/cppumaker -Gc -O. ${LOROOT}/instdir/program/types.rdb ${LOROOT}/instdir/program/types/offapi.rdb)

