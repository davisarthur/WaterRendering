#version 330 core
in vec3 vertexPosition;
in vec3 normal;

uniform vec3 eye;

out vec4 FragColor;

void main() {
   vec3 transmittedColor = vec3(0.0, 0.3, 0.4);
   vec3 reflectedColor = vec3(0.4, 0.6, 0.8);
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

   // caustics
   vec3 up = vec3(0.0, 1.0, 0.0);
   vec3 causticRay = refract(up, -normal, nWater / nAir);
   float causticBrightness = pow(dot(causticRay, up), 1000);
   vec3 causticColor = vec3(1.0, 1.0, 1.0);

   FragColor = vec4(reflectivity * reflectedColor + (1 - reflectivity) * transmittedColor + 0.4 * causticColor * causticBrightness, 1);
}