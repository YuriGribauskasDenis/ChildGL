#version 450

$GLMatrices
$Lights
$Material

in vec4 position;
in vec3 normal;

out vec3 fN;
out vec3 fH[lights.length()];
out vec3 fE;
out vec3 fL[lights.length()];

void main() {
  vec3 eN = normalize(normalMatrix * normal);
  vec3 eposition = vec3(modelviewMatrix * position);
  vec3 V = normalize(-eposition.xyz);
    for (int i = 0; i < lights.length(); i++) 
    {
        if (lights[i].enabled == 0)
            continue;
        int directCheck = lights[i].directional;
        fL[i]=normalize(vec3(lights[i].positionEye)-eposition);
        if (directCheck!=0)
        {
            fL[i] = normalize(vec3(lights[i].positionEye));
        }
        fH[i]=normalize(fL[i]+V);
    }
  fN = eN;
  fE = V;
  gl_Position = modelviewprojMatrix * position;
}