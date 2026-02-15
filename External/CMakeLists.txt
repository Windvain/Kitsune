if (KITSUNE_BUILD_TEST_EXECUTABLES)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    add_subdirectory("googletest")

    set_target_properties(gtest PROPERTIES FOLDER "External/googletest")
    set_target_properties(gmock PROPERTIES FOLDER "External/googletest")
    set_target_properties(gtest_main PROPERTIES FOLDER "External/googletest")
    set_target_properties(gmock_main PROPERTIES FOLDER "External/googletest")
endif()
