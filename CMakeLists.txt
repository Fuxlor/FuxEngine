# CMakeList.txt : projet CMake pour FuxEngine, incluez la source et définissez
# la logique spécifique au projet ici.
#
cmake_minimum_required (VERSION 3.10)

# Activez Rechargement à chaud pour les compilateurs MSVC si cela est pris en charge.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(FuxEngine
    VERSION 0.1.0
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ajoutez une source à l'exécutable de ce projet.
add_executable (FuxEngine "src/FuxEngine.cpp" "src/FuxEngine.h" "third_party/glad/src/glad.c" "include/Core/Window.h" "src/Core/Window.cpp" "include/Graphics/Shader.h" "src/Graphics/Shader.cpp" "include/Graphics/VertexBuffer.h" "src/Graphics/VertexBuffer.cpp" "include/Graphics/VertexArray.h" "src/Graphics/VertexArray.cpp" "include/Graphics/Renderer.h" "src/Graphics/Renderer.cpp" "include/Graphics/IndexBuffer.h" "src/Graphics/IndexBuffer.cpp" "include/Graphics/Mesh.h" "src/Graphics/Mesh.cpp" "include/Graphics/BufferLayout.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET FuxEngine PROPERTY CXX_STANDARD 20)
endif()

# TODO: Ajoutez des tests et installez des cibles si nécessaire.

target_include_directories(FuxEngine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
)

FetchContent_MakeAvailable(glfw)

target_link_libraries(FuxEngine PRIVATE glfw)

target_include_directories(FuxEngine PRIVATE
    third_party/glad/include
)