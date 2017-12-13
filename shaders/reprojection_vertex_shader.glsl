#version 420

in vec4 position;

out vec4 projector2TexCoord;   /*2D coordinate in camera 2*/
out vec4 shadowCoord1;         /*coordinate for shadow mapping (camera 2)*/
out vec4 shadowCoord2;         /*coordinate for shadow mapping (camera 2)*/

uniform mat4 MVP1;
uniform mat4 MVP2;
uniform float alpha;
uniform vec3 camCenter;


void main(){

  mat4 biasMatrix = mat4(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0
    );
  shadowCoord1 =  MVP1 * vec4(position.xyz, 1.0);
  shadowCoord1 = biasMatrix * shadowCoord1;
  shadowCoord1.z -= 0.000500;
  shadowCoord2 =  MVP2 * vec4(position.xyz, 1.0);
  shadowCoord2 = biasMatrix * shadowCoord2;
  shadowCoord2.z -= 0.000500;
  
  projector2TexCoord = MVP2 * vec4(position.xyz, 1.0);
  projector2TexCoord.y = -projector2TexCoord.y;
  projector2TexCoord = biasMatrix * projector2TexCoord;
  gl_Position =  MVP1 * vec4(position.xyz,1.0);
}
