//? #version 430

uniform float rim_width;
uniform float rim_steepness;

uniform float noise_strength;

struct crater {
	vec4 center;
	float size;
	float smoothness;
	float depth;
	float rim_width;
	float rim_steepness;
};

layout(std430, binding = 0) buffer craters_buffer {
	crater craters[];
};

float hash(float n) {
	return fract(sin(n) * 753.5453123);
}

float snoise(vec3 x) {
	vec3 p = floor(x);
	vec3 f = fract(x);
	f = f * f * (3.0f - (2.0f * f));

	float n = p.x + p.y * 157.0f + 113.0f * p.z;
	return mix(mix(mix(hash(n + 0.0f), hash(n + 1.0f), f.x),
		mix(hash(n + 157.0f), hash(n + 158.0f), f.x), f.y),
		mix(mix(hash(n + 113.0f), hash(n + 114.0f), f.x),
			mix(hash(n + 270.0f), hash(n + 271.0f), f.x), f.y), f.z);
}

float pnoise(vec3 position, float octaves, float frequency, float persistence) {
	float total = 0.0f;
	float amplitude = 1.0f;
	for (int i = 0; i < ceil(octaves); i++) {
		float factor = clamp(octaves - i, 0, 1);
		total += snoise(position * frequency) * amplitude * factor;
		frequency *= 2.0f;
		amplitude *= persistence;
	}
	return total;
}

// https://www.iquilezles.org/www/articles/smin/smin.htm
float smoothMin(float a, float b, float k) {
	k = max(0, k);
	float h = max(0, min(1, (b - a + k) / (2 * k)));
	return a * h + b * (1 - h) - k * h * (1 - h);
}

float smoothMax(float a, float b, float k) {
	k = min(0, -k);
	float h = max(0, min(1, (b - a + k) / (2 * k)));
	return a * h + b * (1 - h) - k * h * (1 - h);
}

float distanceToOctave(float dist, float factor) {
	return  clamp(-log(dist) + factor, 1, 16);
}

float displacementAtSurfacePos(vec3 pos, float octaves, float frequency) {
	float displacement = 0;

	for (int i = 0; i < craters.length(); i++) {
		float x = length(pos - craters[i].center.xyz) / craters[i].size;

		float cavity = x * x - 1;

		float rim_x = min(x - 1 - rim_width, 0);

		float rim = rim_steepness * rim_x * rim_x;

		float smoothness = 1.0;

		float crater_shape = smoothMax(cavity, craters[i].depth, smoothness);
		crater_shape = smoothMin(crater_shape, rim, craters[i].smoothness);
		displacement += crater_shape * craters[i].size;
	}

	displacement += pnoise(pos.xyz, octaves, frequency, 0.45) * noise_strength;
	return displacement;
}

vec3 recomputeNormals(vec3 pos, vec3 normal, float octaves, float frequency, float displacement_factor) {
	// pick unit vector for cross product with the normal
	vec3 ux = vec3(1, 0, 0);
	vec3 uy = vec3(0, 1, 0);
	vec3 uz = vec3(0, 0, 1);

	float dx = dot(normal, ux);
	float dy = dot(normal, uy);
	float dz = dot(normal, uz);

	vec3 u;
	if (dx <= dy && dx <= dz) {
		u = ux;
	}
	else if (dy <= dx && dy <= dz) {
		u = uy;
	}
	else {
		u = uz;
	}

	vec3 tangent1 = normalize(cross(normal, u));
	vec3 tangent2 = cross(normal, tangent1);

	float sample_distance = 0.001;
	float h_at_t1_pos = displacementAtSurfacePos(pos + tangent1 * sample_distance, octaves, frequency);
	float h_at_t1_neg = displacementAtSurfacePos(pos - tangent1 * sample_distance, octaves, frequency);
	float h_at_t2_pos = displacementAtSurfacePos(pos + tangent2 * sample_distance, octaves, frequency);
	float h_at_t2_neg = displacementAtSurfacePos(pos - tangent2 * sample_distance, octaves, frequency);

	h_at_t1_pos *= displacement_factor;
	h_at_t1_neg *= displacement_factor;
	h_at_t2_pos *= displacement_factor;
	h_at_t2_neg *= displacement_factor;

	vec3 sampled_tangent_1 = tangent1 * sample_distance * 2 + h_at_t1_pos * normal - h_at_t1_neg * normal;
	vec3 sampled_tangent_2 = tangent2 * sample_distance * 2 + h_at_t2_pos * normal - h_at_t2_neg * normal;

	vec3 new_normal = normalize(cross(sampled_tangent_1, sampled_tangent_2));
	return new_normal;
}