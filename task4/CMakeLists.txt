project (task4)

find_package(OpenGL REQUIRED)

# task 4
add_executable(task4
        main.cpp
        ${CMAKE_SOURCE_DIR}/common/shader.cpp
        ${CMAKE_SOURCE_DIR}/common/shader.hpp
        )
target_link_libraries(task4 ${ALL_LIBS})
# Xcode and Visual working directories
set_target_properties(task4 PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/task4/")
create_target_launcher(task4 WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/task4/")

add_custom_command(
        TARGET task4 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/task4${CMAKE_EXECUTABLE_SUFFIX}"
        "${CMAKE_CURRENT_SOURCE_DIR}"
)
