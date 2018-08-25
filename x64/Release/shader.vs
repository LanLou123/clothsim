#version 330  

layout (location = 0) in vec3 Position;
uniform mat4 gScale;
out vec4 Color;
uniform mat4 viewproj;

void main()
{
	
    //gl_Position = viewproj*gScale*vec4(Position, 1.0);
	gl_Position = viewproj*vec4(Position,1.0);
	Color = vec4(clamp(Position,0.0,1.0),1.0);
}