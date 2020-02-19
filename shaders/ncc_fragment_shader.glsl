#version 420
#define M_PI 3.1415926535897932384626433832795

in vec4 projector1TexCoord;
in vec2 tex2Coord;   
in vec2 tex1Coord;  
in vec4 shadowCoord1;

layout(location = 0) out float ncc;
layout(location = 1) out vec3 var;
layout(location = 2) out vec2 mean;
layout(location = 3) out float reliability;

//uniform sampler2DShadow shadowMap1;
uniform  float imW;
uniform  float imH;
uniform  float window;
uniform  float LOD;
uniform  float modeSSD;
uniform sampler2D image1;
uniform sampler2D image2Repr;
uniform sampler2D depthXYZ;
uniform sampler2D shadowMap;

void main(){

  vec4 img1;
  vec4 image2Reproj;
  float W = window;
  float sumWeight = 0.0f;
  float beta2     = 5.0f;
  float eps       = 5.0f;
  float sum1= 0.0f, sum2 = 0.0f, sqsum1 = 0.0f, sqsum2 = 0.0f, prod12 = 0.0f, diff12 = 0.0f;
  float offset = 0.995f;
  float step = LOD + 1.0f;
 //step = (1);
  float curRow, curCol;
  float sigma = (window / 2.0f);//*(LOD+1);
  float minDiff = -1.0f;
  float maxDiff = 0.0f;

  float curN    =  0.0f;
  float tot     =  0.0f;
  float avgDiff =  0.0f;
  vec4 currentDepth;

  vec4 centerDepthValue = texture(depthXYZ, tex2Coord);

  // for(curRow = -(step)*W/imH; curRow <= (step)*W/imH; curRow += (step)/imH){
  //   curCol = -(step)*W/imW;
  //   currentDepth = texture(depthXYZ, tex2Coord + vec2(curCol, curRow));//check why not with LOD
  //   if(minDiff< 0.0f || minDiff > abs(currentDepth.x-centerDepthValue.x)){
  //     minDiff = abs(currentDepth.x-centerDepthValue.x);
  //   }
  //   if(maxDiff< 0.0f || maxDiff < abs(currentDepth.x-centerDepthValue.x)){
  //     maxDiff = abs(currentDepth.x-centerDepthValue.x);
  //   }

  //   curCol = (step)*W/imW;
  //   currentDepth = texture(depthXYZ, tex2Coord + vec2(curCol, curRow));//check why not with LOD
  //   if(minDiff< 0.0f || minDiff > abs(currentDepth.x-centerDepthValue.x)){
  //     minDiff = abs(currentDepth.x-centerDepthValue.x);
  //   }
  //   if(maxDiff< 0.0f || maxDiff < abs(currentDepth.x-centerDepthValue.x)){
  //     maxDiff = abs(currentDepth.x-centerDepthValue.x);
  //   }
  // } 


  // for(curCol = -(step)*W/imW; curCol <= (step)*W/imW; curCol += (step)/imW){
  //     curRow = -(step)*W/imH;
  //     currentDepth = texture(depthXYZ, tex2Coord + vec2(curCol, curRow));//check why not with LOD
  //     if(minDiff< 0.0f || minDiff > abs(currentDepth.x-centerDepthValue.x)){
  //       minDiff = abs(currentDepth.x-centerDepthValue.x);
  //     }
  //     if(maxDiff< 0.0f || maxDiff < abs(currentDepth.x-centerDepthValue.x)){
  //       maxDiff = abs(currentDepth.x-centerDepthValue.x);
  //     }

  //     curRow = (step)*W/imH;
  //     currentDepth = texture(depthXYZ, tex2Coord + vec2(curCol, curRow));
  //     if(minDiff< 0.0f || minDiff > abs(currentDepth.x-centerDepthValue.x)){
  //       minDiff = abs(currentDepth.x-centerDepthValue.x);
  //     }
  //     if(maxDiff< 0.0f || maxDiff < abs(currentDepth.x-centerDepthValue.x)){
  //       maxDiff = abs(currentDepth.x-centerDepthValue.x);
  //     }
  // }  


  float diff;
  minDiff = 0.0;

//find max diff w.r.t. center depth
  for(curRow = -(step)*W/imH; curRow <= (step)*W/imH; curRow += (1.0)/imH){
    for(curCol = -(step)*W/imW; curCol <= (step)*W/imW; curCol += (1.0)/imW){
      currentDepth = texture(depthXYZ, tex2Coord + vec2(curCol, curRow));
      if( maxDiff < abs(currentDepth.x-centerDepthValue.x)){
        maxDiff = abs(currentDepth.x-centerDepthValue.x);
      }
    }
  }


float coherentDiff = 0.0;
float coherentDiffDistSq=0.0;

  for(curRow = -(step)*W/imH; curRow <= (step)*W/imH; curRow += (1.0)/imH){
    for(curCol = -(step)*W/imW; curCol <= (step)*W/imW; curCol += (1.0)/imW){
      currentDepth = texture(depthXYZ, tex2Coord + vec2(curCol, curRow));//check why not with LOD
      diff= abs(currentDepth.x-centerDepthValue.x);
      if(abs(diff - maxDiff) > abs(diff - minDiff)) {//diff is closer to minDiff
        avgDiff = avgDiff + diff;
        float dis = curRow*curRow + curCol*curCol;
        if(dis > coherentDiffDistSq){
          coherentDiffDistSq = dis;
          coherentDiff = diff;
        }
        curN = curN + 1.0f ;
      }
      //tot = tot + 1.0f;
    }
  }
  avgDiff = avgDiff / curN;
 
  curN=0.0;
  float curC, curR, gaussianWeight, curWeight;
  for(curRow = -(step)*W/imH; curRow <= (step)*W/imH; curRow += (step)/imH){

    if(tex1Coord.x + curRow > (1.0f - offset) && tex1Coord.x + curRow < offset){
      for(curCol = -(step)*W/imW; curCol <= (step)*W/imW; curCol += (step)/imW){
        if(tex1Coord.y + curCol > (1.0f - offset) && tex1Coord.y + curCol < offset){
          currentDepth = texture(depthXYZ, tex2Coord + vec2(curCol, -curRow));//check why not with LOD
          diff= abs(currentDepth.x-centerDepthValue.x);
          //if( diff < 3.0 * avgDiff){
            if(abs(diff - maxDiff)*10.0f > abs(diff - coherentDiff)) {//diff is closer to minDiff
            curC = curCol * imW;
            curR = curRow *imH;
            gaussianWeight = (1/(sigma * 2 * M_PI)) * exp (-(curC * curC + curR * curR)/(2 * sigma * sigma));
            curWeight = gaussianWeight;
            //img1 = 255.0f*texture(image1, tex1Coord + vec2(curCol, curRow));
            image2Reproj = 255.0f*texture(image2Repr, tex2Coord + vec2(curCol, curRow));//check why not with LOD
            img1 = 255.0f*textureLod(image1, tex1Coord + vec2(curCol, curRow),LOD);
            if((img1.x > 0.000000000001f ||img1.x < -0.000000000001f)&& image2Reproj.x > 0.000000000001f){
              sum1   += curWeight * img1.x;
              sum2   += curWeight * image2Reproj.x;
              sqsum1 += curWeight * img1.x * img1.x;
              sqsum2 += curWeight * image2Reproj.x * image2Reproj.x;
              prod12 += curWeight * img1.x * image2Reproj.x;
              diff12 += curWeight * img1.x - image2Reproj.x;
              sumWeight += curWeight;

            curN = curN + 1.0f ;
            }
          }
          tot = tot + 1.0f;
          }

      }
    }
  }
  float mean1 = sum1/sumWeight;
  float mean2 = sum2/sumWeight;
  float var1 = sqsum1/sumWeight - mean1*mean1 + beta2;
  float var2 = sqsum2/sumWeight - mean2*mean2 + beta2;
  float var12 = prod12/sumWeight - mean1*mean2;

  ncc  = var12/(sqrt(var1 * var2));
  mean = vec2(mean1, mean2);
  var  = vec3(var1, var2, var12);

  reliability = min(var1,var2)/(min(var1, var2) + eps*eps);
  // reliability = var1/(var1 + eps)*(1-modeSSD) ;//use reliability texture to save ssd if needed
  // reliability = diff12/sumWeight;//use reliability texture to save ssd if needed

}
