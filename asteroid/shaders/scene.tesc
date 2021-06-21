//! #version 430

in vec3 tcs_pos[];
in vec3 tcs_normal[];
in vec3 tcs_displaced_pos[];
in float tcs_scale[];
in vec3 tcs_noise_seed[];
in mat4 tcs_instance_matrix[];

layout(vertices=3) out;
out vec3 tes_pos[];
out vec3 tes_normal[];
out mat4 tes_instance_matrix[];
out float tes_scale[];
out vec3 tes_noise_seed[];

uniform float tess_level;
uniform vec3 view_position;
uniform int is_shadow_pass;
uniform int is_asteroid_belt;

float tesselationLevel(float va, float vb) {
    float average = (va + vb) / 2;
    float factor = 20;
    return  clamp(1/average * factor, 1, 30);
}

void main(){
    tes_normal[gl_InvocationID] = tcs_normal[gl_InvocationID];
    tes_pos[gl_InvocationID] = tcs_pos[gl_InvocationID];
	tes_instance_matrix[gl_InvocationID] = tcs_instance_matrix[gl_InvocationID];
	tes_scale[gl_InvocationID] = tcs_scale[gl_InvocationID];
	tes_noise_seed[gl_InvocationID] = tcs_noise_seed[gl_InvocationID];

	if (is_asteroid_belt == 1) {
		float shadows_tessellation_level = 1;
		gl_TessLevelOuter[0] = shadows_tessellation_level;
		gl_TessLevelOuter[1] = shadows_tessellation_level;
		gl_TessLevelOuter[2] = shadows_tessellation_level;

		gl_TessLevelInner[0] = shadows_tessellation_level;
	} else if (is_shadow_pass == 1 && is_asteroid_belt == 0) {
		float shadows_tessellation_level = 5;
		gl_TessLevelOuter[0] = shadows_tessellation_level;
		gl_TessLevelOuter[1] = shadows_tessellation_level;
		gl_TessLevelOuter[2] = shadows_tessellation_level;

		gl_TessLevelInner[0] = shadows_tessellation_level;
    } else if (is_shadow_pass == 0 && is_asteroid_belt == 0){
		float v0_distance = distance(view_position, tcs_displaced_pos[0].xyz);
		float v1_distance = distance(view_position, tcs_displaced_pos[1].xyz);
		float v2_distance = distance(view_position, tcs_displaced_pos[2].xyz);

		float e0t = tesselationLevel(v1_distance, v2_distance);
		float e1t = tesselationLevel(v2_distance, v0_distance);
		float e2t = tesselationLevel(v0_distance, v1_distance);

		float innert = (e0t + e1t + e2t) / 3;

		gl_TessLevelOuter[0] = e0t;
		gl_TessLevelOuter[1] = e1t;
		gl_TessLevelOuter[2] = e2t;

		gl_TessLevelInner[0] = innert;
    }
}
