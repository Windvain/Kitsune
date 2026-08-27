add_library(KitsuneMain STATIC
    "Launch/EngineLoop.cpp"
    "Launch/EngineLoop.h"
    "UniversalMain.cpp"
)

add_library(Kitsune::Main ALIAS KitsuneMain)
kitsune_set_base_settings(KitsuneMain)

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    target_sources(KitsuneMain PRIVATE
        "Windows/WindowsEngineLoop.cpp"
        "Windows/WindowsEntry.cpp"
    )
else()
    message(
        FATAL_ERROR
        "Could not find an entry point implementation for ${CMAKE_SYSTEM_NAME}."
    )
endif()

set_target_properties(KitsuneMain PROPERTIES FOLDER "Engine")

target_link_libraries(KitsuneMain PUBLIC Kitsune::Runtime)
target_include_directories(KitsuneMain PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
