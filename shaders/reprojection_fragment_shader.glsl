#version 420

in vec4 projector2TexCoord;
in vec4 shadowCoord2;
in vec4 shadowCoord1;
in vec3 normalV;

layout (location=0) out vec4 reproj;

uniform sampler2D image2;
uniform sampler2DShadow shadowMap1;
uniform sampler2DShadow shadowMap2;
uniform float LOD;

void main(){

  vec4 image2Reproj = textureProjLod(image2, projector2TexCoord,LOD);
  float shadowCoeff2 = textureProj(shadowMap2, shadowCoord2);
  float shadowCoeff1 = textureProj(shadowMap1, shadowCoord1);

  reproj = shadowCoeff1 *shadowCoeff2* vec4(image2Reproj.x, image2Reproj.x,image2Reproj.x, 0.0);
  //reproj = shadowCoeff1 * vec4(1.0);
  
}