#version 450

$GLMatrices
$Lights
$Material

in vec4 color;

out vec4 fragColor;

void main()
{
    fragColor = color;
}
