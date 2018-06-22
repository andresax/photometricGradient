#version 420

in vec3 positionXYZ;

layout(location = 0) out float fragmentdepth;

uniform vec3 center;
void main(){
    fragmentdepth = length(center-positionXYZ);
    
}
