add_library(toy2d)

target_sources(toy2d
    PUBLIC
        toy2d.h
        context.h
        utility.h
        swapchain.h
    PRIVATE
        toy2d.cpp
        context.cpp
        utility.cpp
        swapchain.cpp
)
