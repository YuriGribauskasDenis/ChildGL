#version 420 core

$GLMatrices
$Lights

// Atributos del vértice (sistema de coordenadas del modelo)
in vec4 position;
in vec2 texCoord;
in vec3 normal;
in vec3 tangent;

// Añade las variables out que necesites
out VS_OUT
{
    vec3 fPosition;
    vec2 texCoord;
    vec3 tangentLightPos;
    vec3 tangentViewPos;
    vec3 tangentFPos;
} vs_out;

void main()
{
	vs_out.texCoord = vec2(texCoord);
    vs_out.fPosition = vec3(modelMatrix*position);
	gl_Position = modelviewprojMatrix * position;

    vec3 eN = normalize(normalMatrix * normal);
    vec3 T = normalize(mat3(modelMatrix) * tangent);
    vec3 B = normalize(mat3(modelMatrix) * cross(eN, tangent));
    vec3 N = normalize(mat3(modelMatrix) * eN);
    mat3 TBN = transpose(mat3(T, B, N));

    vec3 eposition = vec3(modelviewMatrix * position);
    vec3 V = normalize(-eposition);
    vs_out.tangentLightPos = TBN * vec3(lights[0].positionWorld);
    vs_out.tangentViewPos  = TBN * V;
    vs_out.tangentFPos  = TBN * vs_out.fPosition;
}
