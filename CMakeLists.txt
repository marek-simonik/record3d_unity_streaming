cmake_minimum_required(VERSION 3.13)
project(record3d_unity_streaming)

set(CMAKE_CXX_STANDARD 14)

if (APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE INTERNAL "" FORCE)
endif()

if (UNIX AND NOT APPLE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

if(WIN32)
    add_compile_definitions(WIN32)
endif()

include_directories(include)

include(GNUInstallDirs)
add_library(record3d_unity_streaming SHARED Record3DUnityStreaming.cpp)

if (APPLE)
target_link_directories(record3d_unity_streaming PUBLIC /usr/local/lib)
endif()

target_link_libraries(record3d_unity_streaming record3d_cpp lzfse usbmuxd plist)
install(TARGETS record3d_unity_streaming EXPORT Record3DUnityStreamingConfig
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT Record3DUnityStreamingConfig DESTINATION share/Record3DUnityStreaming/cmake)

set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)