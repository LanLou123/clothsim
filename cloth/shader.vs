#version 330  

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 normal1;
uniform mat4 gScale;
uniform vec3 lightDir;
out vec4 Color;
out float intensity;
uniform mat4 viewproj;

void main()
{
	vec3 dir = normalize(vec3(-5,-20,-1));
	
    //gl_Position = viewproj*gScale*vec4(Position, 1.0);
	gl_Position = viewproj*vec4(Position,1.0);
	intensity = dot(normal1,dir);
	Color = vec4(clamp(Position,0.0,1.0),1.0);
}