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
    mensaje.h mensaje.cpp
    usuario.h usuario.cpp
    matrizamistad.h matrizamistad.cpp
    almacenamiento.h almacenamiento.cpp
    udeabook.h udeabook.cpp


)

include(GNUInstallDirs)
install(TARGETS Desafio2
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
# Crear la carpeta del ejecutable generador
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/generador")

# Crear la carpeta datos dentro de la salida del generador (donde el ejecutable busca archivos)
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/generador/datos")

# Crear también la carpeta datos en el ejecutable principal Desafio2
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/datos")

# Definir el ejecutable independiente del generador
add_executable(generador
    generador/generador.cpp
)

# Indicar la carpeta de salida específica para el generador
set_target_properties(generador PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/generador"
)