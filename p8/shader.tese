#version 450 core

layout (quads, equal_spacing, ccw) in;

$GLMatrices

uniform	sampler2D texUnitHeightMap;
uniform float depth;
uniform vec3 lightpos;
uniform vec3 light_pos = vec3(0.0, 100.0, 100.00);
uniform int PP;
uniform vec3 lightDirUI;

in TESC_OUT {
	vec2 textureCoord;
} from_tesc[];

out TESE_OUT {
	vec2 textureCoord;
	vec3 eyeDir;//reserved for tangens
    vec3 lightDir;//tangent light
	vec3 worCord;
	vec3 eyeCord;
} from_tese;

out float height;

void main()
{
	vec2 t1 = mix(from_tesc[0].textureCoord, from_tesc[1].textureCoord, gl_TessCoord.x);
	vec2 t2 = mix(from_tesc[2].textureCoord, from_tesc[3].textureCoord, gl_TessCoord.x);

	from_tese.textureCoord = mix(t2, t1, gl_TessCoord.y);

	vec4 p1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
	vec4 p2 = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);

	vec4 p = mix(p2, p1, gl_TessCoord.y);
	height = texture(texUnitHeightMap, from_tese.textureCoord).x;
	p.y += height * depth;

	//vec4 p01 = gl_in[0].gl_Position;
	//vec4 p02 = gl_in[1].gl_Position;
	//vec4 p03 = gl_in[2].gl_Position;
	//vec3 v1 = normalize(p02.xyz - p01.xyz);
	//vec3 v2 = normalize(p03.xyz - p01.xyz);

	gl_Position = modelviewprojMatrix * p;
	//============================================mist
	vec4 eyeP = modelviewMatrix * p;
	from_tese.worCord = p.xyz;
	from_tese.eyeCord = eyeP.xyz;
	//============================================light
	vec4 P = modelviewMatrix * p;
	vec4 light = vec4(0.0f);
	vec3 L = light.xyz;
	switch(int(PP))
	{
		case 0:
			light = viewMatrix*vec4(light_pos,1.0);
			L = light.xyz - P.xyz;
		break;
		case 1:
			light = viewMatrix*vec4(light_pos,1.0);
			L = light.xyz - P.xyz;
		break;
		case 2:
			light = viewMatrix*vec4(light_pos,1.0);
			L = light.xyz - P.xyz;
		break;
		case 3:
			light = viewMatrix*vec4(lightDirUI,1.0);
			L = light.xyz;
		break;
		default:
			light = viewMatrix*vec4(light_pos,1.0);
			L = light.xyz - P.xyz;
		break;
	};
	vec3 normal = vec3(0.0,1.0,0.0);
	vec3 tangent = vec3(1.0,0.0,0.0);
	vec3 N = normalize(mat3(modelviewMatrix)*normal);
	vec3 T = normalize(mat3(modelviewMatrix)*tangent);
	vec3 B = cross(N, T);
	//mat3 TBN = mat3(T, B, N);
	from_tese.lightDir = normalize(vec3(dot(L,T),dot(L,B),dot(L,N)));
	vec3 V = -P.xyz;
	from_tese.eyeDir = normalize(vec3(dot(V,T),dot(V,B),dot(V,N)));
}
