#version 330 core
struct Material {
    sampler2D texture_diffuse;
    sampler2D texture_specular;
    sampler2D emissive;
    float shininess;
    float emissiveMultiplier;
}; 
  
uniform Material material;

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
uniform DirLight dirLight;

uniform vec3 viewPos;

in vec3 FragPos;
in vec3 Normal;  
in vec2 TexCoords;
out vec4 FragColor;

//uniform sampler2D texture_diffuse1;
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);  
void main()
{

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // define an output color value
    vec3 output = vec3(0.0);
    // add the directional light's contribution to the output
    output += CalcDirLight(dirLight,norm,viewDir);

    //FragColor = texture(texture_diffuse1, TexCoords);
    FragColor = vec4(output, 1.0);

}
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  ;
    vec3 diffuse  = light.diffuse  * diff ;
    vec3 specular = light.specular * spec;
    return (ambient + diffuse + specular);
}  