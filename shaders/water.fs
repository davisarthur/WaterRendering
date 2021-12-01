#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;
in vec2 xzPos;

uniform sampler2D waterTexture;
uniform samplerCube skybox;
uniform vec3 eye;
uniform float maxY;
uniform float minY;
uniform float maxSlopeX;
uniform float minSlopeX;
uniform float maxSlopeZ;
uniform float minSlopeZ;

void main() { 
    vec4 data = texture(waterTexture, TexCoords);
    float height = minY + data.x * (maxY - minY);
    float slopeX = minSlopeX + data.y * (maxSlopeX - minSlopeX);
    float slopeZ = minSlopeZ + data.z * (maxSlopeZ - minSlopeZ);
    vec3 vertexPosition = vec3(xzPos.x, height, xzPos.y);
    vec3 normal = vec3(-slopeX, 1, -slopeZ);
    float nAir = 1.0;
    float nWater = 1.33;
    vec3 incidentRay = normalize(vec3(vertexPosition - eye));
    vec3 reflectedRay = reflect(incidentRay, normal);
    float dotProduct = dot(incidentRay, normal);
    float gamma = nAir / nWater * dotProduct;
    float secondaryTerm = sqrt(1.0 - pow(nAir / nWater * length(cross(incidentRay, normal)), 2.0));
    if (dotProduct > 0) {
        gamma -= secondaryTerm;
    }
    else {
        gamma += secondaryTerm;
    }
    float thetaAir = acos(abs(dot(incidentRay, normal))); 
    float thetaWater = asin(nAir / nWater * sin(thetaAir));
    vec3 transmittedRay = nAir / nWater * incidentRay + gamma * normal;
    float r1 = sin(thetaWater - thetaAir) / sin(thetaWater + thetaAir);
    float r2 = tan(thetaWater - thetaAir) / tan(thetaWater + thetaAir);
    float reflectivity = 0.5 * (r1 * r1 + r2 * r2);
    vec4 reflectedColor = vec4(texture(skybox, reflectedRay).rgb, 1.0);
    vec4 transmittedColor = vec4(texture(skybox, transmittedRay).rgb, 1.0);
    FragColor = reflectivity * reflectedColor + (1 - reflectedColor) * transmittedColor;
}