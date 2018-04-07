#version 410

in vec2 uv;
out mediump vec4 fragColor;
uniform sampler2D textureSampler;

void main()
{
    //fragColor = vec4(1.0, 0.0, 0.0, 1.0);
    vec4 texColor = texture(textureSampler, uv).rgba;
    fragColor = vec4(texColor.r, texColor.g, texColor.b, texColor.a);
}
