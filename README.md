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

Physics has a first educational implementation in `include/Physics` and `src/Physics`. Audio remains planned until it has concrete responsibilities and an implementation.

## Physics prototype

`PhysicsWorld` owns rigid bodies by entity ID. Create a body with `world.CreateBody(entity, CollisionShape::AABB, mass, halfExtents)` or `CollisionShape::Sphere` (the `x` dimension is its radius), then call `world.Step(scene, deltaTime)` each frame. A zero mass makes a static body. Forces accumulate until the next step; impulses change velocity immediately. Gravity is acceleration in world units per second squared.

The integrator is semi-implicit Euler: acceleration updates velocity, then velocity updates position. The prototype resolves AABB/AABB, sphere/sphere, and sphere/AABB overlaps using positional correction, restitution, and Coulomb-style friction. It iterates the contact solver eight times per step. Collider dimensions are scaled by the entity transform.

Rigid bodies also support angular velocity (radians/second), accumulated torque, and principal inertia for boxes and spheres. Collision impulses at contact points can change angular velocity. The demo applies torque to the falling cube. Press `F2` to show physics debug lines: AABBs are green, spheres cyan, and current contact normals red. Box colliders remain axis-aligned while their rendered mesh can rotate; oriented-box collision, angular damping, continuous collision detection, and a broad phase are future improvements. For stability, the demo advances physics at a fixed 1/120-second timestep.

## Build

Configure with CMake, then build the `FuxEngine` target. CMake fetches GLFW and GLM and copies `assets/` next to the executable.
