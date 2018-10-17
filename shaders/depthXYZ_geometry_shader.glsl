#version 420
 
layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;

in vec4 shadowCoordV[];
in vec3 positionPV[];
 
out vec4 shadowCoord;
out vec3 normalFacet;
out vec3 positionP;
 
void main(){

  vec3 normal = normalize(cross((positionPV[2]).xyz - (positionPV[0]).xyz, (positionPV[1]).xyz - (positionPV[0]).xyz));
  
  for(int i = 0; i < gl_in.length(); i++){
    //emit the normal info
    normalFacet = normal;
    gl_Position = gl_in[i].gl_Position;
    shadowCoord = shadowCoordV[i];
    positionP = positionPV[i];
    
    // done with the vertex
    EmitVertex();
  }
}