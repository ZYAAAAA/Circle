#version 330 core
//layout( location = 0 ) out vec4 FragColor;

out vec4 FragColor;
uniform vec3 objectColor;
uniform vec3 lightColor;
in vec3 FragPos;
in vec3 Normal;
uniform vec3 lightdirection;

void main()
{
//       float ambientStrength = 0.1;
//       vec3 ambient = ambientStrength * lightColor;

//       vec3 norm = normalize(Normal);
//       vec3 lightDir = normalize(-lightdirection);
//       float diff = max(dot(norm, lightDir), 0.0);
//       vec3 diffuse = diff * lightColor;

//       vec3 result = (ambient + diffuse) * objectColor;

       //FragColor = vec4(result, 1.0);
        FragColor = vec4(objectColor,1.0);
}
