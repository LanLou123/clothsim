#version 330  

in vec4 Color;
in float intensity;
out vec4 FragColor;  
uniform mat4 gScale;


void main()
{
    float ambientVal =0.15f;
    FragColor = clamp(intensity*1.1f,0,1)*vec4(0.f,0.f,1.f,1.0)+vec4(ambientVal,ambientVal,ambientVal,1);
}