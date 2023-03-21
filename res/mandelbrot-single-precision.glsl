#shader vertex   ------------------------------------------------------------------------------------------
#version 440 core

layout(location = 0) in vec4 position;

void main() {
    gl_Position = position;
}

#shader fragment ------------------------------------------------------------------------------------------
#version 440 core

out vec4 color;

vec3 calculate_mandelbrot(vec2 p) {    
    vec2 z = vec2(0);  

    for (int i = 0; i < 128; ++ i) {  
        z = vec2(z.x * z.x - z.y * z.y, 2. * z.x * z.y) + p; 


        if (dot(z, z) > 4.0)
            return vec3(sin(0.7 * i), sin(i + 2), cos(i * 0.9)) + vec3(0.4);
    }

    return vec3(0);
}

uniform vec2 resolution;

uniform float zoom;
uniform vec2 position;

uniform float antialiasing_level;

void main() {
    vec2 mandelbrot_position = (gl_FragCoord.xy / resolution.xy * 2. - 1.) *
        vec2(resolution.x / resolution.y, 1) * zoom + position;

    vec3 output_color = vec3(0);

    if (antialiasing_level == 0.0)
        output_color = calculate_mandelbrot(mandelbrot_position);
    else {
        float e = 1.0 / min(resolution.y , resolution.x);    
        for (float i = -antialiasing_level; i < antialiasing_level; ++ i)
            for (float j = -antialiasing_level; j < antialiasing_level; ++ j)
                output_color += calculate_mandelbrot(mandelbrot_position + zoom * vec2(i, j) *
                                                    (e / antialiasing_level))
                    / (4.0 * antialiasing_level * antialiasing_level);
    }

    color = vec4(output_color.xyz, 1);
}
