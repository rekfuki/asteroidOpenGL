//? #version 430

vec3 light_color = vec3(1.5, 1.5, 1.5);
vec3 light_pos = vec3(-900, 1, 1);

const float asteroid_field_octaves = 10;
const float asteroid_field_octaves_factor = 11;
const float asteroid_field_frequency = 1.5;

const float main_asteroid_vertex_octaves = 6;
const float main_asteroid_tesse_octaves = 10;
const float main_asteroid_octaves_factor = 11;
const float main_asteroid_frequency = 0.25;