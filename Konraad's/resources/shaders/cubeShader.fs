#version 330 core

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
}; 
uniform DirLight dirLight;

struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
#define NUM_P_LIGHT 4
uniform PointLight pointLight[NUM_P_LIGHT];

struct SpotLight {
	vec3 position;
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float cutOff;
	float outCutOff;
};
uniform SpotLight spotLight;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform Material material;

struct TextureMaterial {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};
uniform TextureMaterial textureMaterial;

uniform bool useTexture;
uniform bool isDebugColors;
uniform vec3 viewPos;

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

vec3 CalcDirLight(DirLight dirLight, vec3 normal, vec3 viewDir, bool useTexture);
vec3 CalcPointLight(PointLight pointLight, vec3 normal, vec3 viewDir, vec3 FragPos, bool useTexture);
vec3 CalcSpotLight(SpotLight spotLight, vec3 normal, vec3 viewDir, vec3 FragPos, bool useTexture);

void main() {
	// properties
	vec3 normal = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	// directional light
	vec3 result = CalcDirLight(dirLight, normal, viewDir, useTexture);
	// point light
	for (int i = 0; i < NUM_P_LIGHT; i++) {
		result += CalcPointLight(pointLight[0], normal, viewDir, FragPos, useTexture);
	}
	// spot light
	result += CalcSpotLight(spotLight, normal, viewDir, FragPos, useTexture);
	
	if (isDebugColors)
		FragColor = vec4(normalize(Normal), 1.0);
	else
		FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight dirLight, vec3 normal, vec3 viewDir, bool useTexture) {
	vec3 lightDir = normalize(-dirLight.direction); 
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	//float spec = pow(max(dot(reflectDir, viewDir), 0.0), 32);
	//combine
	if (useTexture) {
		float spec = pow(max(dot(reflectDir, viewDir), 0.0), textureMaterial.shininess);
		vec3 ambient = dirLight.ambient * vec3(texture(textureMaterial.diffuse, TexCoords));
		vec3 diffuse = dirLight.diffuse * diff * vec3(texture(textureMaterial.diffuse, TexCoords));
		vec3 specular = dirLight.specular * spec * vec3(texture(textureMaterial.specular, TexCoords));

		return (ambient + diffuse + specular);
	}
	else {
		float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
		vec3 ambient = dirLight.ambient * material.ambient;
		vec3 diffuse = dirLight.diffuse * material.diffuse * diff;
		vec3 specular = dirLight.specular * material.specular * spec;

		return (ambient + diffuse + specular);
	}
}

vec3 CalcPointLight(PointLight pointLight, vec3 normal, vec3 viewDir, vec3 FragPos, bool useTexture) {
	vec3 lightDir = normalize(pointLight.position - FragPos);
	// diffuse shading
	float diff = max(dot(lightDir, normal), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	//float spec = pow(max(dot(reflectDir, viewDir), 0.0), 32);
	// attenuation
	float distance = length(pointLight.position - FragPos);
	float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));
	// combine
	if (useTexture) {
		float spec = pow(max(dot(reflectDir, viewDir), 0.0), textureMaterial.shininess);
		vec3 ambient = pointLight.ambient * vec3(texture(textureMaterial.diffuse, TexCoords));
		vec3 diffuse = pointLight.diffuse * diff * vec3(texture(textureMaterial.diffuse, TexCoords));
		vec3 specular = pointLight.specular * spec * vec3(texture(textureMaterial.specular, TexCoords));
		
		ambient *= attenuation;
		diffuse *= attenuation;
		specular *= attenuation;

		return (ambient + diffuse + specular);
	}
	else {
		float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
		vec3 ambient = pointLight.ambient * material.ambient;
		vec3 diffuse = pointLight.diffuse * material.diffuse * diff;
		vec3 specular = pointLight.specular * material.specular * spec;

		ambient *= attenuation;
		diffuse *= attenuation;
		specular *= attenuation;

		return (ambient + diffuse + specular);
	}
}

vec3 CalcSpotLight(SpotLight spotLight, vec3 normal, vec3 viewDir, vec3 FragPos, bool useTexture) {
	vec3 lightDir = normalize(spotLight.position - FragPos);
	// diffuse shading
	float diff = max(dot(lightDir, normal), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	//float spec = pow(max(dot(reflectDir, viewDir), 0.0), 32);
	
	float theta = dot(lightDir, normalize(-spotLight.direction));
	float epsilon = spotLight.cutOff - spotLight.outCutOff;

	float intensity = clamp((theta - spotLight.outCutOff) / epsilon, 0.0, 1.0);

	// we want the specular light to follow the camera view
	float alignment = dot(lightDir, -spotLight.direction);
	// Use alignment to create a "hotspot" - high value when looking directly at fragment

	// combine
	if (useTexture) {
		float spec = pow(alignment, textureMaterial.shininess);
		vec3 ambient = spotLight.ambient * vec3(texture(textureMaterial.diffuse, TexCoords));
		vec3 diffuse = spotLight.diffuse * diff * vec3(texture(textureMaterial.diffuse, TexCoords));
		vec3 specular = spotLight.specular * spec * vec3(texture(textureMaterial.specular, TexCoords));

		ambient *= intensity;
		diffuse *= intensity;
		specular *= intensity;

		return (ambient + diffuse + specular);
	}
	else {
		float spec = pow(alignment, material.shininess);
		vec3 ambient = spotLight.ambient * material.ambient;
		vec3 diffuse = spotLight.diffuse * material.diffuse * diff;
		vec3 specular = spotLight.specular * material.specular * spec;

		ambient *= intensity;
		diffuse *= intensity;
		specular *= intensity;

		return (ambient + diffuse + specular);
	}
}