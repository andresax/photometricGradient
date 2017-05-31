#version 420


#define M_PI 3.1415926535897932384626433832795

in vec4 projector1TexCoord;
in vec2 tex2Coord;   /*2D coordinate in camera 1*/
in vec2 tex1Coord;   /*2D coordinate in camera 1*/
in vec4 shadowCoord1;

//layout(location = 0) 
layout(location = 0) out float d2mcc;

//uniform sampler2DShadow shadowMap1;
uniform  float  imW;
uniform  float imH;
uniform sampler2D mean;
uniform sampler2D var;
uniform sampler2D ncc;
uniform sampler2D img1;
uniform sampler2D img2;
uniform sampler2D reliability; 
uniform  float window;
uniform  float LOD;

void main(){

  vec4 I1 = 255.0*texture(img1, tex1Coord);
  vec4 I2 = 255.0*texture(img2, -tex2Coord);

  vec4 means, vars, nccs;
  float W = window;
  float sigma = 3.0;
  float sumWeight=0.0;
  float a = 0.0, b = 0.0, c = 0.0;

  float curRow, curCol;
  float step = (LOD+1);
  //step = (1);
  for(curRow = -(step)*W/imH; curRow <= (step)*W/imH; curRow += (step)/imH){
    if(tex2Coord.y + curRow>0 && tex2Coord.y + curRow<imH)
    for(curCol = -(step)*W/imW; curCol <= (step)*W/imW; curCol += step/imW){
      if(tex2Coord.x + curCol>0 && tex2Coord.x + curCol<imW){
        float gaussianWeight = (1/(sigma * 2*M_PI)) * exp (-(curCol*curCol + curRow * curRow)/(2*sigma*sigma));
        float curWeight = gaussianWeight;

        means = texture(mean, tex2Coord + vec2(curCol, curRow));
        vars = texture(var, tex2Coord + vec2(curCol, curRow));
        nccs = texture(ncc, tex2Coord + vec2(curCol, curRow));

        a += curWeight * -1.0/(sqrt(vars.x * vars.y));
        b += curWeight * nccs.x/vars.y;
        c += curWeight * (means.x/(sqrt(vars.x * vars.y)) - (means.y * nccs.x)/(vars.y));

        sumWeight += curWeight;
      }
    }
  }
  float aTot = a/sumWeight;
  float bTot = b/sumWeight;
  float cTot = c/sumWeight;

  vec4 rel = texture(reliability, tex2Coord );
  d2mcc = rel.x * (aTot * I1.x + bTot * I2.x + cTot);


      // d2mcc = texture(mean, tex2Coord).x;
  //d2mcc = (aTot * I1.x + bTot * I2.x + cTot);
  //d2mcc = abs(aTot);


/*
        means = texture(mean, tex2Coord );
        vars = texture(var, tex2Coord );
        nccs = texture(ncc, tex2Coord );
  d2mcc = means.x;*/
}
