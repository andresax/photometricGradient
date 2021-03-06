#version 420

layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;


in vec4 projector1TexCoordV[];
in vec2 tex2CoordV[];
in vec2 tex1CoordV[];


out vec4 projector1TexCoord;   
out vec2 tex2Coord;   
out vec2 tex1Coord;  


uniform float imW;
uniform float imH;

void main(){


    for(int i = 0; i < gl_in.length(); i++){

      gl_Position = gl_in[i].gl_Position;
      projector1TexCoord = projector1TexCoordV[i];
      tex1Coord = tex1CoordV[i];
      tex2Coord = tex2CoordV[i];
      
      EmitVertex();
    }
}

