# cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
# cmake --build build -j"$(nproc)"

cmake_minimum_required(VERSION 3.28)


project(img2avif 
VERSION 1.0.0
DESCRIPTION "C++23 library and command-line toolset for converting images to the modern AVIF format."
HOMEPAGE_URL "https://github.com/Zheng-Bote/Img2Avif"
LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBAVIF REQUIRED IMPORTED_TARGET libavif)
pkg_check_modules(EXIV2 REQUIRED IMPORTED_TARGET exiv2)

add_library(img2avif_lib src/Img2Avif.cpp include/stb_image.h include/stb_image_resize2.h)
target_include_directories(img2avif_lib PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(img2avif_lib PUBLIC PkgConfig::LIBAVIF PkgConfig::EXIV2)
target_compile_options(img2avif_lib PUBLIC ${LIBAVIF_CFLAGS_OTHER})

add_executable(img2avif cli/img2avif.cpp)
target_link_libraries(img2avif PRIVATE img2avif_lib)

add_executable(img2avifs cli/img2avifs.cpp)
target_link_libraries(img2avifs PRIVATE img2avif_lib)

# Installation and Package Configuration
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS img2avif_lib img2avif img2avifs
    EXPORT img2avifTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES include/Img2Avif.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT img2avifTargets
    FILE img2avifTargets.cmake
    NAMESPACE img2avif::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/img2avif
)

configure_package_config_file(
    cmake/img2avifConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/img2avifConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/img2avif
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/img2avifConfigVersion.cmake
    VERSION 0.1.0
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/img2avifConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/img2avifConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/img2avif
)

# libs.txt Generate (timing-safe via GENERATE)
file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/libs.txt" 
     CONTENT "$<TARGET_PROPERTY:img2avif,LINK_LIBRARIES>")