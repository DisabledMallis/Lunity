if(MSVC)
add_compile_options(
		$<$<CONFIG:>:/MT> #---------|
		$<$<CONFIG:Debug>:/MTd> #---|-- Statically link the runtime libraries
		$<$<CONFIG:Release>:/MT> #--|
)
endif()

link_directories("${CMAKE_CURRENT_LIST_DIR}/Lib/PolyHook_2_0/_install/lib/")
link_directories("${CMAKE_CURRENT_LIST_DIR}/Lib/minhook/Release/")

add_library(Lunity SHARED)

target_sources(Lunity PRIVATE 
	"main.cpp"
)

add_subdirectory(Client)
add_subdirectory(Utils)
add_subdirectory(Mem)
add_subdirectory(Lib)

target_include_directories(Lunity PRIVATE "${CMAKE_CURRENT_LIST_DIR}/Lib/PolyHook_2_0/_install/include/")
target_include_directories(Lunity PRIVATE "${CMAKE_CURRENT_LIST_DIR}/Lib/cpp-httplib/")
target_include_directories(Lunity PRIVATE "${CMAKE_CURRENT_LIST_DIR}/Lib/HTTPRequest/include/")
target_include_directories(Lunity PRIVATE "${CMAKE_CURRENT_LIST_DIR}/Lib/kiero/")
target_include_directories(Lunity PRIVATE "${CMAKE_CURRENT_LIST_DIR}/Lib/minhook/include/")
target_include_directories(Lunity PRIVATE "${CMAKE_CURRENT_LIST_DIR}/Lib/imgui/")

message("${CMAKE_CURRENT_LIST_DIR}")

target_link_libraries(Lunity PRIVATE PolyHook_2 capstone version minhook.x64 d3d11 d3dcompiler)