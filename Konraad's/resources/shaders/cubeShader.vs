#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform int flipNormal;

void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.0);

	// flip normals if needed
	vec3 normal = (flipNormal == 1) ? -aNormal : aNormal;
	// the normal matrix (good practice would be to calculate it on cpu then send it to the shaders via an uniform)
	Normal = mat3(transpose(model)) * normal;

	FragPos = vec3(model * vec4(aPos, 1.0));
}