 #version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in uvec4 textures;
layout(location = 2) in uvec4 user_data;

out vec3 tdata;
out vec4 tint;

uniform samplerBuffer tp_data; 

void main(void)
{
	gl_Position = vec4(position.xy, 0.0, 1.0);
	tdata = texelFetch(tp_data, int(textures.x)).xyz;
	tint = vec4(user_data.x / 256.0f, user_data.y / 256.0f, user_data.z / 256.0f,  user_data.w / 256.0f);
}
