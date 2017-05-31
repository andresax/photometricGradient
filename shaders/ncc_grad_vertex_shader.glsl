#version 420

in vec2 position;
in vec2 texcoord;

out vec4 projector1TexCoord;   /*2D coordinate in camera 1*/
out vec2 tex2Coord;   /*2D coordinate in camera 1*/
out vec2 tex1Coord;   /*2D coordinate in camera 1*/


void main(){
  
  tex2Coord=texcoord;
  tex2Coord.y=1.0-tex2Coord.y;
  tex1Coord=texcoord;
  
  gl_Position =  vec4(position,0.0,1.0);
}

