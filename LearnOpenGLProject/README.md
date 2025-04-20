# OpenGL Model Viewer

A 3D model viewer built with OpenGL that allows you to load and interact with 3D models.

## Features

- **Model Loading**: Load and view 3D models in common formats (OBJ, FBX, etc.) using the Assimp library
- **Texture Support**: Display models with their textures and materials
- **Interactive Camera**: Navigate freely around models with an intuitive camera system
- **Wireframe Mode**: Toggle between solid and wireframe rendering
- **Model Statistics**: View detailed information about the model (vertex count, triangle count)
- **Auto-Focus**: Automatically adjust the camera to focus on the loaded model
- **Performance Metrics**: Display real-time rendering statistics

## Controls

- **WASD**: Move camera horizontally
- **QE**: Move camera vertically (Q: down, E: up)
- **Mouse**: Rotate camera view
- **Mouse Wheel**: Adjust camera movement speed
- **T**: Toggle wireframe mode
- **F**: Focus/center camera on model
- **Arrow Up/Down**: Increase/decrease camera movement speed
- **ESC**: Exit application

## Dependencies

- **OpenGL**: Core graphics API
- **GLFW**: Window creation and input handling
- **GLAD**: OpenGL function loading
- **GLM**: Mathematics library for 3D transformations
- **Assimp**: Model loading library
- **stb_image**: Image loading library

## Usage

Run the application by either:

1. Running the executable directly (loads the default backpack model)
2. Providing a model path as a command-line argument:
   ```
   OpenGLModelViewerProject.exe path/to/your/model.obj
   ```
3. If on Windows, right click on 3d model(fbx, obj and etc) and selct open with, choose another app, choose this app, optionally check "always use this app to open .fbx files"

## Building from Source

This project uses Visual Studio and requires the following libraries to be properly configured:
- GLFW
- GLAD
- GLM
- Assimp

The project includes all necessary shaders and resources in the resources directory.

## Project Structure

- **ModelViewer.cpp**: Main application file
- **Model.h/cpp**: 3D model loading and rendering
- **Mesh.h/cpp**: Mesh representation and rendering
- **Shader.h/cpp**: Shader compilation and management
- **Camera.h/cpp**: Camera system with movement controls
- **resources/shaders/**: Contains all GLSL shader files
- **resources/models/**: Contains example models

## Display Information

The application displays model statistics in the window title bar, including:
- Vertex count
- Triangle count
- Camera movement speed