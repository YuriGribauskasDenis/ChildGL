#version 450

$GLMatrices
$Lights
$Material

layout (location = 25) uniform float nu;
layout (location = 26) uniform float nv;
layout (location = 27) uniform float spec_coeff;
layout (location = 28) uniform float diff_coeff;

in vec3 fN;
in vec3 fH[lights.length()];
in vec3 fE;
in vec3 fL[lights.length()];
in vec3 fTang;
in vec3 fBi;

out vec4 fragColor;

vec4 pow5(vec4 f)
{
    return f*f*f*f*f;
}

vec4 Frensel(vec4 refl, vec4 kh)
{
    return refl+(vec4(1.0f)-refl)*pow5(vec4(1.0f)-kh);
}

vec4 mclamp(vec4 v)
{
    return max(v, vec4(0.01f));
}

vec4 ash_specular(float nu, float nv, vec3 n, vec3 h, vec3 light, vec3 eye, vec3 u, vec3 v, vec4 spec)
{
    vec3 k = light;
    vec4 nDotH = mclamp(vec4(dot(h,n)));
    vec4 nDotK = mclamp(vec4(dot(k,n)));
    vec4 nDotL = mclamp(vec4(dot(light,n)));
    vec4 nDotE = mclamp(vec4(dot(eye,n)));
    vec4 kDotH = mclamp(vec4(dot(h,n)));
    vec4 hDotU = vec4(dot(h,u));
    vec4 hDotV = vec4(dot(h,v));

    vec4 exponent = (vec4(nu)*hDotU*hDotU + vec4(nv)*hDotV*hDotV) / (vec4(1.0f) - nDotH*nDotH);
    vec4 Gatten = pow (nDotH, exponent) / (nDotK*max(nDotL,nDotE));

    return Gatten*Frensel(spec, kDotH);
}

vec4 ash_diffuse(vec3 normal, vec3 light, vec3 viewer, vec4 spec, vec4 diffuse)
{
    vec4 scale = diffuse*(vec4(1.0f)-spec);
    vec4 v = vec4(1.0f) - pow5(vec4(1.0f-max(dot(normal,light),0.0f)/2.0f));
    vec4 l = vec4(1.0f) - pow5(vec4(1.0f-max(dot(normal,viewer),0.0f)/2.0f));
    return scale*v*l;
}

void main()
{
        
        vec3 n = normalize (fN);
        vec3 u = normalize (fTang);
        vec3 v = normalize (fBi);
        vec3 k2 = normalize (fE);
        vec3 k1[lights.length()];
        vec3 h[lights.length()];

        vec4 color = emissive;
        
        for (int i = 0; i < lights.length(); i++)
        {
            if (lights[i].enabled == 0)
                continue;
            k1[i]=normalize(fL[i]);
            h[i]=normalize(fH[i]);
            //k1[i]=normalize(vec3(lights[i].positionEye)-fE);
            //h[i]=normalize(k1[i]+k2);

            int directCheck = lights[i].directional;
            float d = distance (k1[i], k2);
            float aten = 1;
            if (directCheck==0)
            {
                aten = 1 / max ( 1, lights[i].attenuation[0] + lights[i].attenuation[1] * d + lights[i].attenuation[2] * pow(d,2) );
            }
            float foco = 1.0;
            if (lights[i].directional==0 && lights[i].spotCutoff < 180.0)
            {
                float maxCosAngle = max(dot(-k1[i], lights[i].spotDirectionEye), 0.0);
                foco = maxCosAngle >= lights[i].spotCosCutoff ? foco = pow(maxCosAngle,lights[i].spotExponent) : 0.0;
            }

            vec4 Rs = lights[i].specular;
            vec4 Rd = lights[i].diffuse;
            vec4 Ra = lights[i].ambient;
            vec4 Ms = specular;
            vec4 Md = diffuse;
            vec4 Ma = ambient;
            
            color += aten*foco*(
            Ma * Ra
            + Ms * spec_coeff * ash_specular(nu, nv, n, h[i], k1[i], k2, u, v, Rs)
            + Md * diff_coeff * ash_diffuse(n, k1[i], k2, Rs, Rd));
        }
        
        fragColor=color;
}