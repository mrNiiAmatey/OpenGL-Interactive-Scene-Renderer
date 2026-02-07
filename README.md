# OpenGL Interactive Scene Renderer

A real-time 3D graphics application built with C++ and OpenGL that renders an interactive desktop workspace scene with advanced lighting, texturing, and camera controls.

![C++](https://img.shields.io/badge/C++-00599C?style=flat&logo=c%2B%2B&logoColor=white)
![OpenGL](https://img.shields.io/badge/OpenGL-5586A4?style=flat&logo=opengl&logoColor=white)
![GLFW](https://img.shields.io/badge/GLFW-3.3-blue)
![GLEW](https://img.shields.io/badge/GLEW-2.1-green)
![GLM](https://img.shields.io/badge/GLM-Math-orange)

## 🎯 Project Overview

This project demonstrates proficiency in computer graphics programming by creating a fully interactive 3D scene from scratch. The application renders a realistic desktop workspace complete with a monitor, mug, keyboard, mouse, and books—all using custom shader programs and primitive geometric shapes.

**Key Achievement**: Transformed basic 3D primitives (cylinders, boxes, spheres, toruses) into a cohesive, textured, and lit 3D scene through mathematical transformations and shader programming.

## ✨ Core Features

### Graphics Rendering
- **Custom 3D Scene Composition**: Built complex objects from primitive shapes (cylinders, boxes, spheres, toruses, planes)
- **Advanced Texture Mapping**: Applied realistic textures with UV scaling for materials like stainless steel, wood, and glass
- **Phong Lighting Model**: Implemented multi-light source system with ambient, diffuse, and specular components
- **Material System**: Defined custom materials with configurable shininess, reflectivity, and color properties

### Interactive Camera System
- **Dual Projection Modes**: 
  - Perspective projection (default) with adjustable FOV
  - Orthographic projection for technical views
- **6-DOF Camera Movement**:
  - WASD: Forward/backward/strafe movement
  - Q/E: Vertical movement (up/down)
  - Mouse: 360° look-around capability
- **Smooth Motion**: Frame-rate independent movement using delta time

### Technical Architecture
- **Object-Oriented Design**: Modular architecture with separate managers for shaders, scenes, and views
- **Shader Pipeline**: Custom GLSL vertex and fragment shaders for rendering
- **Memory Management**: Proper resource allocation and deallocation with C++ RAII principles
- **Texture Management**: Efficient texture loading, binding, and slot management system

## 🛠️ Technologies & Libraries

| Technology | Purpose |
|------------|---------|
| **C++** | Core programming language |
| **OpenGL 4.6** | Graphics rendering API |
| **GLFW 3.3** | Window creation and input handling |
| **GLEW** | OpenGL extension loading |
| **GLM** | Mathematics library for 3D transformations |
| **stb_image** | Image loading for textures |

## 📁 Project Structure

```
OpenGL-Interactive-Scene-Renderer/
├── Source/
│   ├── MainCode.cpp           # Application entry point and initialization
│   ├── SceneManager.cpp/h     # 3D scene construction and rendering logic
│   ├── ViewManager.cpp/h      # Camera controls and projection management
│   ├── ShaderManager.cpp/h    # Shader compilation and uniform management
│   └── ShapeMeshes.cpp/h      # Primitive mesh generation
├── Utilities/
│   ├── shaders/
│   │   ├── vertexShader.glsl  # Vertex transformation shader
│   │   └── fragmentShader.glsl # Lighting and texture shader
│   └── textures/              # Texture image assets
└── README.md
```

## 🚀 Getting Started

### Prerequisites
- C++17 compatible compiler (MSVC, GCC, or Clang)
- OpenGL 4.6 compatible graphics card
- CMake 3.10+ (recommended for building)

### Building the Project

```bash
# Clone the repository
git clone https://github.com/yourusername/OpenGL-Interactive-Scene-Renderer.git
cd OpenGL-Interactive-Scene-Renderer

# Build (adjust based on your build system)
mkdir build && cd build
cmake ..
make

# Run
./SceneRenderer
```

### Controls

| Key | Action |
|-----|--------|
| **W/S** | Move camera forward/backward |
| **A/D** | Strafe camera left/right |
| **Q/E** | Move camera up/down |
| **Mouse** | Look around (360° rotation) |
| **P** | Switch to perspective projection |
| **O** | Switch to orthographic projection |
| **ESC** | Exit application |

## 💡 Technical Highlights

### 1. Advanced Transformation Pipeline
```cpp
// Example: Mug rendering with compound transformations
SetTransformations(
    glm::vec3(1.0f, 1.5f, 1.0f),  // Scale
    180.0f, 0.0f, 0.0f,            // Rotation (flipped upside down)
    glm::vec3(3.0f, 0.5f, 3.0f)    // Position
);
```

### 2. Multi-Light Source System
- **Light Source 1**: Blue-tinted point light with high focal strength
- **Light Source 2**: Warm directional light for realistic illumination
- Dynamic light positioning and configurable intensity

### 3. Material Property System
Custom material definitions for realistic surface rendering:
- **Metallic**: High specularity, moderate shininess (monitor stand/base)
- **Glass**: High reflectivity, strong specular highlights (monitor screen)
- **Matte**: Low specularity, soft diffuse (mug texture)

### 4. Efficient Texture Management
- Centralized texture loading and binding system
- Support for multiple texture formats (JPG, PNG)
- Automatic mipmap generation for optimal performance
- Tag-based texture retrieval for easy scene management

## 📊 Performance Considerations

- **Frame-rate independent movement**: Uses delta time for consistent camera speed
- **Depth testing enabled**: Proper Z-buffer handling for correct occlusion
- **Optimized mesh generation**: Reusable primitive meshes loaded once
- **Efficient shader usage**: Single shader program for entire scene

## 🎓 Learning Outcomes

This project demonstrates proficiency in:
- **3D Mathematics**: Transformation matrices, vector operations, quaternion rotations
- **Graphics Pipeline**: Understanding of vertex processing, rasterization, and fragment shading
- **Shader Programming**: GLSL vertex and fragment shader development
- **OpenGL Architecture**: Buffer management, texture handling, and rendering pipeline
- **Software Design**: Modular architecture with separation of concerns
- **C++ Best Practices**: Memory management, RAII, and object-oriented design

## 🔄 Future Enhancements

- [ ] Implement shadow mapping for realistic shadows
- [ ] Add normal mapping for enhanced surface detail
- [ ] Integrate ImGui for runtime scene manipulation
- [ ] Implement skybox for environment reflections
- [ ] Add object picking with mouse interaction
- [ ] Support for loading external 3D models (OBJ, FBX)
- [ ] Post-processing effects (bloom, HDR, anti-aliasing)

## 📝 Academic Context

**Course**: CS-330 Computational Graphics and Visualization  
**Institution**: Southern New Hampshire University  
**Completion**: 2024

This project represents the culmination of coursework in computational graphics, demonstrating the application of:
- Real-time rendering techniques
- 3D scene composition and spatial reasoning
- Interactive camera systems and user input handling
- Shader-based graphics programming

## 🤝 Skills Demonstrated

**Technical Skills:**
- Advanced C++ programming with modern practices
- OpenGL graphics API and rendering pipeline
- 3D mathematics and linear algebra
- Shader programming (GLSL)
- Graphics optimization and performance tuning

**Software Engineering:**
- Object-oriented design and architecture
- Resource management and memory safety
- Modular code organization
- Documentation and code maintainability

## 📧 Contact

**Nii Amatey Tagoe**  
[LinkedIn](https://www.linkedin.com/in/yourprofile) | [Portfolio](https://mrniiamateytagoe.dev) | [GitHub](https://github.com/yourusername)

---

**Note**: This project was developed as part of academic coursework and demonstrates foundational skills in computer graphics programming applicable to game development, simulation, visualization, and graphics-intensive applications.

## 📄 License

This project is available for educational and portfolio purposes. Please credit appropriately if using any code snippets or concepts.

---

*If you found this project interesting or useful, please consider giving it a ⭐!*

