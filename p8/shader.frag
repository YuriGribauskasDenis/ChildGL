#version 450 core

$GLMatrices

in TESE_OUT {
	vec2 textureCoord;
	vec3 eyeDir;
    vec3 lightDir;
	vec3 worCord;
	vec3 eyeCord;
} from_tese;

in float height;

uniform	sampler2D texUnitHeightMap;
uniform	sampler2D normalMap;
uniform sampler1D colorGradient;

uniform vec3 lightpos;
//uniform vec3 light_pos = vec3(0.0, 100.0, 100.0);
//uniform vec3 light_pos;
uniform vec4 fogColor = vec4(0.7, 0.7, 0.7, 1.0);
uniform bool useFog;
uniform bool useLight;
uniform int PP;
uniform vec3 lightDirUI;
uniform float eat, eati, emit, emiti;

out vec4 finalColor;

vec4 mist(vec4 color)
{
	float z = length(from_tese.eyeCord);
	float de = eati*smoothstep(0.0,eat,10.0-from_tese.worCord.y);
	float di = emiti*smoothstep(0.0,emit,20.0-from_tese.worCord.y);
	float forceExtinct = exp(-z*de);//absorb light
	float forceInscatt = exp(-z*di);//emit light
	return color*forceExtinct+fogColor*(1.0-forceInscatt);
}

vec3 lightAndShadow (vec3 landColor)
{
    vec3 V = normalize(from_tese.eyeDir);
    vec3 L = normalize(from_tese.lightDir);
    vec3 N = normalize(texture(normalMap, from_tese.textureCoord).rgb*2.0f - vec3(1.0f));
	vec3 R = reflect(-L,N);
	vec3 diff = max(dot(N,L),0.0)*landColor.xyz;
	vec3 specShine = vec3(0.1);
	vec3 spec = max(pow(dot(R,V),5.0),0.0)*specShine;
	return diff+spec;
}

vec3 lightAndShadow2(vec3 landColor)
{
    vec3 V = normalize(from_tese.eyeDir);
    vec3 L = normalize(from_tese.lightDir);
    vec3 N = normalize(texture(normalMap, from_tese.textureCoord).rgb*2.0f - vec3(1.0f));
 vec3 color = max(0.0, dot(N, normalize(L-V))) * landColor;

  return color;
  }

void main()
{
	vec4 landColor = texture(colorGradient, height);
	switch(int(PP))
	{
		case 0:
		break;
		case 1:
			landColor = mist(landColor);
		break;
		case 2:
			landColor = vec4(lightAndShadow(landColor.xyz),landColor.w);
		break;
		case 3:
			landColor = vec4(lightAndShadow(landColor.xyz),landColor.w);
		break;
		default:
		break;
	};
	finalColor = landColor;
	/*
//=======================basic
	vec4 landColor = texture(colorGradient, height);
	//=================================mist
	if (useFog)
	{
		landColor = mist(landColor);
	}
	//=========================light
    vec3 V = normalize(from_tese.eyeDir);
    vec3 L = normalize(from_tese.lightDir);
    vec3 N = normalize(texture(normalMap, from_tese.textureCoord).rgb*2.0f - vec3(1.0f));
	vec3 R = reflect(-L,N);
	vec3 diff = max(dot(N,L),0.0)*landColor.xyz;
	vec3 specShine = vec3(0.1);
	vec3 spec = max(pow(dot(R,V),5.0),0.0)*specShine;
	if (useLight)
	{
		landColor = vec4(diff+spec,landColor.w);
	}
	finalColor = landColor;
	*/
}
