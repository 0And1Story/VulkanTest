add_library(toy2d)

file(GLOB TOY2D_HEADERS "*.hpp")
file(GLOB TOY2D_SOURCES "*.cpp")

target_sources(toy2d
    PUBLIC
        ${TOY2D_HEADERS}
    PRIVATE
        ${TOY2D_SOURCES}
)
