#version 450

$GLMatrices
$Lights
$Material

in vec4 position;
in vec3 normal;

//out vec4 color;

out vec3 pNormal;
out vec3 pView;
out vec3 pPos;
out vec3 pL[lights.length()];

void lightStore(vec3 pos)
{
    for (int i = 0; i < lights.length(); i++) 
    {
        if (lights[i].enabled == 0)
            continue;
        int directCheck = lights[i].directional;
        vec3 L = normalize(vec3(lights[i].positionEye) - pos);
        if (directCheck!=0)
        {
            L = normalize(vec3(lights[i].positionEye));
        }
    pL[i] = L;
    }
}

void main() {
  // Normal en el espacio de la cámara
  vec3 eN = normalize(normalMatrix * normal);
  // Vértice en el espacio de la cámara
  vec3 eposition = vec3(modelviewMatrix * position);
  // Vector vista (desde vértice a la cámara)
  vec3 V = normalize(-eposition.xyz);
  lightStore(eposition);
  pNormal = eN;
  pView = V;
  pPos = eposition;
  gl_Position = modelviewprojMatrix * position;
}
