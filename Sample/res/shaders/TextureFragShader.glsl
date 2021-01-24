#version 330 core

in vec3 tdata;

out vec4 fragColour;

uniform sampler2D samplers[15];

void main(void)
{
	fragColour = texture(samplers[int(tdata.z)], tdata.xy);
}