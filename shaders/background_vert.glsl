#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
uniform vec3 base_scale;
uniform vec2 camera_pos;
uniform vec2 position;
uniform float camera_zoom;
out vec2 TexCoords;
void main()
{
	vec2 transformed  = vec2(aPos.x, aPos.y);

	transformed *= vec2(base_scale.x,base_scale.y);
	transformed += position;
	
	transformed *= camera_zoom;
	transformed += -camera_pos;
	
    gl_Position = vec4(transformed,aPos.z,1.0);
    
	TexCoords = aTexCoords;
}