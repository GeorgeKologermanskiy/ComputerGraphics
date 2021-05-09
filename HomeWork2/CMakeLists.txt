project (HomeWork2)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)

# task 2
add_executable(HomeWork2
        main.cpp
        Enemy.h
        Fireball.h
        ${CMAKE_SOURCE_DIR}/common/shader.cpp
        ${CMAKE_SOURCE_DIR}/common/shader.hpp
        ${CMAKE_SOURCE_DIR}/common/texture.cpp
        ${CMAKE_SOURCE_DIR}/common/texture.hpp
        ${CMAKE_SOURCE_DIR}/common/controls.cpp
        ${CMAKE_SOURCE_DIR}/common/controls.hpp
        )
target_link_libraries(HomeWork2 ${ALL_LIBS})
# Xcode and Visual working directories
set_target_properties(HomeWork2 PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/HomeWork2/")
create_target_launcher(HomeWork2 WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/HomeWork2/")

add_custom_command(
        TARGET HomeWork2 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/HomeWork2${CMAKE_EXECUTABLE_SUFFIX}"
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

