#version 420

in vec4 position;

out vec4 projector2TexCoordV;   /*2D coordinate in camera 2*/
out vec4 shadowCoord1V;         /*coordinate for shadow mapping (camera 2)*/
out vec4 shadowCoord2V;         /*coordinate for shadow mapping (camera 2)*/
out vec3 positionPV;

uniform mat4 MVP1;
uniform mat4 MVP2;
uniform vec3 c1;
uniform vec3 c2;


void main(){

  mat4 biasMatrix = mat4(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0
    );
  shadowCoord1V =  MVP1 * vec4(position.xyz, 1.0);
  shadowCoord1V = biasMatrix * shadowCoord1V;
  //shadowCoord1V.z -= 0.500;
  shadowCoord2V =  MVP2 * vec4(position.xyz, 1.0);
  shadowCoord2V = biasMatrix * shadowCoord2V;
  //shadowCoord2V.z -= 0.50;
  
  projector2TexCoordV = MVP2 * vec4(position.xyz, 1.0);
  projector2TexCoordV.y = -projector2TexCoordV.y;
  projector2TexCoordV = biasMatrix * projector2TexCoordV;
  gl_Position =  MVP1 * vec4(position.xyz,1.0);
}
