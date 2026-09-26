# FuxEngine

FuxEngine is a C++20 and OpenGL engine prototype.

## Source architecture

```text
include/ and src/
+-- Core/       Window, Scene, Entity, Camera
+-- Graphics/   Renderer, materials, meshes, shaders, framebuffers
+-- Input/      CameraController (keyboard and mouse camera controls)
+-- Lighting/   Point, directional, and spot lights
+-- Resources/  Shader, texture, material, and OBJ loading
```

`src/FuxEngine.cpp` is the demo application entry point. The engine modules live in matching `include/` and `src/` folders.

Physics and Audio are planned modules. Their folders and APIs will be introduced when those systems have concrete responsibilities and implementations.

## Build

Configure with CMake, then build the `FuxEngine` target. CMake fetches GLFW and GLM and copies `assets/` next to the executable.
