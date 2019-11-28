cmake_minimum_required (VERSION 3.8)

# solution
project(TerrainGenerator)
# project
set(APP_NAME "TerrainGenerator")

set( CMAKE_AUTORCC ON )
set( CMAKE_AUTOUIC ON )
set( CMAKE_AUTOMOC ON )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 QUIET COMPONENTS Widgets Concurrent OpenGL)
find_package(glm QUIET)
find_package(GLEW QUIET)    
find_package(ASSIMP REQUIRED QUIET)   

find_path(ASSIMP_INCLUDE_DIR assimp/config.h HINTS ${ASSIMP_DIR}/../../../include)
find_library(ASSIMP_LIBRARY ${ASSIMP_LIBRARIES} HINTS ${ASSIMP_DIR}/../../../lib) 

set(SHADER_FILES
   src/shaders/depthMap.vert 
   src/shaders/terrain.vert
   src/shaders/depthMap.frag 
   src/shaders/terrain.frag
)

set(GUI_FILES
   src/mainwindow.ui
)

set(RESOURCE_FILES
   src/resources.qrc 
)

set(HEADER_FILES
   src/AABB.h
   src/Application.h
   src/AssimpIO.h
   src/Camera.h
   src/ConcurrencyHandler.h
   src/DirectionalLight.h
   src/FileLoader.h
   src/FreeLookCamera.h
   src/FreeLookOrthoCamera.h
   src/Heightmap.h
   src/HeightmapGenerator.h
   src/Light.h
   src/MainWindow.h
   src/Material.h
   src/Mesh.h
   src/MouseEventHandler.h
   src/MyGLWidget.h
   src/OrbitCamera.h
   src/OrbitPerspectiveCamera.h
   src/OrthoCamera.h
   src/PerspectiveCamera.h
   src/ProjectionCamera.h
   src/Renderer.h
   src/Shader.h
   src/ShaderProgram.h
   src/Utility.h
   src/VerticalRangesBar.h
   src/ViewCamera.h
)

set(SOURCE_FILES
   src/AABB.cpp
   src/Application.cpp
   src/AssimpIO.cpp
   src/ConcurrencyHandler.cpp
   src/FileLoader.cpp
   src/FreeLookCamera.cpp
   src/FreeLookOrthoCamera.cpp
   src/Heightmap.cpp
   src/HeightmapGenerator.cpp
   src/main.cpp
   src/MainWindow.cpp
   src/Material.cpp
   src/Mesh.cpp
   src/MouseEventHandler.cpp
   src/MyGLWidget.cpp
   src/OrbitCamera.cpp
   src/OrbitPerspectiveCamera.cpp
   src/OrthoCamera.cpp
   src/PerspectiveCamera.cpp
   src/Renderer.cpp
   src/Shader.cpp
   src/ShaderProgram.cpp
   src/Utility.cpp
   src/VerticalRangesBar.cpp
)

# Define a grouping for source files in IDE project generation
source_group("Shader Files" FILES ${SHADER_FILES})

# Define a grouping for source files in IDE project generation
source_group("GUI Files" FILES ${GUI_FILES})

# Define a grouping for source files in IDE project generation
source_group("Resource Files" FILES ${RESOURCE_FILES})

# Define a grouping for source files in IDE project generation
source_group("Source Files" FILES ${SOURCE_FILES})

# Define a grouping for source files in IDE project generation
source_group("Header Files" FILES ${HEADER_FILES})

add_executable(${APP_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${GUI_FILES} ${RESOURCE_FILES} ${SHADER_FILES})

target_include_directories(${APP_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src
                                       PUBLIC ${ASSIMP_INCLUDE_DIR}
                                       )                                     

target_link_libraries(${APP_NAME} Qt5::Widgets 
                                  Qt5::Concurrent
                                  Qt5::OpenGL
                                  glm
                                  GLEW::GLEW 
                                  ${ASSIMP_LIBRARY}
                                  )