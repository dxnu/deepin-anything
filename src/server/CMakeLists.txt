cmake_minimum_required(VERSION 3.1)

# ---------------------------------------------------------------------------------------
# Start deepin-anything-server project
# ---------------------------------------------------------------------------------------
project(
    deepin-anything-server
    VERSION 1.0.0
    DESCRIPTION "A file search engine"
    LANGUAGES CXX
)

# Find the dependencies
find_package(QT NAMES Qt6 Qt5 CONFIG REQUIRED COMPONENTS Core DBus)
find_package(Qt${QT_VERSION_MAJOR} CONFIG REQUIRED Core DBus)
find_package(PkgConfig REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(spdlog REQUIRED)
pkg_check_modules(GNL REQUIRED libnl-3.0 libnl-genl-3.0)
pkg_check_modules(MOUNT REQUIRED mount IMPORTED_TARGET)
pkg_check_modules(LUCENE REQUIRED liblucene++)
pkg_check_modules(LUCENE_CONTRIB REQUIRED liblucene++-contrib)

# Generate QDbus file
# message(STATUS "CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
# message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
set(DBUS_ADAPTOR_SOURCES com.deepin.Anything.xml)
set(ANYTHING_DBUS_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/core/base_event_handler.h")
set(QT_GENERATE_DBUS_INTERFACE "qt${QT_VERSION_MAJOR}_generate_dbus_interface")
set(QT_ADD_DBUS_ADAPTOR "qt${QT_VERSION_MAJOR}_add_dbus_adaptor")
set(QT_WRAP_CPP "qt${QT_VERSION_MAJOR}_wrap_cpp")

cmake_language(CALL ${QT_GENERATE_DBUS_INTERFACE} ${ANYTHING_DBUS_HEADER} com.deepin.Anything.xml OPTIONS -A)
cmake_language(CALL ${QT_ADD_DBUS_ADAPTOR}
    DBUS_ADAPTOR_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/com.deepin.Anything.xml
    ${ANYTHING_DBUS_HEADER}
    base_event_handler
)
cmake_language(CALL ${QT_WRAP_CPP} anything_dbus_server_moc ${ANYTHING_DBUS_HEADER})

# Found all source files
file(GLOB_RECURSE SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")
list(LENGTH SOURCE_FILES SRC_FILES_SIZE)
message(STATUS "Found ${SRC_FILES_SIZE} source files of deepin-server")
foreach(source_file ${SOURCE_FILES})
    message(STATUS "  ${source_file}")
endforeach()

# Define a shared library
add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES} ${anything_dbus_server_moc} ${DBUS_ADAPTOR_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../kernelmod>
    # $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    ${LUCENE_INCLUDE_DIRS}
    ${GNL_INCLUDE_DIRS}
    ${MOUNT_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    ${LUCENE_LIBRARIES}
    ${LUCENE_CONTRIB_LIBRARIES}
    ${GNL_LIBRARIES}
    ${MOUNT_LIBRARIES}
    ${Boost_LIBRARIES}
    Qt${QT_VERSION_MAJOR}::DBus
    pthread
    stdc++fs
    spdlog::spdlog
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror -Wextra)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

configure_file ("${PROJECT_NAME}.pc.in" "${PROJECT_NAME}.pc" @ONLY)
install(TARGETS ${PROJECT_NAME}
    COMPONENT linapp
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    DESTINATION lib
)