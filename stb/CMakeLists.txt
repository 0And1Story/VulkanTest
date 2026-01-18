add_library(stb)

file(GLOB STB_HEADERS "*.h")
file(GLOB STB_SOURCES "*.cpp")

target_sources(stb
    PUBLIC
        ${STB_HEADERS}
    PRIVATE
        ${STB_SOURCES}
)
