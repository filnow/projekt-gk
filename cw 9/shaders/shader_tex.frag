#version 430 core

float AMBIENT = 0.1;

uniform vec3 lightPos;
uniform sampler2D colorTexture;

in vec3 vecNormal;
in vec3 worldPos;
in vec2 TexCoord;


out vec4 outColor;
void main()
{
	vec3 lightDir = normalize(lightPos-worldPos);
	vec3 normal = normalize(vecNormal);
	float diffuse=max(0,dot(normal,lightDir));
	vec4 textureColor = texture2D(colorTexture, TexCoord);

	outColor = vec4(texture2D(colorTexture, TexCoord).rgb * diffuse, 1.0);
}
