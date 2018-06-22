#version 420

in vec4 position;
out vec3 positionXYZ;

uniform mat4 MVP;
uniform vec3 center;

void main(){
  positionXYZ = position.xyz;
  gl_Position =  MVP * vec4(position.xyz,1.0);
}

