#version 420


#define M_PI 3.1415926535897932384626433832795

in vec4 projector1TexCoord;
in vec2 tex2Coord;   
in vec2 tex1Coord;  

//layout(location = 0) 
layout(location = 0) out float d2mcc;

uniform float  imW;
uniform float imH;
uniform sampler2D mean;
uniform sampler2D var;
uniform sampler2D ncc;
uniform sampler2D img1;
uniform sampler2D img2;
uniform sampler2D reliability; 
uniform float window;
uniform float LOD;
uniform sampler2D depthXYZ;

void main(){

  vec4 I1 = 255.0*texture(img1, tex1Coord);
  vec4 I2 = 255.0*texture(img2, -tex2Coord);

  vec4 means, vars, nccs;
  float W = window;
  float sigma = (window/2);//*(LOD+1);
  //sigma = 2.0;
  float sumWeight=0.0;
  float a = 0.0, b = 0.0, c = 0.0;



  float curRow, curCol;
  float step = (LOD+1);
  float minDiff = -1.0f;
  float maxDiff = -1.0f;


  float curN    =  0.0f;
  float tot     =  0.0f;
  float avgDiff =  0.0f;
  vec4 currentDepth;

  vec4 centerDepthValue = texture(depthXYZ, tex2Coord);//check why not with LOD
  // for(curRow = -(step)*W/imH; curRow <= (step)*W/imH; curRow += (step)/imH){
  //   curCol = -(step)*W/imW;
  //   currentDepth = texture(depthXYZ, tex1Coord + vec2(curCol, curRow));//check why not with LOD
  //   if(minDiff< 0.0f || minDiff > abs(currentDepth.x-centerDepthValue.x)){
  //     minDiff = abs(currentDepth.x-centerDepthValue.x);
  //   }
  //   if(maxDiff< 0.0f || maxDiff < abs(currentDepth.x-centerDepthValue.x)){
  //     maxDiff = abs(currentDepth.x-centerDepthValue.x);
  //   }

  //   curCol = (step)*W/imW;
  //   currentDepth = texture(depthXYZ, tex1Coord + vec2(curCol, curRow));//check why not with LOD
  //   if(minDiff< 0.0f || minDiff > abs(currentDepth.x-centerDepthValue.x)){
  //     minDiff = abs(currentDepth.x-centerDepthValue.x);
  //   }
  //   if(maxDiff< 0.0f || maxDiff < abs(currentDepth.x-centerDepthValue.x)){
  //     maxDiff = abs(currentDepth.x-centerDepthValue.x);
  //   }
  // } 


  // for(curCol = -(step)*W/imW; curCol <= (step)*W/imW; curCol += (step)/imW){
  //     curRow = -(step)*W/imH;
  //     currentDepth = texture(depthXYZ, tex1Coord + vec2(curCol, curRow));//check why not with LOD
  //     if(minDiff< 0.0f || minDiff > abs(currentDepth.x-centerDepthValue.x)){
  //       minDiff = abs(currentDepth.x-centerDepthValue.x);
  //     }
  //     if(maxDiff< 0.0f || maxDiff < abs(currentDepth.x-centerDepthValue.x)){
  //       maxDiff = abs(currentDepth.x-centerDepthValue.x);
  //     }

  //     curRow = (step)*W/imH;
  //     currentDepth = texture(depthXYZ, tex1Coord + vec2(curCol, curRow));//check why not with LOD
  //     if(minDiff< 0.0f || minDiff > abs(currentDepth.x-centerDepthValue.x)){
  //       minDiff = abs(currentDepth.x-centerDepthValue.x);
  //     }
  //     if(maxDiff< 0.0f || maxDiff < abs(currentDepth.x-centerDepthValue.x)){
  //       maxDiff = abs(currentDepth.x-centerDepthValue.x);
  //     }
  // }  

// float diff;

//   for(curRow = -(step)*W/imH; curRow <= (step)*W/imH; curRow += (step)/imH){

//     for(curCol = -(step)*W/imW; curCol <= (step)*W/imW; curCol += (step)/imW){

//       currentDepth = texture(depthXYZ, tex1Coord + vec2(curCol, curRow));//check why not with LOD
//        diff= abs(currentDepth.x-centerDepthValue.x);
//       if(abs(diff - maxDiff)*1.0f > abs(diff - minDiff)) {//diff is closer to minDiff
//         avgDiff = avgDiff + diff;
//               // curN = curN + 1.0f ;
//       }
//       tot = tot + 1.0f;
//     }
//   }
//   avgDiff = avgDiff / curN;
 

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



  //step = (1);
  for(curRow = -(step)*W/imH; curRow <= (step)*W/imH; curRow += (step)/imH){
    if(tex2Coord.y + curRow>0 && tex2Coord.y + curRow<imH)
    for(curCol = -(step)*W/imW; curCol <= (step)*W/imW; curCol += step/imW){
      if(tex2Coord.x + curCol>0 && tex2Coord.x + curCol<imW){
        currentDepth = texture(depthXYZ, tex2Coord + vec2(curCol, curRow));//check why not with LOD
        diff= abs(currentDepth.x-centerDepthValue.x);
             // if( abs(currentDepth.x-centerDepthValue.x) < 1.0f * avgDiff){
          if(abs(diff - maxDiff)*10.0f > abs(diff - coherentDiff)) {//diff is closer to minDiff
          float  curC = curCol * imW;
          float  curR = curRow *imH;
          //float  gaussianWeight = (1/(sigma * 2 * M_PI)) * exp (-(curC * curC + curR * curR)/(2 * sigma * sigma));
          float gaussianWeight = (1/(sigma * 2*M_PI)) * exp (-(curCol*curCol + curRow * curRow)/(2*sigma*sigma));
          float curWeight = gaussianWeight;
          // curWeight = 1.0;

          means = texture(mean, tex2Coord + vec2(curCol, curRow));
          vars = texture(var, tex2Coord + vec2(curCol, curRow));
          nccs = texture(ncc, tex2Coord + vec2(curCol, curRow));

          a += curWeight * -1.0/(sqrt(vars.x * vars.y));
          b += curWeight * nccs.x/vars.y;
          c += curWeight * (means.x/(sqrt(vars.x * vars.y)) - (means.y * nccs.x)/(vars.y));

          sumWeight += curWeight;
        curN = curN + 1.0f ;
        }
        tot = tot + 1.0f;
      }
    }
  }
  float aTot = a/sumWeight;
  float bTot = b/sumWeight;
  float cTot = c/sumWeight;

  vec4 rel = texture(reliability, tex2Coord );
  d2mcc = rel.x * (aTot * I1.x + bTot * I2.x + cTot);
}
