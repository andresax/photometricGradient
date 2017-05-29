#version 420
#define M_PI 3.1415926535897932384626433832795

in vec4 projector1TexCoord;
in vec2 tex2Coord;   /*2D coordinate in camera 1*/
in vec2 tex1Coord;   /*2D coordinate in camera 1*/
in vec4 shadowCoord1;

/*output texture 0.0...255.0*/
layout(location = 0) out float ncc;
layout(location = 1) out vec3 var;
layout(location = 2) out vec2 mean;
layout(location = 3) out float reliability;

//uniform sampler2DShadow shadowMap1;
uniform  float imW;
uniform  float imH;
uniform  float window;
uniform  float LOD;
uniform sampler2D image1;
uniform sampler2D image2Repr;

void main(){

  vec4 img1;
  vec4 image2Reproj;
  float W = window;
  float sumWeight=0.0;
  float beta2 = 10.0;
  float eps = 5.0;
  float sum1 = 0.0, sum2 = 0.0, sqsum1 = 0.0, sqsum2 = 0.0, prod12 = 0.0;
  
  float curRow, curCol;
  float sigma = 2.0;

  float offset = 0.995;

  float curC, curR, gaussianWeight, curWeight;
  for(curRow = -W/imH; curRow < W/imH; curRow += (LOD+1)/imH){

    if(tex1Coord.x + curRow > (1.0 - offset) && tex1Coord.x + curRow < offset){
      for(curCol = -W/imW; curCol < W/imW; curCol += (LOD+1)/imW){
        if(tex1Coord.y + curCol > (1.0 - offset) && tex1Coord.y + curCol < offset){
          curC = curCol * imW;
          curR = curRow *imH;
          gaussianWeight = (1/(sigma * 2 * M_PI)) * exp (-(curC * curC + curR * curR)/(2 * sigma * sigma));
          curWeight = gaussianWeight;
          img1 = 255.0*texture(image1, tex1Coord + vec2(curCol, curRow));
          image2Reproj = 255.0*texture(image2Repr, tex2Coord + vec2(curCol, curRow));
          //img1 = 255.0*textureLod(image1, tex1Coord + vec2(curCol, curRow),3);
          //image2Reproj = 255.0*textureLod(image2Repr, tex2Coord + vec2(curCol, curRow),3);
          sum1 += curWeight * img1.x;
          sum2 += curWeight * image2Reproj.x;
          sqsum1 += curWeight * img1.x * img1.x;
          sqsum2 += curWeight * image2Reproj.x * image2Reproj.x;
          prod12 += curWeight * img1.x * image2Reproj.x;
          sumWeight += curWeight;
        }
      }
    }
  }
  float mean1 = sum1/sumWeight;
  float mean2 = sum2/sumWeight;
  float var1 = sqsum1/sumWeight - mean1*mean1 + beta2;
  float var2 = sqsum2/sumWeight - mean2*mean2 + beta2;
  float var12 = prod12/sumWeight - mean1*mean2;

  ncc = var12/(sqrt(var1 * var2));
  
  mean = vec2(mean1, mean2);
  var = vec3(var1, var2, var12);

  reliability = min(var1,var2)/(min(var1, var2) + eps);
  reliability = var1/(var1 + eps);

}
