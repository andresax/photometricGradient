#version 420

in vec4 projector2TexCoord;
in vec4 shadowCoord2;
in vec4 shadowCoord1;
in vec3 normalFacet;
in vec3 positionP;

layout (location=0) out vec4 reproj;

uniform sampler2D image2;
uniform sampler2DShadow shadowMap1;
uniform sampler2DShadow shadowMap2;
uniform float LOD;
uniform vec3 c1;
uniform vec3 c2;

void main(){

  float bias1 = clamp(0.005*tan(acos(dot(normalFacet, positionP-c1))),0.0f,0.00005);
  float bias2 = clamp(0.005*tan(acos(dot(normalFacet, positionP-c2))),0.0f,0.00005);
  vec4 shadowCoord1Biased = shadowCoord1;
  vec4 shadowCoord2Biased = shadowCoord2;
  shadowCoord1Biased.z -= bias1;
  shadowCoord2Biased.z -= bias2;


  vec4 image2Reproj = textureProjLod(image2, projector2TexCoord,LOD);
  float shadowCoeff1 = textureProj(shadowMap1, shadowCoord1Biased);
  float shadowCoeff2 = textureProj(shadowMap2, shadowCoord2Biased);

  reproj = shadowCoeff1 *shadowCoeff2* vec4(image2Reproj.x, image2Reproj.x,image2Reproj.x, 0.0);
}