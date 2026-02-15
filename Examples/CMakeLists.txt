if (WIN32)
    set(EXAMPLE_EXE_TYPE WIN32)
endif()

link_libraries(KitsuneEngine)

# ========== Sandbox ========================================]
add_executable(Sandbox ${EXAMPLE_EXE_TYPE}
    "Sandbox/Sandbox.cpp"
)

set_target_properties(Sandbox PROPERTIES FOLDER "Examples")
