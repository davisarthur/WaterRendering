#version 330 core
out vec4 FragColor;
  
in vec3 vertexPosition;
in vec3 normal;
in vec4 color;

uniform vec3 eye;
uniform samplerCube skybox;

void main() { 
    float nAir = 1.0;
    float nWater = 1.33;
    vec3 incidentRay = normalize(vec3(vertexPosition - eye));
    vec3 reflectedRay = reflect(incidentRay, normal);
    vec3 transmittedRay = refract(incidentRay, normal, nAir / nWater);
    float thetaAir = acos(abs(dot(incidentRay, normal))); 
    float thetaWater = asin(nAir / nWater * sin(thetaAir));
    float r1 = sin(thetaWater - thetaAir) / sin(thetaWater + thetaAir);
    float r2 = tan(thetaWater - thetaAir) / tan(thetaWater + thetaAir);
    float reflectivity = 0.5 * (r1 * r1 + r2 * r2);
    vec3 transmittedColor = vec3(texture(skybox, transmittedRay).rgb);
    vec3 reflectedColor = vec3(texture(skybox, reflectedRay).rgb);
    FragColor = vec4(reflectivity * reflectedColor + (1 - reflectivity) * transmittedColor, 1.0);
}