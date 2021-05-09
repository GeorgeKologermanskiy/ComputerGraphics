project (task3)

find_package(OpenGL REQUIRED)

# task 3
add_executable(task3
        main.cpp
        ${CMAKE_SOURCE_DIR}/common/shader.cpp
        ${CMAKE_SOURCE_DIR}/common/shader.hpp
        )
target_link_libraries(task3 ${ALL_LIBS})
# Xcode and Visual working directories
set_target_properties(task3 PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/task3/")
create_target_launcher(task3 WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/task3/")

add_custom_command(
        TARGET task3 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/task3${CMAKE_EXECUTABLE_SUFFIX}"
        "${CMAKE_CURRENT_SOURCE_DIR}"
)
