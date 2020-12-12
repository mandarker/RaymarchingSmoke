#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 u_Proj;
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_LightSpaceMatrix;

void main()
{
	gl_Position = u_Proj * u_View * u_Model * position;
    vs_out.FragPos = vec3(u_Model * position);
    vs_out.FragPosLightSpace = u_LightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
	vs_out.Normal = normal;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec4 FragPosLightSpace;
} fs_in;


uniform sampler2D shadowMap;

uniform vec3 u_Light;
uniform vec3 u_Cam;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(u_Light - fs_in.FragPos);
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.00005);
    
	float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{
	//vec4 texColor = texture(u_Texture, v_TexCoord);
	//vec3 normalLight = normalize(light);
	vec3 normalized = normalize(fs_in.Normal);
	// 1 - ShadowCalculation(fs_in.FragPosLightSpace);//
	vec3 shadow = vec3(1 - ShadowCalculation(fs_in.FragPosLightSpace)); 
	vec3 diffuse2 = vec3((dot(normalize(u_Light), normalized) + 1) / 2);

	color = vec4(diffuse2 * shadow, 1);
};