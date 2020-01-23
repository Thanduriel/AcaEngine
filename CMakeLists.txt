cmake_minimum_required (VERSION 3.12)
project (AcaEngine)

#---------------------------------------------------------------------------------------
# main project
#---------------------------------------------------------------------------------------

# find all source and header files
file(GLOB_RECURSE SOURCE_FILES "src/*.hpp" "src/*.cpp" "src/*.inl")

# organize the files according to the organisation in the file system
# from: http://cmake.3232098.n2.nabble.com/Keep-folder-structure-in-Visual-Studio-Project-generated-with-CMake-td7586044.html
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

add_executable(AcaEngine ${SOURCE_FILES})
set_target_properties(AcaEngine PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
)
target_include_directories(AcaEngine PRIVATE "src")

#---------------------------------------------------------------------------------------
# libraries
#---------------------------------------------------------------------------------------

# OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(AcaEngine PRIVATE ${OPENGL_gl_LIBRARY})

# glfw
find_package(glfw QUIET)
if (glfw_FOUND)
	target_link_directories(AcaEngine PRIVATE ${GLFW_LIBRARY_DIRS})
else()
	set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
	set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
	add_subdirectory ("dependencies/glfw")
	set(GLFW_INCLUDE_DIRS "dependencies/glfw/include")
	set(GLFW_LIBRARIES "glfw")
endif(glfw_FOUND)
target_include_directories(AcaEngine PRIVATE ${GLFW_INCLUDE_DIRS})
target_link_libraries (AcaEngine PRIVATE ${GLFW_LIBRARIES})

# glew
find_package(GLEW QUIET)
if (NOT GLEW_FOUND)
	set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "" FORCE)
	set(glew-cmake_BUILD_MULTI_CONTEXT OFF CACHE BOOL "" FORCE)
	set(ONLY_LIBS ON CACHE BOOL "" FORCE)
	add_subdirectory ("dependencies/glew/build/cmake")
	set(GLEW_INCLUDE_DIRS "dependencies/glew/include")
	set(GLEW_LIBRARIES "glew_s")
endif(NOT GLEW_FOUND)
target_include_directories(AcaEngine PRIVATE ${GLEW_INCLUDE_DIRS})
target_link_libraries (AcaEngine PRIVATE ${GLEW_LIBRARIES})

# glm
find_package(glm QUIET)
if (NOT glm_FOUND)
	set(GLM_INCLUDE_DIRS "dependencies/glm")
endif(NOT glm_FOUND)
target_include_directories(AcaEngine PRIVATE ${GLM_INCLUDE_DIRS})

# spdlog
find_package(spdlog QUIET)
if (NOT spdlog_FOUND)
	add_subdirectory ("dependencies/spdlog")
endif(NOT spdlog_FOUND)
target_link_libraries (AcaEngine PRIVATE spdlog::spdlog)
