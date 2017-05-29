#version 420
 
layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;

in vec4 projector2TexCoordV[];
in vec4 shadowCoord2V[];
in vec4 shadowCoord1V[];
in vec3 posVV[];
in vec4 oldpos2DVV[];
in vec4 pos2DVV[];

//in vec4 shadowCoordV[];         /*coordinate for shadow mapping (camera 1)*/
//in vec4 shadowCoord2V[];        /*coordinate for shadow mapping (camera 2)*/
//in vec4 projectorTexCoordV[];   /*2D coordinate in camera 1*/
//in vec4 projectorTexCoordVFlip[];   /*2D coordinate in camera 1flipped*/
//in vec4 projector2TexCoordV[];  /*2D coordinate in camera 2*/
//in vec4 positionPointV[];       /*3D point position*/
 
out vec4 projector2TexCoord;
out vec4 shadowCoord2;
out vec4 shadowCoord1;
out vec3 posV;
out vec4 oldpos2DV;
out vec4 pos2DV;
out vec3 normalFacet;           /*normal to the facet the vertex belongs*/

 
 void main(){

  vec3 normal = normalize(cross((posVV[2]).xyz - (posVV[0]).xyz, (posVV[1]).xyz - (posVV[0]).xyz));
  
  for(int i = 0; i < gl_in.length(); i++){
    //emit the normal info
    normalFacet = normal;

    gl_Position = gl_in[i].gl_Position;
     projector2TexCoord = projector2TexCoordV[i];
     shadowCoord2 = shadowCoord2V[i];
     shadowCoord1 = shadowCoord1V[i];
     posV = posVV[i];
     oldpos2DV = oldpos2DVV[i];
     pos2DV= pos2DVV[i];
    // done with the vertex
    EmitVertex();
  }
}