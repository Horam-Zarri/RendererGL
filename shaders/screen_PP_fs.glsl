#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloomTexture;

uniform bool sharpen;
uniform float sharpness;

uniform bool blur;
uniform bool grayscale;

uniform bool gamma;
uniform bool hdr;
uniform bool bloom;

uniform float exposure=1.0;

const float offset = 1.0 / 600.0;

vec2 offsets[9] = vec2[](
    vec2(-offset, offset), // top-left
    vec2( 0.0f, offset), // top-center
    vec2( offset, offset), // top-right
    vec2(-offset, 0.0f), // center-left
    vec2( 0.0f, 0.0f), // center-center
    vec2( offset, 0.0f), // center-right
    vec2(-offset, -offset), // bottom-left
    vec2( 0.0f, -offset), // bottom-center
    vec2( offset, -offset) // bottom-right
);

vec3 process_sharpness() {
    const float sharpness_kernel[9] = float[] (
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );

    vec3 sampleTex[9];
    for (int i = 0; i < 9; i++) {
        // multiply sharpness amount
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st
                            + (offsets[i] * sharpness)));
    }

    vec3 col = vec3(0.0f);

    for (int i = 0; i < 9; i++)
        col += sampleTex[i] * sharpness_kernel[i];

    return col;
}

vec3 process_blur() {
    const float blur_kernel[9] = float[] (
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16
    );

    vec3 sampleTex[9];
    for (int i = 0; i < 9; i++) {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st
                            + offsets[i]));
    }

    vec3 col = vec3(0.0f);

    for (int i = 0; i < 9; i++)
        col += sampleTex[i] * blur_kernel[i];

    return col;
}

void main()
{
    vec3 color;

    if (sharpen) {
        color = process_sharpness();
    } else if (blur) {
        color = process_blur();
    } else {
        color = texture(screenTexture, TexCoords).rgb;

        if (grayscale) {
            const float r_weight = 0.2126;
            const float g_weight = 0.7152;
            const float b_weight = 0.0722;

            float avg = r_weight * color.r + g_weight * color.g
                + b_weight * color.b;

            color = vec3(avg, avg, avg);
        }
    }

    if (bloom)
        color += texture(bloomTexture, TexCoords).rgb;

    if (hdr)
        color = vec3(1.0) - exp(-color * exposure);

    if (gamma)
        color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}
