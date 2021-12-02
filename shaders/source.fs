#version 330 core
in vec3 vertexPosition;
in vec3 normal;

uniform vec3 eye;

out vec4 FragColor;

void main() {
   vec4 transmittedColor = vec4(0.0, 0.8, 0.8, 1.0);
   vec4 reflectedColor = vec4(0.64, 0.84, 1.0, 1.0);
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
   FragColor = reflectivity * reflectedColor + (1 - reflectivity) * transmittedColor;
}