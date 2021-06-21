//! #version 430
//! #include "surface.glsl"
//! #include "constants.glsl"

layout(triangles, fractional_even_spacing) in;
in vec3 tes_pos[];
in vec3 tes_normal[];
in float tes_scale[];
in vec3 tes_noise_seed[];
in mat4 tes_instance_matrix[];

uniform	mat4 model, view, projection;
uniform	mat4 normal_matrix;
uniform mat4 light_view;
uniform mat4 light_projection;
uniform vec3 view_position;
uniform int is_asteroid_belt;

out FragmentShaderInput {
	vec3 frag_pos;
	vec3 original_pos;
	vec3 original_normal;
	vec4 frag_pos_light_space;
} tese_output;

vec3 lerp3D(vec3 v0, vec3 v1, vec3 v2) {
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

void main() {
    vec3 pos = lerp3D(tes_pos[0],tes_pos[1],tes_pos[2]);
    vec3 normal = lerp3D(tes_normal[0], tes_normal[1], tes_normal[2]);

	if (is_asteroid_belt == 1) {
		float scale = tes_scale[0];
		vec3 noise_seed = tes_noise_seed[0];
		vec3 displacement_sample_point = pos * scale + noise_seed;

		float displacement = displacementAtSurfacePos(displacement_sample_point, 
			asteroid_field_octaves, asteroid_field_frequency) * scale;
		vec3 displacement_vector = displacement * normal;

		vec3 displaced_pos = pos + displacement_vector;

		mat4 instance_matrix = tes_instance_matrix[0];
		vec3 instanced_pos = (instance_matrix * vec4(displaced_pos, 1)).xyz;

		tese_output.original_pos = pos;
		tese_output.frag_pos = instanced_pos;
		tese_output.original_normal = normal;
		tese_output.frag_pos_light_space = light_projection * light_view * vec4(instanced_pos, 1.0);
		gl_Position = projection * view * vec4(instanced_pos, 1);
	} else {
		float displacement = displacementAtSurfacePos(pos, main_asteroid_tesse_octaves, main_asteroid_frequency);
		vec3 displacement_vector = displacement * normal;

		vec3 displaced_pos = pos + displacement_vector;

		tese_output.original_pos = pos;
		tese_output.frag_pos = displaced_pos;
		tese_output.original_normal = normal;
		tese_output.frag_pos_light_space = light_projection * light_view * vec4(displaced_pos, 1.0);
		gl_Position = projection * view * vec4(displaced_pos, 1.0 );
	}
}
