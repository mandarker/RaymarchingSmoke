#shader vertex
#version 330 core

layout(location = 0) in vec4 l_position;
layout(location = 1) in vec3 l_normal;
layout(location = 2) in vec2 l_uv;
layout(location = 3) in vec3 l_tangent;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
	vec3 Tangent;
    vec4 FragPosLightSpace;
	vec2 UV;
} vs_out;

uniform mat4 u_Proj;
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_LightSpaceMatrix;

void main()
{

	gl_Position = u_Proj * u_View * u_Model * l_position;
    vs_out.FragPos = vec3(u_Model * l_position);
	vs_out.Normal = l_normal;
	vs_out.Tangent = l_tangent;
    vs_out.FragPosLightSpace = u_LightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
	vs_out.UV = l_uv;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
	vec3 Tangent;
    vec4 FragPosLightSpace;
	vec2 UV;
} fs_in;

uniform sampler2D u_ShadowMap;
uniform sampler2D u_Texture;

uniform vec3 u_Light;
uniform vec3 u_Cam;
uniform vec3 u_Color;
uniform vec3 u_LightColor;
uniform vec3 u_SpecularColor;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(u_Light - fs_in.FragPos);
    
    float depth = projCoords.z;
    float bias = max(0.00275 * (1.0 - dot(normal, lightDir)), 0.000275);

	float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
    
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += depth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
	shadow /= 9.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

vec3 CalcBumpedNormal()
{
    vec3 n = normalize(fs_in.Normal);
    vec3 t = normalize(fs_in.Tangent);
    vec3 b = cross(t, n);

    vec3 bumped = texture(u_Texture, fs_in.UV).rgb;
    bumped = bumped * 2 - vec3(1.0);

    vec3 newNormal;
    mat3 TBN = mat3(t, b, n);
    newNormal = TBN * bumped;
    newNormal = normalize(newNormal);
    return newNormal;
}

void main()
{
	vec3 normalized = CalcBumpedNormal();
	vec3 normalizedLight = normalize(u_Light);
	vec3 viewDir = normalize(u_Cam - fs_in.FragPos);
    vec3 reflectDir = reflect(-normalizedLight, normalized);

	vec3 diff = clamp(dot(normalizedLight, normalized), 0, 1) * u_LightColor;

    vec3 halfwayDir = normalize(normalizedLight + viewDir);  
    vec3 spec = pow(max(dot(normalized, halfwayDir), 0.0), 3.0) * u_SpecularColor;

	vec3 shadow = vec3(1 - ShadowCalculation(fs_in.FragPosLightSpace)); 
	
	color = vec4((u_Color * (diff + spec) * shadow), 1);
};