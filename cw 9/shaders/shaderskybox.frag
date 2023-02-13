#version 430 core

uniform samplerCube skybox;
uniform float exposition;

in vec3 texCoord;

out vec4 out_color;

in vec3 texCoord;

out vec4 out_color;

void main()
{
<<<<<<< Updated upstream
	out_color = texture(skybox,texCoord);
=======
	out_color = texture(skybox,texCoord)*exposition;
>>>>>>> Stashed changes
}