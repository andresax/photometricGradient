#version 420

in vec4 shadowCoord;
in vec3 normalFacet;
in vec3 positionP;

layout(location = 0) out float fragmentdepth;

uniform vec3 center;
uniform sampler2DShadow shadowMap;
void main(){
  float bias = clamp(0.9*tan(acos(dot(normalFacet, positionP-center))),0.0f,0.9);


  vec4 shadowCoordBiased = shadowCoord;
  shadowCoordBiased.z -= bias;

  float shadowCoeff = textureProj(shadowMap, shadowCoordBiased);


  fragmentdepth = shadowCoeff * length(center-positionP);
 // fragmentdepth = 1.0;
    
}
