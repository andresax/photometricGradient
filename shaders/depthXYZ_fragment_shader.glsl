#version 420

in vec4 positionXYZ;

layout(location = 0) out float fragmentdepth;

uniform vec3 center;
void main(){
    fragmentdepth = glm::length(center-positionXYZ);
}
