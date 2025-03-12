#version 330 core

out vec4 FragColor;

in vec3 ObjColor;
in vec3 FragNorm;  
in vec3 FragPos;  

uniform vec3 LightPos; 
uniform vec3 LightColor;
uniform vec3 ViewPos; 
uniform vec3 CamPos; 

void main()
{
    float Dist = distance(LightPos, FragPos);
    float da = 1.0f, db = 0.001f, dc = 0.000001f;

    // Ambient
    float AmbientStrength = 0.2f;
    vec3 Ambient = AmbientStrength * LightColor;
  	
    // Diffuse
    float DiffuseStrength = 0.1f;
    float DiffuseAlpha = 1.0f;
    vec3 Vec_L = normalize(LightPos - FragPos);
    vec3 Vec_N = normalize(FragNorm);
    float Diff = pow(max(dot(Vec_N, Vec_L), 0.0f), DiffuseAlpha);
    vec3 Diffuse = DiffuseStrength * Diff * LightColor;
    
    // Specular
    float SpecularStrength = 0.8f;
    float SpecularAlpha = 2.0f;
    vec3 Vec_V = normalize(CamPos - ViewPos);
    vec3 Vec_R = normalize(reflect(-Vec_L, Vec_N));  
    float Spec = pow(max(dot(Vec_V, Vec_R), 0.0f), SpecularAlpha);
    vec3 Specular = SpecularStrength * Spec * LightColor;  
    
    vec3 Color = ObjColor * ((1 / (da + db * Dist + dc * Dist * Dist)) * (Diffuse + Specular) + Ambient);
    FragColor = vec4(Color, 1.0);
}
