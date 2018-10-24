#version 420

in vec4 position;
out vec3 positionPV;
out vec4 shadowCoordV;      

uniform mat4 MVP;
uniform vec3 center;
uniform sampler2DShadow shadowMap;

void main(){

  mat4 biasMatrix = mat4(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0
    );
  shadowCoordV =  MVP * vec4(position.xyz, 1.0);
  shadowCoordV = biasMatrix * shadowCoordV;
  positionPV = position.xyz;
  gl_Position =  MVP * vec4(position.xyz,1.0);
}

