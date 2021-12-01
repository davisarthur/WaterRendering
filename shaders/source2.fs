#version 330 core
in vec3 vertexPosition;
in vec3 normal;

uniform vec3 eye;
uniform samplerCube skybox;

out vec4 FragColor;

void main() {
   vec4 transmittedColor = vec4(0.0, 0.8, 0.8, 1.0);
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
   vec4 trasmittedColor = vec4(texture(skybox, transmittedRay).rgb, 1.0);
   FragColor = reflectivity * reflectedColor + (1 - reflectedColor) * transmittedColor;
}