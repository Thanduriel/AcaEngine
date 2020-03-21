add_compile_definitions(RESOURCE_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/resources")

add_executable(test_meshdata_load test_meshdata_load.cpp)
set_target_properties(test_meshdata_load PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED YES
)
set(TEST_INCLUDE_DIR ${INCLUDE_DIR})
list(TRANSFORM INCLUDE_DIR PREPEND ../)
target_link_libraries(test_meshdata_load PRIVATE AcaEngine)
target_include_directories(test_meshdata_load PRIVATE  ${INCLUDE_DIR})

add_test(meshdata_load test_meshdata_load)












