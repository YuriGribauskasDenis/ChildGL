#version 450

$GLMatrices
$Lights
$Material

in vec4 position;
in vec3 normal;

out vec4 color;

// Suponiendo fuentes puntuales
vec4 iluminacion(vec3 pos, vec3 N, vec3 V) {
  // Componente emisiva del material
  vec4 color = emissive;
  for (int i = 0; i < lights.length(); i++) {
    // Si la fuente está apagada, no tenerla en cuenta
    if (lights[i].enabled == 0)
      continue;
    // Vector iluminación (desde vértice a la fuente)
    //if directional check
    int directCheck = lights[i].directional;
    //vec3 L = directCheck==0?normalize(vec3(lights[i].positionEye) - pos):normalize(vec3(-lights[i].spotDirectionEye));
    vec3 L = normalize(vec3(lights[i].positionEye) - pos);
    if (directCheck!=0)
    {
        L = normalize(vec3(lights[i].positionEye));
    }
    //My code
    float d = distance (vec3(lights[i].positionEye), pos);
    //float d = length (vec3(lights[i].positionEye) - pos);
    //check directional
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

void main() {
  // Normal en el espacio de la cámara
  vec3 eN = normalize(normalMatrix * normal);
  // Vértice en el espacio de la cámara
  vec3 eposition = vec3(modelviewMatrix * position);
  // Vector vista (desde vértice a la cámara)
  vec3 V = normalize(-eposition.xyz);
  // Cálculo de la iluminación
  color = iluminacion(eposition, eN, V);
  gl_Position = modelviewprojMatrix * position;
}
