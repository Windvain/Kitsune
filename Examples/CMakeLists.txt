if (WIN32)
    add_executable(Sandbox WIN32)
else()
    add_executable(Sandbox)
endif()

target_sources(Sandbox PRIVATE "Sandbox/Sandbox.cpp")

target_include_directories(Sandbox PRIVATE "${KITSUNE_ROOT_DIR}/Source/Runtime")
target_link_libraries(Sandbox PRIVATE KitsuneLaunch KitsuneApplicationCore)
