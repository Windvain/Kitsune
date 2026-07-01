add_library(KitsuneLauncher STATIC "Game.cpp")
set_target_properties(KitsuneLauncher PROPERTIES FOLDER "Engine")

target_include_directories(KitsuneLauncher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(KitsuneLauncher PUBLIC KitsuneRuntime)
