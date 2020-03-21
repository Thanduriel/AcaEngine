cmake_minimum_required (VERSION 3.12)
project (AcaEngine)

#---------------------------------------------------------------------------------------
# main project
#---------------------------------------------------------------------------------------

# find all source and header files
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.hpp" "src/*.inl")

# organize the files according to the organisation in the file system
# from: http:#cmake.3232098.n2.nabble.com/Keep-folder-structure-in-Visual-Studio-Project-generated-with-CMake-td7586044.html
macro(GroupSources curdir)
   file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)
   foreach(child ${children})
          if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
                  GroupSources(${curdir}/${child})
          else()
                  string(REPLACE "/" "\\" groupname ${curdir})
            source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
          endif()
   endforeach()
endmacro()

# execute tha macro
GroupSources(src)

add_executable(Game src/main.cpp)
set_target_properties(Game PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
)
list(APPEND INCLUDE_DIR Game PRIVATE "src")
if (MSVC)
	# disable type conversion warnings globaly
	add_compile_options(/wd4267)
	add_compile_options(/wd4244)
	add_compile_options(/wd4305)
	add_compile_options(/wd4996)
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT AcaEngine)
endif()

#---------------------------------------------------------------------------------------
# libraries
#---------------------------------------------------------------------------------------

add_library(AcaEngine STATIC ${SOURCE_FILES})
set_target_properties(AcaEngine PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED YES
)

# OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(AcaEngine PUBLIC ${OPENGL_LIBRARIES})
list(APPEND INCLUDE_DIR ${OPENGL_INCUDE_DIRS})

# glfw
# find_package(glfw QUIET)
if (glfw_FOUND)
	list(APPEND INCLUDE_DIR  ${GLFW_LIBRARY_DIRS})
else()
	message("Using submodule glfw.")
	set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
	set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
	add_subdirectory ("dependencies/glfw")
	set(GLFW_INCLUDE_DIRS "dependencies/glfw/include")
	set(GLFW_LIBRARIES "glfw")
endif(glfw_FOUND)
list(APPEND INCLUDE_DIR  ${GLFW_INCLUDE_DIRS})
target_link_libraries (AcaEngine PUBLIC ${GLFW_LIBRARIES})

# glew
# find_package(GLEW QUIET)
if (NOT GLEW_FOUND)
	message("Using submodule GLEW.")
	set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "" FORCE)
	set(glew-cmake_BUILD_MULTI_CONTEXT OFF CACHE BOOL "" FORCE)
	set(ONLY_LIBS ON CACHE BOOL "" FORCE)
	add_subdirectory ("dependencies/glew/")
	set(GLEW_INCLUDE_DIRS "dependencies/glew/include")
	set(GLEW_LIBRARIES "libglew_static")
endif(NOT GLEW_FOUND)
list(APPEND INCLUDE_DIR  ${GLEW_INCLUDE_DIRS})
target_link_libraries (AcaEngine PUBLIC ${GLEW_LIBRARIES})

# glm
find_package(glm QUIET)
if (NOT glm_FOUND)
	message("Using submodule glm.")
	set(GLM_INCLUDE_DIRS "dependencies/glm")
endif(NOT glm_FOUND)
list(APPEND INCLUDE_DIR  ${GLM_INCLUDE_DIRS})

# spdlog
# find_package(spdlog QUIET)
if (NOT spdlog_FOUND)
	message("Using submodule spdlog.")
	add_subdirectory ("dependencies/spdlog")
endif(NOT spdlog_FOUND)
target_link_libraries (AcaEngine PUBLIC spdlog::spdlog)

# freetype
# find_package(Freetype QUIET)
if (NOT FREETYPE_FOUND)
	message("Using submodule Freetype.")
	set(CMAKE_DISABLE_FIND_PACKAGE_ZLIB ON CACHE BOOL "" FORCE)
	set(CMAKE_DISABLE_FIND_PACKAGE_BrotliDec ON CACHE BOOL "" FORCE)
	set(CMAKE_DISABLE_FIND_PACKAGE_HarfBuzz ON CACHE BOOL "" FORCE)
	set(CMAKE_DISABLE_FIND_PACKAGE_BZip2 ON CACHE BOOL "" FORCE)
	set(CMAKE_DISABLE_FIND_PACKAGE_PNG ON CACHE BOOL "" FORCE)
	add_subdirectory ("dependencies/freetype")
	target_link_libraries (AcaEngine PRIVATE freetype)
else()
	target_link_libraries(AcaEngine PRIVATE ${FREETYPE_LIBRARIES})
	target_include_directories(AcaEngine PRIVATE ${FREETYPE_INCLUDE_DIRS})
endif(NOT FREETYPE_FOUND)

# stb_image
list(APPEND INCLUDE_DIR  "dependencies/stb")

target_include_directories(AcaEngine PUBLIC ${INCLUDE_DIR})
target_link_libraries(Game AcaEngine)
target_include_directories(Game PRIVATE ${INCLUDE_DIR})

enable_testing()
add_subdirectory(tests)