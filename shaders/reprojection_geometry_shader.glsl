#version 420
 
layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;

in vec4 projector2TexCoordV[];
in vec4 shadowCoord2V[];
in vec4 shadowCoord1V[];
 
out vec4 projector2TexCoord;
out vec4 shadowCoord2;
out vec4 shadowCoord1;
out vec3 normalFacet;
 
void main(){

  vec3 normal = normalize(cross((posVV[2]).xyz - (posVV[0]).xyz, (posVV[1]).xyz - (posVV[0]).xyz));
  
  for(int i = 0; i < gl_in.length(); i++){
    //emit the normal info
    normalFacet = normal;
    gl_Position = gl_in[i].gl_Position;
    projector2TexCoord = projector2TexCoordV[i];
    shadowCoord2 = shadowCoord2V[i];
    shadowCoord1 = shadowCoord1V[i];
    
    // done with the vertex
    EmitVertex();
  }
}