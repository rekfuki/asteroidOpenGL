//! #version 430
//! #include "surface.glsl"
//! #include "lighting.glsl"
//! #include "constants.glsl"

out vec4 FragColor;

uniform vec3 view_position;
uniform sampler2D shadowMap;

in FragmentShaderInput {
	vec3 frag_pos;
	vec3 displacement_sample_point;
	vec3 original_normal;
    float displacement_factor;
    mat4 instance_matrix;
	vec4 frag_pos_light_space;
} fs_input;


// Code adapted from https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping0
float compute_shadows(vec4 pos) {
    vec3 proj_coords = pos.xyz * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closest_depth = texture(shadowMap, proj_coords.xy).r; 
    // get depth of current fragment from light's perspective
    float current_depth = proj_coords.z;
    float bias = 0.001;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcf_depth = texture(shadowMap, proj_coords.xy + vec2(x, y) * texelSize).r; 
            shadow += current_depth - bias > pcf_depth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(proj_coords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main() {
    float dist = distanceToOctave(distance(view_position, fs_input.frag_pos), 
        asteroid_field_octaves_factor);
    vec3 norm = recomputeNormals(fs_input.displacement_sample_point, fs_input.original_normal,
        dist, asteroid_field_frequency, fs_input.displacement_factor);

    norm = normalize(mat3(fs_input.instance_matrix) * norm);

    float ambient_strength = 0.03;
    vec3 ambient = ambient_strength * light_color;
  	
    vec3 light_dir = normalize(light_pos - fs_input.frag_pos);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * light_color;
    
    float specular_strength = 0.005;
    vec3 view_dir = normalize(view_position - fs_input.frag_pos);
    vec3 reflect_dir = reflect(-light_dir, norm);  
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specular = specular_strength * spec * light_color;  
        
    vec3 object_color = vec3(0.58, 0.541, 0.470);

    float shadows = compute_shadows(fs_input.frag_pos_light_space);
    vec3 lighting = (ambient + (1 - shadows) * (diffuse));

    float mu  = clamp(dot(-view_dir, norm), 0, 1);  
    float mu0 = clamp(dot(light_pos, norm), 0, 1);
    
    vec3 a = vec3(1.0, 1.0, 1.0);
    vec3 a_ss = Albedo_to_SSAlbedo(a);
    vec3 col = HapkeBRDF(mu, mu0, a_ss) * mu0;
    lighting *= col;
    
    FragColor = vec4(lighting, 1.0);
}