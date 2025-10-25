if (WIN32)
    add_executable(Sandbox WIN32)
else()
    add_executable(Sandbox)
endif()

target_sources(Sandbox PRIVATE "Sandbox.cpp")
set_target_properties(Sandbox PROPERTIES FOLDER "Examples")

target_compile_options(Sandbox PRIVATE "${KITSUNE_GLOBAL_COMPILER_FLAGS}")
target_link_options(Sandbox PRIVATE "${KITSUNE_GLOBAL_LINKER_FLAGS}")

target_link_libraries(
    Sandbox
    PRIVATE

    KitsuneEngine
    "comctl32.lib"
)
