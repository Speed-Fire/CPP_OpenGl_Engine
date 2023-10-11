#version 440

#define _MAX_POINT_LIGHTS_ 10
#define _MAX_SPOT_LIGHTS_  10

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
	sampler2D diffuseTex;
	sampler2D specularTex;
};

//Light structs

struct DirLight
{
	float intensity;
	vec3 color;

	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	float intensity;
	vec3 color;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight
{
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;

	float intensity;
	vec3 color;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 vs_position;
in vec3 vs_color;
in vec2 vs_texcoord;
in vec3 vs_normal;

out vec4 fs_color;

uniform Material material;
uniform PointLight pointLight;
uniform vec3 camPosition;


uniform DirLight dirLight;
uniform PointLight pointLights[_MAX_POINT_LIGHTS_];
uniform SpotLight spotLights[_MAX_SPOT_LIGHTS_];

float near = 0.1f;
float far = 50.0f;

//vec4 Negative()
//{
//	vec4 final_color;
//
//	vec4 textureColor0=texture2D(material.diffuseTex, vs_texcoord);
//
//	final_color = texture(material.diffuseTex, vs_texcoord) *
//				vec4(1.f - textureColor0.r, 1.f - textureColor0.g, 1.f - textureColor0.b, 1.f);
//
//	return final_color;
//}
//
//vec4 Green()
//{
//vec4 final_color;
//
//	final_color = texture(material.diffuseTex, vs_texcoord) * vec4(-0.7f, 2.f, -0.3f, 1.f);
//
//	return final_color;
//}

//vec4 vertColorToTex()
//{
//	vec4 final_color;
//
//	final_color = texture(material.diffuseTex, vs_texcoord) * vec4(vs_color, 1.f);
//
//	return final_color;
//}
//
//vec4 vertColor()
//{
//	vec4 final_color;
//
//	final_color = vec4(vs_color, 1.f);
//
//	return final_color;
//}

vec4 Normal()
{
	vec4 final_color;

	final_color = texture(material.diffuseTex, vs_texcoord);

	return final_color;
}

//LIGHTING

//vec3 calcAmbient(Material mtrl, PointLight lightPnt)
//{
//	return mtrl.ambient * lightPnt.color * lightPnt.ambient;
//}
//
//vec3 calcDiffuse(Material mtrl, vec3 vs_position, PointLight lightPnt, vec3 vs_normal)
//{
//	vec3 posToLightDirVec = normalize(lightPnt.position - vs_position);
//	float diffuse = clamp(dot(posToLightDirVec, normalize(vs_normal)), 0, 1);
//	vec3 diffuseFinal = mtrl.diffuse * diffuse * lightPnt.diffuse;
//
//	return diffuseFinal;
//}
//
//vec3 calcSpecular(Material mtrl, vec3 vs_position, PointLight lightPnt, vec3 vs_normal, vec3 cameraPos)
//{
//	vec3 lightToPosDirVec = normalize(vs_position - lightPnt.position);
//	vec3 reflectDirVec = normalize(reflect(lightToPosDirVec, normalize(vs_normal)));
//	vec3 posToViewDirVec = normalize(cameraPos - vs_position);
//	float specularConstant = pow(max(dot(posToViewDirVec, reflectDirVec), 0), mtrl.shininess);
//	vec3 specularFinal = mtrl.specular * specularConstant * lightPnt.color * lightPnt.specular * texture(mtrl.specularTex, vs_texcoord).rgb;
//	//vec3 specularFinal = mtrl.specular * specularConstant * lightPnt.color * lightPnt.specular;
//
//	return specularFinal;
//}
//
//float calcAttenuation(PointLight lightPnt, vec3 vs_position)
//{
//	float StartDistance = length(lightPnt.position - vs_position);
//	float attenuation = lightPnt.constant / 
//		(1.f + lightPnt.linear * StartDistance + lightPnt.quadratic * (StartDistance * StartDistance));
//	return attenuation;
//}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 ambient, vec3 diffuse, vec3 specular)
{
	vec3 lightDir = normalize(-light.direction);
	
	//diffuse
	float diff = clamp(dot(normal, lightDir), 0, 1);

	//specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float specConst = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

//	vec3 ambient_ = light.ambient * material.ambient * light.color * texture(material.diffuseTex, vs_texcoord).rgb;
//	vec3 diffuse_ = light.diffuse * material.diffuse * diff * texture(material.diffuseTex, vs_texcoord).rgb;
//	vec3 specular_ = light.specular * material.specular * specConst * texture(material.specularTex, vs_texcoord).rgb;

	vec3 ambient_ = ambient * light.ambient * light.color;
	vec3 diffuse_ = diffuse * light.diffuse * diff;
	vec3 specular_ = specular * light.specular * specConst;

	return (ambient_ + diffuse_ + specular_) * light.intensity;
}


vec3 calcPointLight(PointLight light, vec3 position, vec3 normal, vec3 viewDir, vec3 ambient, vec3 diffuse, vec3 specular)
{
	vec3 lightDir = normalize(light.position - position);

	//diffuse
	float diff = clamp(dot(normal, lightDir), 0, 1);

	//specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float specConst = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

	//attenuation
	float dist = length(light.position - position);
	float attenuation = light.constant / (1.f + light.linear * dist + light.quadratic * (dist * dist));


	vec3 ambient_ = ambient * light.ambient * light.color;
	vec3 diffuse_ = diffuse * light.diffuse * diff;
	vec3 specular_ = specular * light.specular * specConst;
	ambient_ *= attenuation;
	diffuse_ *= attenuation;
	specular_ *= attenuation;

	return (ambient_ + diffuse_ + specular_) * light.intensity;
}

vec3 calcSpotLight(SpotLight light, vec3 position, vec3 normal, vec3 viewDir, vec3 ambient, vec3 diffuse, vec3 specular)
{
	vec3 lightDir = normalize(light.position - position);

	float theta = dot(lightDir, normalize(-light.direction));

	if(theta > cos(radians(light.outerCutOff)))
	{
		float epsilon = cos(radians(light.cutOff)) - cos(radians(light.outerCutOff));
		float intensity = clamp((theta - cos(radians(light.outerCutOff))) / epsilon, 0.f, 1.f);
		
		//diffuse
		float diff = clamp(dot(normal, lightDir), 0, 1);

		//specular
		vec3 reflectDir = reflect(-lightDir, normal);
		float specConst = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

		//attenuation
		//float dist = length(light.position - position);
		//float attenuation = light.constant / (1.f + light.linear * dist + light.quadratic * (dist * dist));


		vec3 ambient_ = ambient * light.ambient * light.color;
		vec3 diffuse_ = diffuse * light.diffuse * diff;
		vec3 specular_ = specular * light.specular * specConst;
//		ambient_ *= attenuation;
//		diffuse_ *= attenuation;
//		specular_ *= attenuation;

		diffuse_ *= intensity;
		specular_ *= intensity;

		return (ambient_ + diffuse_ + specular_) * light.intensity;
	}
	else
	{
		return light.ambient * ambient;
	}
}

float LinearizeDepth(float depth)
{
	float z = depth * 2.0f - 1.0f;
	return (2.0f * near * far) / (far + near - z*(far - near));
}

void main()
{
	//ATTENAUTION
//	float attenuation = calcAttenuation(pointLight, vs_position);
//	vec4 ambientFinal = vec4(calcAmbient(material, pointLight) * attenuation, 1.f);
//	vec4 diffuseFinal = vec4(calcDiffuse(material, vs_position, pointLight, vs_normal) * attenuation, 1.f);
//	vec4 specularFinal = vec4(calcSpecular(material, vs_position, pointLight, vs_normal, camPosition) * attenuation, 1.f);

	vec3 viewDir = normalize(camPosition - vs_position);
	vec3 norm = normalize(vs_normal);

	vec4 difTexColor = texture(material.diffuseTex, vs_texcoord);
	vec4 specTexColor = texture(material.specularTex, vs_texcoord);
	if(difTexColor.a < 0.5f)
	{
		discard;
	}
	if(specTexColor.a < 0.5f)
	{
		discard;
	}

	vec3 ambient = material.ambient * difTexColor.rgb;
	vec3 diffuse = material.diffuse * difTexColor.rgb;
	vec3 specular = material.specular * specTexColor.rgb;

	vec3 result = calcDirLight(dirLight, norm, viewDir, ambient, diffuse, specular);

	for(int i = 0; i < _MAX_POINT_LIGHTS_; i++)
	{
		result += calcPointLight(pointLights[i], vs_position, norm, viewDir, ambient, diffuse, specular); 
	}

	for(int i = 0; i < _MAX_SPOT_LIGHTS_; i++)
	{
		result += calcSpotLight(spotLights[i], vs_position, norm, viewDir, ambient, diffuse, specular); 
	}

	

	//fs_color = Normal() * (ambientFinal + diffuseFinal + specularFinal);

	//Visualize the depth buffer
	float depth = LinearizeDepth(gl_FragCoord.z) / far;
	//result *= vec3(depth);

	//FINAL
	fs_color = vec4(result, 1.0);
}