#version 450 core

$GLMatrices

in vec3 position;
in vec2 texCoord;

out VS_OUT {
	vec2 textureCoord;
} vs_out;

void main()
{
	int x = gl_InstanceID & 63;
	int z = gl_InstanceID >> 6;
	vs_out.textureCoord = vec2((texCoord.x+float(x))/64,(texCoord.y+float(z))/64);
	vec3 newpos = (position + vec3(64*float(x)-64*32+64, 0.0, 64*float(z)-64*32+64));
	gl_Position = vec4(newpos/64, 1.0);
}
