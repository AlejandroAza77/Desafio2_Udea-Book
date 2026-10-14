cmake_minimum_required(VERSION 3.16)

project(Desafio2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Desafio2 main.cpp
    medidor.h medidor.cpp
    lista.h
    fecha.h fecha.cpp
    solicitud.h solicitud.cpp
    cadena.h cadena.cpp
    foto.h foto.cpp
    album.h album.cpp
)

include(GNUInstallDirs)
install(TARGETS Desafio2
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)