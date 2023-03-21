#shader vertex   ------------------------------------------------------------------------------------------

#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

out vec4 frag_color;

void main() {
    frag_color = color;
    gl_Position = position;
}

#shader fragment ------------------------------------------------------------------------------------------

#version 460 core

in vec4 frag_color;
out vec4 color;

uniform vec3 light_source;

void main() {

    float radius = 0.7f;

    // vec3 light_source = vec3(1.0f, 1.0f, 1.0f) * 7.0f;
    vec3 view_source = vec3(0.0f, 0.0f, -5 * radius);
    vec2 resolution = vec2(1080, 1080);

    vec2 normalized_coord = 2 * (gl_FragCoord.xy / resolution.xy) - 1.0f;
    float z = sqrt(radius * radius - dot(normalized_coord, normalized_coord));

    vec3 position = vec3(normalized_coord.xy, z);

    vec3 relative_light = normalize(light_source - position);
    vec3 relative_view  = normalize(view_source - position);

    vec3 normal = normalize(position);

    float diffuse = dot(normal, relative_light);
    float specular = pow(dot(relative_light - 2 * diffuse * normal, relative_view), 15);

    if (specular < 0)
        specular = 0;

    specular *= 0.8;

    if (diffuse < 0)
        diffuse = 0;

    vec4 ambient = vec4(0.1f, 0.05f, 0.4f, 1.0f) * 0.4;

    if (length(position.xy) <= radius)
        color = clamp(specular, 0, 1) * vec4(1, 1, 1, 1) * 0.5 + diffuse * vec4(1, 1, 1, 1) * 0.5 + ambient;
}
