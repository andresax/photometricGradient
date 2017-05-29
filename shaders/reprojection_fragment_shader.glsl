#version 420

in vec4 projector2TexCoord;
in vec4 shadowCoord2;
in vec4 shadowCoord1;
in vec3 normalV;
in vec3 posV;
in vec4 oldpos2DV;
in vec4 pos2DV;

layout (location=0) out vec4 reproj;

uniform sampler2D image2;
uniform sampler2DShadow shadowMap1;
uniform sampler2DShadow shadowMap2;
uniform float LOD;

void main(){

  vec3 l = normalize(vec3(0.10,0.5,1.0));
  vec3 n = (normalV);
  float cosTheta = clamp(dot(n, l),0,1);
  float cosTheta2 = clamp(dot(-n, l),0,1);


  vec4 image2Reproj = textureProjLod(image2, projector2TexCoord,LOD);
  float shadowCoeff2 = textureProj(shadowMap2, shadowCoord2);
  float shadowCoeff1 = textureProj(shadowMap1, shadowCoord1);
  
  vec2 projector2TexCoordXY, oldpos2DVXY,pos2DVXY;

  projector2TexCoordXY.x = projector2TexCoord.x/projector2TexCoord.w;
  projector2TexCoordXY.y = projector2TexCoord.y/projector2TexCoord.w;

  oldpos2DVXY.x = oldpos2DV.x;
  oldpos2DVXY.y = oldpos2DV.y;
  pos2DVXY.x = pos2DV.x;
  pos2DVXY.y = pos2DV.y;

  float pointTraslation = distance(pos2DVXY, oldpos2DVXY);


 // if(  projector2TexCoordXY.x > 0.05 && projector2TexCoordXY.x < 0.95
  //  && projector2TexCoordXY.y > 0.05 && projector2TexCoordXY.y < 0.95
    //&& pointTraslation > 20.0/1392.0
   // )
   //{ reproj = shadowCoeff1 * vec4(image2Reproj.x, image2Reproj.x, image2Reproj.x, 0.0); 

   // reproj = shadowCoeff1  * vec4(abs(pos2DVXY.x-oldpos2DVXY.x), abs(pos2DVXY.x-oldpos2DVXY.x), abs(pos2DVXY.x-oldpos2DVXY.x), 0.0)* 5.0; 
    //reproj = shadowCoeff1  * vec4(0.0,0.10, 10.0*pointTraslation.x, 0.0)* max(cosTheta,cosTheta2); 
 // }
 // else{
 //   reproj = -vec4(1.0f, 1.0f, 1.0f, 0.0); 
  //}
  reproj = shadowCoeff1 *shadowCoeff2* vec4(image2Reproj.x, image2Reproj.x, image2Reproj.x, 0.0);
    
  //  reproj = shadowCoeff1*vec4(1.0f, 1.0f, 1.0f, 0.0); 

}