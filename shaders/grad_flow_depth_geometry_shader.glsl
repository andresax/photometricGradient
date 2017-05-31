#version 420
 
layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;

in vec4 shadowCoordV[];         /*coordinate for shadow mapping (camera 1)*/
in vec4 shadowCoord2V[];        /*coordinate for shadow mapping (camera 2)*/
in vec4 projectorTexCoordV[];   /*2D coordinate in camera 1*/
in vec4 projectorTexCoordVFlip[];   /*2D coordinate in camera 1flipped*/
in vec4 projector2TexCoordV[];  /*2D coordinate in camera 2*/
in vec4 positionPointV[];       /*3D point position*/
 
out vec4 shadowCoord;
out vec4 shadowCoord2;
out vec4 projectorTexCoord;
out vec4 projectorTexCoordFlip;
out vec4 projector2TexCoord;
out vec4 positionPoint;
out vec3 normalFacet;           /*normal to the facet the vertex belongs*/

 
 void main(){

  vec3 normal = normalize(cross((positionPointV[2]).xyz - (positionPointV[0]).xyz, (positionPointV[1]).xyz - (positionPointV[0]).xyz));
  
  for(int i = 0; i < gl_in.length(); i++){
    //emit the normal info
    normalFacet = normal;

    // copy attributes
    gl_Position = gl_in[i].gl_Position;
    shadowCoord = shadowCoordV[i];
    shadowCoord2 = shadowCoord2V[i];
    projectorTexCoord = projectorTexCoordV[i];
    projectorTexCoordFlip = projectorTexCoordVFlip[i];
    projector2TexCoord = projector2TexCoordV[i];
    positionPoint = positionPointV[i];

    // done with the vertex
    EmitVertex();
  }
}