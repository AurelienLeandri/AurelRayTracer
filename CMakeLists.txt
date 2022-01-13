include("./.env.cmake" RESULT_VARIABLE LOCAL_ENV)
message(STATUS "Local .env.cmake: ${LOCAL_ENV}")

cmake_minimum_required(VERSION 3.11.0)

set(NAME AurelRayTracer)

message(STATUS "Using generator ${CMAKE_GENERATOR}")
if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
    if (NOT MINGW_PATH)
      message(FATAL_ERROR "MINGW_PATH not set in .env.cmake")
    endif()
    set(USE_MINGW "True")
    set(CMAKE_C_COMPILER ${MINGW_PATH}/bin/gcc.exe)
    set(CMAKE_CXX_COMPILER  ${MINGW_PATH}/bin/g++.exe)
endif()

project(${NAME} VERSION 1.0.0)

if (NOT DEFINED GLFW_LIB_PATH OR NOT DEFINED GLFW_INCLUDE_PATH)
  message(FATAL_ERROR "GLFW_LIB_PATH and GLFW_INCLUDE_PATH are not set in .env.cmake!")
endif()

message("Using GLFW lib found in ${GLFW_LIB_PATH}")

if (NOT DEFINED ASSIMP_INCLUDE_PATH OR NOT DEFINED ASSIMP_LIB_PATH)
  message(FATAL_ERROR "ASSIMP_LIB_PATH and ASSIMP_INCLUDE_PATH is not set in .env.cmake!")
endif()

message("Using Assimp lib found in ${ASSIMP_LIB_PATH}")

if (NOT DEFINED EMBREE_INCLUDE_PATH OR NOT DEFINED EMBREE_LIB_PATH)
  message(FATAL_ERROR "EMBREE_LIB_PATH and EMBREE_INCLUDE_PATH is not set in .env.cmake!")
endif()

message("Using Embree lib found in ${EMBREE_LIB_PATH}")

if (NOT DEFINED GLAD_INCLUDE_PATH OR NOT DEFINED GLAD_LIB_PATH)
  message(FATAL_ERROR "GLAD_LIB_PATH and EMBREE_INCLUDE_PATH is not set in .env.cmake!")
endif()

message("Using GLAD lib found in ${EMBREE_LIB_PATH}")

include_directories(${INCLUDE_PATH})
message("Additional include directory: ${INCLUDE_PATH}")

include_directories(src)


file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/external/*.cpp ${PROJECT_SOURCE_DIR}/external/*.c)

add_executable(${PROJECT_NAME} ${SOURCES})
 
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")

message(STATUS "CREATING BUILD FOR WINDOWS")

if (USE_MINGW)
  target_include_directories(${PROJECT_NAME} PUBLIC
    ${MINGW_PATH}/include
  )
  target_link_directories(${PROJECT_NAME} PUBLIC
    ${MINGW_PATH}/lib
  )
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
  ${PROJECT_SOURCE_DIR}/src
  ${GLFW_INCLUDE_PATH}
  ${ASSIMP_INCLUDE_PATH}
  ${EMBREE_INCLUDE_PATH}
  ${GLAD_INCLUDE_PATH}
  ${INCLUDE_PATH}
  )

target_link_directories(${PROJECT_NAME} PUBLIC
  ${GLFW_LIB_PATH}
  ${ASSIMP_LIB_PATH}
  ${EMBREE_LIB_PATH}
  ${GLAD_LIB_PATH}
  ${LIB_PATH}
)

target_link_libraries(${PROJECT_NAME} glfw3 embree3 assimp-vc140-mt)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/external/bin"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)
