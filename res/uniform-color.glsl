#shader vertex   ------------------------------------------------------------------------------------------

#version 440 core

layout(location = 0) in vec4 position;

void main() {
    gl_Position = position;
}

#shader fragment ------------------------------------------------------------------------------------------

#version 440 core

uniform vec3 target_color;
out vec4 color;

void main() {
    color = vec4(target_color.xyz, 1.0);
}
