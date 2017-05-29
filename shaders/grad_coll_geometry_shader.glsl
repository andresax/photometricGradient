#version 420

#define M_PI 3.1415926535897932384626433832795
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
out vec3 sumGradient;

//uniform mat4 MVP;
uniform  float imW;
uniform  float imH;
uniform sampler2D gradMap;
uniform sampler2DShadow shadowMap;
uniform sampler2DShadow shadowMap2;
uniform float LOD;

vec3 normalizeH(vec3 pt){
  float norma = 1.0;
  pt.x = pt.x /norma;
  pt.y = pt.y /norma;
  pt.z = pt.z /norma;
  return pt;
}

vec4 normalizeH(vec4 pt){
  vec4 ptr;
  ptr.x = pt.x /pt.w;
  ptr.y = pt.y /pt.w;
  ptr.z = pt.z /pt.w;
  ptr.w = pt.w /pt.w;
  return ptr;
}

float orientPoint(vec2 v0, vec2 v1, vec2 p){
  mat2 m;
  m[0][0] = (v1.x - v0.x);  m[0][1] = ( p.x - v0.x);
  m[1][0] = (v1.y - v0.y);  m[1][1] = ( p.y - v0.y);

  return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}

vec3 barycentricCoordMine(vec2 p, vec2 p0, vec2 p1, vec2 p2){
  vec2 v0, v1, v2;
  //First Check if triangle is counter-clockwise
  if(orientPoint(p0, p1, p2) > 0){
    v0 = p0;
    v1 = p1;
    v2 = p2;
  }else{
    v0 = p0;
    v1 = p2;
    v2 = p1;
  }
  // Compute barycentric coordinates w.r.t pt1
  vec3 barycentricCoordinates;

  float areaTrtwice = orientPoint(v0, v1, v2);

  if(areaTrtwice!=0){
    barycentricCoordinates.x = orientPoint(v0, v1, p)/areaTrtwice;
    barycentricCoordinates.y = orientPoint(v1, v2, p)/areaTrtwice;
    barycentricCoordinates.z = orientPoint(v2, v0, p)/areaTrtwice;
  }else {
   return vec3(-1.0);
  }

  return barycentricCoordinates;
}


vec3 barycentricCoordMine(vec2 p, vec2 v0, vec2 v1, vec2 v2, float twiceSignedArea){
  // Compute barycentric coordinates w.r.t pt1
  vec3 barycentricCoordinates;
  if(twiceSignedArea!=0){
    barycentricCoordinates.x = orientPoint(v0, v1, p)/twiceSignedArea;
    barycentricCoordinates.y = orientPoint(v1, v2, p)/twiceSignedArea;
    barycentricCoordinates.z = orientPoint(v2, v0, p)/twiceSignedArea;
  }else {
   return vec3(-1.0);
  }

  return barycentricCoordinates;
}



void main(){
  vec2 pt0 = projectorTexCoordV[0].xy;
  vec2 pt1 = projectorTexCoordV[1].xy;
  vec2 pt2 = projectorTexCoordV[2].xy;

  vec3 sumGradientTot[3];
  sumGradientTot[0] = vec3(0.0);
  sumGradientTot[1] = vec3(0.0);
  sumGradientTot[2] = vec3(0.0);
 
  bool switchedVertices;


  vec2 maxT,minT;
  maxT.x = max(max(pt1.x,pt2.x),pt0.x);
  maxT.y = max(max(pt1.y,pt2.y),pt0.y);
  minT.x = min(min(pt1.x,pt2.x),pt0.x);
  minT.y = min(min(pt1.y,pt2.y),pt0.y);

  //check boundary
  maxT.x = min(maxT.x, imW);
  maxT.y = min(maxT.y, imH);
  minT.x = max(minT.x, 0.0);
  minT.y = max(minT.y, 0.0);

  float twiceSignedArea = orientPoint(pt0, pt1, pt2);
  vec2 v0, v1, v2;

  switchedVertices = twiceSignedArea > 0;
  
  if (switchedVertices){
    v0 = pt0;
    v1 = pt1;
    v2 = pt2;
  }else{
    v0 = pt0;
    v1 = pt2;
    v2 = pt1;
  }

int end=0;
  vec3 area = vec3(0.0);

  float stepX = (LOD+1)/imW; 
  float stepY = (LOD+1)/imH;

  float curX = (minT.x);
  while(curX <= (maxT.x)&&end==0){

    float curY = (minT.y);
    while(curY <= (maxT.y)&&end==0){
     
      vec2 P = vec2(curX, curY);
      vec3 barycCoord = barycentricCoordMine(P, v0, v1, v2,twiceSignedArea);

      // Check if point is in triangle
      if(barycCoord.x >=0 && barycCoord.y >=0 &&  barycCoord.z >=0 ){

        vec4 curPixelValue;// = textureProj(gradMap, vec4(curX, curY, 0.0, 1.0));
        curPixelValue = texture(gradMap, vec2(curX, curY));

        sumGradientTot[0] = sumGradientTot[0] + barycCoord.x * curPixelValue.xyz;
        area.x = area.x + barycCoord.x;
        if(switchedVertices){
          sumGradientTot[1] = sumGradientTot[1] + barycCoord.y * curPixelValue.xyz;
          area.y = area.y + barycCoord.y;
          sumGradientTot[2] = sumGradientTot[2] + barycCoord.z * curPixelValue.xyz;
          area.z = area.z + barycCoord.z;
          }else{
            sumGradientTot[1] = sumGradientTot[1] + barycCoord.z * curPixelValue.xyz;
            area.y = area.y + barycCoord.z;
            sumGradientTot[2] = sumGradientTot[2] + barycCoord.y * curPixelValue.xyz;
            area.z = area.z + barycCoord.y;
          }
        }
      curY = curY + stepY;
    }
    curX = curX + stepX;
  }
 
  vec3 normal = normalize(cross((positionPointV[2]).xyz - (positionPointV[0]).xyz, (positionPointV[1]).xyz - (positionPointV[0]).xyz));

  for(int i = 0; i < gl_in.length(); i++){
     float shadowCoeff = textureProj(shadowMap, shadowCoord);
     float shadowCoeff2 = textureProj(shadowMap2, shadowCoord2);
     gl_Position = gl_in[i].gl_Position;
     shadowCoord = shadowCoordV[i];
     shadowCoord2 = shadowCoord2V[i];
     projectorTexCoord = projectorTexCoordV[i];
     projectorTexCoordFlip = projectorTexCoordVFlip[i];
     projector2TexCoord = projector2TexCoordV[i];
     positionPoint = positionPointV[i];

    if (abs(twiceSignedArea)<0.0000000000000001)
      sumGradient = vec3(0.0);
    else{
      sumGradient = shadowCoeff * shadowCoeff2 * vec3(sumGradientTot[i].xyz/abs(twiceSignedArea/2));
    }
    EmitVertex();
  }
}