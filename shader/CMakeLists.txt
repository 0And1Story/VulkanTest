# Compile Shaders
find_program(GLSLC_PROGRAM glslc REQUIRED)

file(GLOB SHADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.vert" "${CMAKE_CURRENT_SOURCE_DIR}/*.frag")
set(SPIRV_FILES "")
foreach(SHADER_FILE ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME)
    set(SPIRV_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_NAME}.spv")
    add_custom_command(
            OUTPUT ${SPIRV_FILE}
            COMMAND ${GLSLC_PROGRAM} ${SHADER_FILE} -o ${SPIRV_FILE}
            DEPENDS ${SHADER_FILE}
            COMMENT "Compiling shader: ${SHADER_NAME}"
    )
    list(APPEND SPIRV_FILES ${SPIRV_FILE})
endforeach()

add_custom_target(CompileShaders ALL DEPENDS ${SPIRV_FILES})