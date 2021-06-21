# asteroidOpenGL
 A 3D asteroid space scene implemented using OpenGL

<img src="/asteroidOpenGL.gif?raw=true" width="700px">

The main asteroid as well as the smaller belt asteroids both have procedural terrains (generated using 
Perlin noise function) as well as bump mapping for more realistic terrain look. Additionally, main 
asteroid has procedural craters that can be regenerated at runtime.

Tessellation shaders are being used to add dynamic LOD to the main asteroid. The shadow pass has fixed 
tessellation levels for performance reasons, but the full pass uses camera distance to determine the 
tessellation level.

The asteroid belt is implemented using instanced rendering. For each instance a state matrix is passed in 
for any matrix operations as well as a noise vector together with the scale in order to add variety to the 
Perlin noise generator.

The space itself, is simply a skybox texture that also includes a big star that acts as a light source. All the 
objects in the scene cast shadows via a simple shadow map. The surface reflectance is approximated 
using Hapke BRDF.
