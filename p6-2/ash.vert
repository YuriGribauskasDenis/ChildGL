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
out vec3 fTang;
out vec3 fBi;

void main() {
  vec3 eN = normalize(normalMatrix * normal);
  vec3 tangent;
  vec3 c1 = cross(eN, vec3(0.0f, 0.0f, 1.0f));
  vec3 c2 = cross(eN, vec3(0.0f, 1.0f, 0.0f));
  if( length(c1) > length(c2) )
  {
    tangent = normalize(c1);
  }
  else
  {
    tangent = normalize(c2);
  }
  vec3 biNorm = normalize(cross(tangent, eN));
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
  fBi = biNorm;
  fTang = tangent;
  fE = V;
  //fE = eposition;
  gl_Position = modelviewprojMatrix * position;
}