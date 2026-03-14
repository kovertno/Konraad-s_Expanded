# Gridshot – OpenGL Aim Trainer 🎯

A high-performance aim trainer built from scratch in C++ and OpenGL. Instead of relying on heavy physics engines, it utilizes pure vector mathematics for target detection and lighting.

## 🚀 Key Features
* **Vector-Based Hit Detection**: Optimized target detection using **Dot Product** ray alignment between camera orientation and target world positions—eliminating the need for expensive ray-casting.
* **Collision System**: Implemented **AABB (Axis-Aligned Bounding Box)** collision detection for environment boundaries.
* **Dynamic Visuals**: Custom GLSL shaders for player flashlight effects and target highlight states.
* **Performance**: Direct buffer management using VBOs and VAOs for minimal overhead.

## 🛠 Tech Stack
* **Language:** C++
* **Graphics:** OpenGL, GLM
* **Input/Windowing:** GLFW, glad
