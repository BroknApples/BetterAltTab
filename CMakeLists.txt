cmake_minimum_required(VERSION 3.10)

# WINDOWS ONLY PROJECT.
if (NOT WIN32)
  message(FATAL_ERROR "This project requires Windows.")
endif()

project("BetterAltTab" LANGUAGES CXX)

# Use C++17 (or change to 20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set sources
set(SOURCES
  src/main.cpp
)

file(GLOB IMGUI_SOURCES
  src/imgui/imgui*.cpp
  src/imgui/backends/imgui_impl_win32.cpp
  src/imgui/backends/imgui_impl_dx11.cpp
)

# Add executable
add_executable(${PROJECT_NAME}
  ${SOURCES}
  ${IMGUI_SOURCES}
)

# Include dirs
target_include_directories(${PROJECT_NAME} PRIVATE
  src/imgui
  src/imgui/backends
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
  user32
  d3d11
  dxgi
  dxguid
  d3dcompiler
  dwmapi      
)
