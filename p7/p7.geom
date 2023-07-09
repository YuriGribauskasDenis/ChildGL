#version 450

$GLMatrices

layout (triangles) in;
layout (triangle_strip, max_vertices = 6) out;

uniform float cylinderRadius;
uniform float cylinderHeight;
uniform vec3 cylinderPos;

uniform bool quadrFlag;
uniform float dist;

uniform bool boxFlag;
uniform float box_X;
uniform float box_Z;
uniform float box_Y;
uniform vec3 boxPos;

uniform mat3 boxMatPass;
uniform mat3 cylMatPass;

in vec4 color[3];
out vec4 fragColor;

int isVertexInsideCyl(vec3 q)
{
	vec3 inv = transpose(cylMatPass) * (q - cylinderPos);
	if (0.0<=inv.y&&inv.y<=cylinderHeight&&(pow(inv.x,2)+pow(inv.z,2)<=pow(cylinderRadius,2)))
		return 1;
	return 0;
}

int isVertexInsideBox(vec3 q)
{
	vec3 inv = transpose(boxMatPass) * (q - boxPos);
	if (abs(inv.x) < box_X * 0.5 && inv.y > 0 && inv.y < box_Y && abs(inv.z) < box_Z * 0.5)
		return 1;
	return 0;
}

int isVertexInside(vec3 q)
{
	if (!boxFlag) return isVertexInsideCyl(q);
	if (isVertexInsideCyl(q)==1||isVertexInsideBox(q)==1) return 1;
	return 0;
}

void main()
{
	int vertexCount = 0;
	for (int i = 0; i < gl_in.length(); i++)
	{
		vertexCount += isVertexInside(gl_in[i].gl_Position.xyz);
	}
	mat4 projviewMatrix = projMatrix * viewMatrix;
    if (vertexCount==3&&quadrFlag)
    {
		vec3 CameraRight = vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
		vec3 CameraUp = vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);
		for (int i = 0; i < gl_in.length(); i++)
		{
			vec3 Pos = gl_in[i].gl_Position.xyz;

			Pos -= (CameraRight * (dist/2));
			gl_Position = projviewMatrix * vec4(Pos, 1.0);
			fragColor = color[i]*vec4(1.0f,0.0f,0.0f,1.0f);
			EmitVertex();

			Pos += CameraUp*dist;
			gl_Position = projviewMatrix * vec4(Pos, 1.0);
			fragColor = color[i]*vec4(0.0f,1.0f,0.0f,1.0f);
			EmitVertex();

			Pos -= CameraUp*dist;
			Pos += (CameraRight * dist);
			gl_Position = projviewMatrix * vec4(Pos, 1.0);
			fragColor = color[i]*vec4(1.0f,0.0f,1.0f,1.0f);
			EmitVertex();

			Pos += CameraUp*dist;
			gl_Position = projviewMatrix * vec4(Pos, 1.0);
			fragColor = color[i]*vec4(0.0f,0.0f,1.0f,1.0f);
			EmitVertex();

			EndPrimitive();
		}
	}
	if (0<=vertexCount&&vertexCount<3)
	{
		for (int i = 0; i < gl_in.length(); i++)
		{
			vec4 colorMask = vec4(1.0f);
			gl_Position = projviewMatrix * gl_in[i].gl_Position;
			if (0<vertexCount&&vertexCount<3)
			{
				colorMask.xyz = vec3(0.0f);
				colorMask[2-vertexCount]=1.0f;
			}
			fragColor = color[i]*colorMask;
			EmitVertex();
		}
		EndPrimitive();
	}
}