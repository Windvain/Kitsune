if (WIN32)
    add_executable(Sandbox WIN32)
else()
    add_executable(Sandbox)
endif()

target_sources(Sandbox PRIVATE "Sandbox/Sandbox.cpp")
target_link_libraries(Sandbox PRIVATE KitsuneLaunch)
