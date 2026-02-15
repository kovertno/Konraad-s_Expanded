#version 330 core
 
uniform mat4 projection;
uniform mat4 model;

layout (location = 0) in vec2 aPos;

void main() {
	gl_Position = projection * model * vec4(aPos, 0.0f, 1.0f);
}