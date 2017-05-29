#version 420

in vec3 sumGradient;

in vec4 shadowCoord;
in vec4 shadowCoord2;

out vec4 color;

uniform sampler2D gradMap;
uniform sampler2DShadow shadowMap;
uniform sampler2DShadow shadowMap2;

void main(){
  float shadowCoeff = textureProj(shadowMap, shadowCoord);
  float shadowCoeff2 = textureProj(shadowMap2, shadowCoord2);
  
  color = vec4(sumGradient.x, sumGradient.y,sumGradient.z,1.0);
  //color = shadowCoeff2 * shadowCoeff* vec4(sumGradient.x, sumGradient.y,sumGradient.z,1.0);
}
