#version 450

$GLMatrices
$Lights
$Material

layout (location = 21) uniform float m;
layout (location = 22) uniform float lambda;

in vec3 fN;
in vec3 fH[lights.length()];
in vec3 fE;
in vec3 fL[lights.length()];

const float one_over_pi = 0.313831f;
const float e = 2.718282f;

out vec4 fragColor;

float Beckmann(vec3 H, vec3 N, float m)
{
    float nDotH = max (0.001f, dot(N,H));
    float nDotH2 = pow(nDotH,2);
    float nDotH4 = pow(nDotH,4);
    float tan2a = (nDotH2-1.0f)/nDotH2;
    return (1.0f/(4*m*m*nDotH4)) * pow(e,tan2a/(m*m));
}

float Gatten (vec3 L, vec3 E, vec3 H, vec3 N)
{
    float nDotH = dot(N,H);
    float nDotE = dot(N,E);
    float nDotL = dot(N,L);
    float hDotE = max (0.001f, dot(H,E));
    float X = 2.0f*nDotH/hDotE;
    return min(1.0f, max(0.0f, max(X*nDotE,X*nDotL)));
}

float Frensel (vec3 E, vec3 N, float lambda)
{
    return pow(1.0+dot(E,N),lambda);
}

void main()
{
        
        vec3 N = normalize (fN);
        vec3 E = normalize (fE);
        vec3 L[lights.length()];
        vec3 H[lights.length()];
        vec4 color = emissive;
        for (int i = 0; i < lights.length(); i++)
        {
            if (lights[i].enabled == 0)
                continue;
            L[i]=normalize(fL[i]);
            H[i]=normalize(fH[i]);

            int directCheck = lights[i].directional;
            float d = distance (L[i], E);
            float aten = 1;
            if (directCheck==0)
            {
                aten = 1 / max ( 1, lights[i].attenuation[0] + lights[i].attenuation[1] * d + lights[i].attenuation[2] * pow(d,2) );
            }
            float foco = 1.0;
            if (lights[i].directional==0 && lights[i].spotCutoff < 180.0)
            {
                float maxCosAngle = max(dot(-L[i], lights[i].spotDirectionEye), 0.0);
                foco = maxCosAngle >= lights[i].spotCosCutoff ? foco = pow(maxCosAngle,lights[i].spotExponent) : 0.0;
            }

            float F = one_over_pi*Frensel(E,N,lambda);
            float D = Beckmann (H[i],N,m) / max (0.001f, dot(L[i],N));
            float G = Gatten(L[i],E,H[i],N) / max (0.001f, dot(L[i],N));
            float CT = F*D*G;
            vec4 diffuseS = max(0.0f,dot(N,L[i]))*lights[i].diffuse*diffuse;
            vec4 specularS = max(0.0f,CT)*lights[i].specular*specular;
            vec4 ambientS = lights[i].ambient * ambient;
            color+=aten*foco*(ambientS+diffuseS+specularS);
        }
        fragColor=color;
}