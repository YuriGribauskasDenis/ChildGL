#version 450

$GLMatrices
$Lights
$Material

in vec4 color;

in vec3 pNormal;
in vec3 pView;
in vec3 pPos;
in vec3 pL[lights.length()];

out vec4 fragColor;

vec4 iluminacion(vec3 pos, vec3 N, vec3 V) {
  // Componente emisiva del material
  vec4 color = emissive;
  for (int i = 0; i < lights.length(); i++)
  {
    if (lights[i].enabled == 0)
      continue;
    vec3 L = normalize(pL[i]);
    int directCheck = lights[i].directional;
    float d = distance (L, pos);
    //float aten = directCheck!=0?1:1 / max (1, lights[i].attenuation[0] + lights[i].attenuation[1] * d + lights[i].attenuation[2] * pow(d,2) );
    float aten = 1;
    if (directCheck==0)
    {
        aten = 1 / max ( 1, lights[i].attenuation[0] + lights[i].attenuation[1] * d + lights[i].attenuation[2] * pow(d,2) );
    }
    vec3 L_nagative = L*(-1.0f);
    //float foco = pow(max(dot(L_nagative, lights[i].spotDirectionEye), 0.0), lights[i].spotExponent);
    float foco = 1.0;
    if (lights[i].directional==0 && lights[i].spotCutoff < 180.0)
    {
        float maxCosAngle = max(dot(L_nagative, lights[i].spotDirectionEye), 0.0);
        foco = maxCosAngle >= lights[i].spotCosCutoff ? foco = pow(maxCosAngle,lights[i].spotExponent) : 0.0;
    }
    //end of my code
    // Multiplicador de la componente difusa
    float diffuseMult = max(dot(N, L), 0.0);
    float specularMult = 0.0;
    if (diffuseMult > 0.0) {
      // Multiplicador de la componente especular
      vec3 R = reflect(-L, N);
      specularMult = max(0.0, dot(R, V));
      specularMult = pow(specularMult, shininess);
    }

    color += aten * foco * (lights[i].ambient * ambient +
             lights[i].diffuse * diffuse * diffuseMult +
             lights[i].specular * specular * specularMult);
  }

  return color;
}

void main()
{
        vec3 phongNormal = normalize (pNormal);
        vec3 phongView = (pView);
        vec3 phongPos =  (pPos);
        fragColor = iluminacion(phongPos, phongNormal, phongView);
}
