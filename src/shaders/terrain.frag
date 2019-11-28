#version 430

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    int shininess;
	vec3 textureScale;

	sampler2D diffuseTex;

	bool hasDiffuseTex;

	float maxY; //the upper bound of this material's layer in relation to the height of the mesh (the range is 0-1)
};

struct DirectionalLight
{
    vec3 direction;
    vec3 color;
};

in vec3 TexCoords;
in vec3 Normal;
in vec3 WorldPos;
in vec3 CameraPos;
in vec3 LightSpacePos;

const int MAX_MATERIALS = 14;

uniform sampler2DShadow shadowMap;
uniform int shadowsEnabled;
uniform Material materials[MAX_MATERIALS];
uniform sampler2DArray diffuseTex;
uniform int materialCount; //the actual number of materials in the array
uniform DirectionalLight light;
uniform int poissonSpread;
uniform float shadowBias;

uniform float minY; //the minimum world-space y-coordinate in the mesh
uniform float maxY; //the maximum world-space y-coordinate in the mesh

out vec4 fragColor;

//http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/
vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

//http://stackoverflow.com/questions/22419682/glsl-sampler2dshadow-and-shadow2d-clarification
//http://stackoverflow.com/questions/20877938/glsl-sampler2dshadow-deprecated-past-version-120-what-to-use
float calculateVisibility()
{
    //transform to [0,1] range
    vec3 shadowCoords = LightSpacePos * 0.5 + 0.5;

    float visibility = 0.0;

    for (int i = 0; i < 4; ++i)
    {
        visibility += texture(shadowMap, shadowCoords + vec3(poissonDisk[i] / poissonSpread, -shadowBias), 0.0);
    }

    visibility /= 4.0;

    return visibility;
}

void main()
{
    //calculate shadow
    float visibility = 1.0;
    if (shadowsEnabled == 1)
    {
        visibility = calculateVisibility();
    }

	//calculate the y-position of this fragment in relation to the height of the mesh (in the range 0-1)
	float y = (WorldPos.y - minY) / (maxY - minY);

	//calculate the index into the materials array 
    int index = 0;
    for (int i = 0; i < materialCount; ++i)
    {
        index += int(y > materials[i].maxY);
    }

    //calculate the ambient color
    vec3 ambient = light.color.rgb * materials[index].ambient;
  	
    //calculate the diffuse color
    vec3 normal = normalize(Normal);
    float diffuseCoef = max(dot(normal, -light.direction), 0.0);
    vec3 diffuse = light.color.rgb * diffuseCoef * materials[index].diffuse;
	diffuse *= visibility;
	
	//https://gamedevelopment.tutsplus.com/articles/use-tri-planar-texture-mapping-for-better-terrain--gamedev-13821
    
	vec3 texCoords = TexCoords / materials[index].textureScale;

	vec3 xaxis = vec3(texture(diffuseTex, vec3(texCoords.yz, index)));
    vec3 yaxis = vec3(texture(diffuseTex, vec3(texCoords.xz, index)));
    vec3 zaxis = vec3(texture(diffuseTex, vec3(texCoords.xy, index)));

	//blend the results of the 3 planar projections for the current texture
	vec3 blending = abs(Normal);
    blending = normalize(max(blending, 0.00001));
    float b = (blending.x + blending.y + blending.z);
    blending /= vec3(b, b, b);
	vec3 texColor = xaxis * blending.x + yaxis * blending.y + zaxis * blending.z;

	//only multiply ambient and diffuse color with texture color if the material has a texture
	ambient += float(materials[index].hasDiffuseTex) * ((texColor * ambient) - ambient);
	diffuse += float(materials[index].hasDiffuseTex) * ((texColor * diffuse) - diffuse);

    //calculate the specular color
    vec3 cameraDir = normalize(CameraPos - WorldPos);
    vec3 reflectDir = reflect(light.direction, normal);  
    float specularCoef = pow(max(dot(cameraDir, reflectDir), 0.0), materials[index].shininess);
    vec3 specular = light.color.rgb * (specularCoef * materials[index].specular) * visibility; 

    fragColor = vec4(ambient + diffuse + specular, 1.0f);
}
