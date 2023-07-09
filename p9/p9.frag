#version 420 core

$Lights

layout (binding=$TEXDIFF) uniform sampler2D colores;
layout (binding=$TEXNORM) uniform sampler2D normales;
layout (binding=$TEXSPEC) uniform sampler2D brillos;
// Descomenta este sampler cuando estés preparado para usarlo
layout (binding=$TEXHEIGHT) uniform sampler2D alturas;
uniform float depth;

uniform bool useParallax;

in VS_OUT
{
    vec3 fPosition;
    vec2 texCoord;
    vec3 tangentLightPos;
    vec3 tangentViewPos;
    vec3 tangentFPos;
} fs_in;

out vec4 fragColor;

vec2 ParallaxMapping(vec2 texCoord, vec3 viewDir)
{ 
    float height =  texture(alturas, texCoord).r;     
    return texCoord - viewDir.xy * (height * depth); 
}
vec2 ParallaxMapping2(vec2 texCoords, vec3 viewDir)
{ 
    // number of depth layers
    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * depth; 
    vec2 deltaTexCoords = P / numLayers;
  
    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(alturas, currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(alturas, currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    return currentTexCoords;
}
vec2 ParallaxMapping3(vec2 texCoords, vec3 viewDir)
{ 
    // number of depth layers
    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * depth; 
    vec2 deltaTexCoords = P / numLayers;
  
    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(alturas, currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(alturas, currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(alturas, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

void main()
{
	if (useParallax)
	{
		vec3 viewDir = normalize(fs_in.tangentViewPos - fs_in.tangentFPos);
		vec2 texCoords = ParallaxMapping2(fs_in.texCoord,  viewDir);       
		if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
			discard;

		vec3 normal = texture(normales, texCoords).rgb;
		normal = normalize(normal * 2.0 - 1.0);

		vec3 c = texture(colores, texCoords).xyz;
		vec3 ambient = 0.1 * c;

		vec3 lightDir = normalize(fs_in.tangentLightPos - fs_in.tangentFPos);
		float diff = max(dot(lightDir, normal), 0.0);
		vec3 diffuse = diff * c;

		vec3 reflectDir = reflect(-lightDir, normal);
		vec3 halfwayDir = normalize(lightDir + viewDir);  
		float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

		vec3 specular = texture(brillos, texCoords).rgb * spec;
		fragColor = vec4(ambient + diffuse + specular, 1.0);
	}
	else
	{
		//vec4 c = texture(colores, fs_in.texCoord);
		//vec4 n = texture(normales, fs_in.texCoord);
		//vec4 b = texture(brillos, fs_in.texCoord);

		vec3 normal = texture(normales, fs_in.texCoord).rgb;
		normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
   
		// get diffuse color
		vec3 c = texture(colores, fs_in.texCoord).rgb;
		// ambient
		vec3 ambient = 0.1 * c;
		// diffuse
		vec3 lightDir = normalize(fs_in.tangentLightPos - fs_in.tangentFPos);
		float diff = max(dot(lightDir, normal), 0.0);
		vec3 diffuse = diff * c;
		// specular
		vec3 viewDir = normalize(fs_in.tangentViewPos - fs_in.tangentFPos);
		vec3 reflectDir = reflect(-lightDir, normal);
		vec3 halfwayDir = normalize(lightDir + viewDir);  
		float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

		vec3 specular = texture(brillos, fs_in.texCoord).rgb * spec;
		//fragColor = vec4(ambient + diffuse + specular, 1.0);

		if (lights[0].positionEye.y > 2.0)
		{
			//fragColor = c * n * b;
			fragColor = vec4(ambient + diffuse + specular, 1.0);
		}
		else
			//fragColor = c;
			fragColor = vec4(diffuse, 1.0);
	}
}
