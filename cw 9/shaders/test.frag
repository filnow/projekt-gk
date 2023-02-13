#version 330 core
out vec4 FragColor;
  
in vec2 tc;

uniform sampler2D color;
uniform sampler2D highlight;

float rescale_z(float z){
	float n = 0.05;
	float f = 20.;
	return (2*n*f/(z*(n-f)+n+f))/f;
}
float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

vec4 blur()
{             
    vec2 tex_offset = 1.0 / textureSize(color, 0); // gets size of single texel
    vec3 result = texture(highlight, tc).rgb * weight[0]; // current fragment's contribution
    bool horizontal = true;
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(highlight, tc + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(highlight, tc - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(highlight, tc + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(highlight, tc - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    return vec4(result, 1.0);
}

void main()
{             
    //float depthValue = texture(depthMap, tc).r;
    //FragColor = vec4(vec3(rescale_z(depthValue)+0.5), 1.0);
    //FragColor = vec4(vec3((depthValue)+0.5), 1.0);
    
    FragColor = vec4(texture(highlight, tc).rgb+texture(color, tc).rgb,1);
    //FragColor = vec4( (texture(color, tc)-0.98)*50);
}  