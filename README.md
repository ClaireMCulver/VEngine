# VEngine
A framework using Vulkan as a graphics api for my own educational purposes, both for learning vulkan and various engine feature implementations.
This is not meant to be a usable engine for a third-party, nor is that planned at all.

Some features implemented:

- Model loading (.dae format only, currently using rapidxml)
- Texture loading (via FreeImage)
- Keyboard input
- Material system for shaders and uniforms and such (ShaderC for compiling shaders from GLSL to SPIR-V)

- Components system.
  Some features implemented using components:
    - Basic camera controls
    - Particle emitters and particle systems
 ![alt text](https://github.com/PalaceDCXVI/VEngine/blob/master/ExampleImages/Components%26Particles.gif)
    - Basic Boids ai demo (for a university course assignment)
    - Basic drawing program (for a university course assignment)
    - Reasonable* system for multiple renderpasses
      - *Some micromanagment required for renderpass dependent on other renderpasses.
      - This was most recently implemented, so the only screen effect implemented thus far is FXAA.

      ![alt text](https://github.com/PalaceDCXVI/VEngine/blob/master/ExampleImages/fxaa.png)
