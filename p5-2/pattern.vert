#version 450 core

$GLMatrices

//layout location?
uniform int tile;

in vec3 position;
in vec2 texCoord;
in vec3 color;

out vec2 texCoordFrag;
out vec3 colorFrag;

void main() {
  texCoordFrag = texCoord*tile;
  colorFrag = color;
  gl_Position = modelviewprojMatrix * vec4(position, 1.0);
}
