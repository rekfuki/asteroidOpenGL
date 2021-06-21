//! #version 430
//! #include "surface.glsl"
//! #include "constants.glsl"

layout (location = 0) in vec3 pos;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 noise_seed;
layout (location = 4) in float scale;
layout (location = 5) in mat4 instance_matrix;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 light_projection;
uniform mat4 light_view;

out FragmentShaderInput {
	vec3 frag_pos;
	vec3 displacement_sample_point;
	vec3 original_normal;
    float displacement_factor;
    mat4 instance_matrix;
	vec4 frag_pos_light_space;
} vert_output;

void main() {
	vec3 displacement_sample_point = pos * scale + noise_seed;

	float displacement = displacementAtSurfacePos(displacement_sample_point, 
		asteroid_field_octaves, asteroid_field_frequency) * scale;
	vec3 displacement_vector = displacement * normal;

	vec3 new_normal = recomputeNormals(displacement_sample_point, normal,
		asteroid_field_octaves, asteroid_field_frequency, scale);
	vec3 displaced_pos = pos + displacement_vector;

	vert_output.displacement_sample_point = displacement_sample_point;
	vert_output.frag_pos = displaced_pos;
	vert_output.original_normal = normal;
	vert_output.displacement_factor = scale;
	vert_output.instance_matrix = instance_matrix;
	vert_output.frag_pos_light_space = light_projection * light_view * instance_matrix *  vec4(displaced_pos, 1.0);

    gl_Position = projection * view * instance_matrix * vec4(displaced_pos, 1.0f); 
}
