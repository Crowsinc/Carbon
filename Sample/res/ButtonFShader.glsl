#version 330 core

in vec3 tdata;
in vec4 tint;

out vec4 fragColour;

uniform sampler2D samplers[15];

void main(void)
{
	fragColour = tint * texture(samplers[int(tdata.z)], tdata.xy);
}