//! #version 430
//! #include "surface.glsl"
//! #include "constants.glsl"

layout(location = 0) in vec3 position;
layout(location = 2) in vec3 normal;
layout (location = 3) in vec3 noise_seed;
layout (location = 4) in float scale;
layout(location = 5) in mat4 instance_matrix;

out vec3 tcs_pos;
out vec3 tcs_displaced_pos;
out vec3 tcs_normal;
out float tcs_scale;
out vec3 tcs_noise_seed;
out mat4 tcs_instance_matrix;

uniform mat4 light_projection, light_view; 
uniform mat4 projection, view;
uniform int is_asteroid_belt;

void main() {
	// not used in shadow pass
	float displacement = displacementAtSurfacePos(position, main_asteroid_vertex_octaves,
		main_asteroid_frequency);

	tcs_displaced_pos = position + displacement * normal;
	tcs_pos = position;
	tcs_normal = normal;
	tcs_instance_matrix = instance_matrix;
	tcs_scale = scale;
	tcs_noise_seed = noise_seed;
}
