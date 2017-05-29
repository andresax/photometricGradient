#version 420

layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;


in vec4 projector1TexCoordV[];   /*2D coordinate in camera 1*/
in vec2 tex2CoordV[];   /*2D coordinate in camera 1*/
in vec2 tex1CoordV[];   /*2D coordinate in camera 1*/


out vec4 projector1TexCoord;   /*2D coordinate in camera 1*/
out vec2 tex2Coord;   /*2D coordinate in camera 1*/
out vec2 tex1Coord;   /*2D coordinate in camera 1*/


uniform float imW;
uniform float imH;

void main(){

/*  vec2 p1 = tex1Coord[0]*vec2(imW, imH);
  vec2 p2 = tex1Coord[1]*vec2(imW, imH);
  vec2 p3 = tex1Coord[2]*vec2(imW, imH);

  area = 0.5 * abs((p3.x-p1.x) * (p2.y - p1.y) - (p2.x-p1.x) * (p3.y - p1.y));

  if(area > 16){*/
    for(int i = 0; i < gl_in.length(); i++){

      gl_Position = gl_in[i].gl_Position;
      projector1TexCoord = projector1TexCoordV[i];
      tex1Coord = tex1CoordV[i];
      tex2Coord = tex2CoordV[i];
      
      EmitVertex();
    }
  //}
}

