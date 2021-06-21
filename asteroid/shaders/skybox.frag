//! #version 430
out vec4 FragColor;

in vec3 tex_coords;

uniform samplerCube skybox;

void main() {    
    FragColor = texture(skybox, tex_coords);
}