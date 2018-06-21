#version 420

#define M_PI 3.1415926535897932384626433832795
in vec4 shadowCoord;
in vec4 shadowCoord2;
in vec4 projectorTexCoord;
in vec4 projectorTexCoordFlip;
in vec4 projector2TexCoord;
in vec4 positionPoint;
in vec3 normalFacet;

//out vec4 color;
layout(location = 0) out vec4 gradientContribution;

uniform sampler2D gradimage2x;
uniform sampler2D gradimage2y;
uniform sampler2D gradSimilarityImg;
uniform sampler2DShadow shadowMap;
uniform sampler2DShadow shadowMap2;
uniform vec3 camPosition;
uniform vec3 cam2Position;
uniform mat4 MVPcam2;
uniform mat4 pointTocamMat2;
uniform mat4 MVPcam2Orig;
uniform float LOD;
uniform float hasW;

void main(){

  vec4 Pj_1, Pj_2, Pj_3;
  vec4 vertexColor,gradx,grady;
  mat3 gradProj = mat3(0.0);
  vec3 gradientPixel;
  vec3 camPointVector, camPointVector2;


  float bias1 = clamp(0.9*tan(acos(dot(normalFacet, positionPoint.xyz-camPosition))),0.0f,0.9);
  float bias2 = clamp(0.9*tan(acos(dot(normalFacet, positionPoint.xyz-cam2Position))),0.0f,0.9);
  //float bias2 = max(0.5 * (1.0 - dot(normalFacet, c2-positionP)), 0.05);
  vec4 shadowCoord1Biased = shadowCoord;
  vec4 shadowCoord2Biased = shadowCoord2;
  shadowCoord1Biased.z -= bias1;
  shadowCoord2Biased.z -= bias2;


  float shadowCoeff = textureProj(shadowMap, shadowCoord1Biased);
  float shadowCoeff2 = textureProj(shadowMap2, shadowCoord2Biased);

  vec4 positionPointToCam1= positionPoint;
  vec4 positionToCam = pointTocamMat2 * positionPoint;
  vec4 vecNorm = positionPoint;
  //gradient of second cam matrix projection computation

  Pj_1 =  vec4(MVPcam2Orig[0][0], MVPcam2Orig[0][1], MVPcam2Orig[0][2], MVPcam2Orig[0][3]);
  Pj_2 =  vec4(MVPcam2Orig[1][0], MVPcam2Orig[1][1], MVPcam2Orig[1][2], MVPcam2Orig[1][3]);
  Pj_3 =  vec4(MVPcam2Orig[2][0], MVPcam2Orig[2][1], MVPcam2Orig[2][2], MVPcam2Orig[2][3]);

  vec3 Xj = vec3(dot(Pj_1,vecNorm), dot(Pj_2,vecNorm), dot(Pj_3,vecNorm));
  vec3 DPj_Dx = ((Xj.z * Pj_1.xyz) - (Xj.x * Pj_3.xyz))/(Xj.z * Xj.z);
  vec3 DPj_Dy = ((Xj.z * Pj_2.xyz) - (Xj.y * Pj_3.xyz))/(Xj.z * Xj.z);


  vec4 gradSimilarity = shadowCoeff * shadowCoeff2 * texture2DProj(gradSimilarityImg, projectorTexCoord);

  //image gradient of second cam matrix projection
  gradx = 255.0* textureProjLod(gradimage2x, projector2TexCoord,LOD);
  grady = 255.0* textureProjLod(gradimage2y, projector2TexCoord,LOD);

  //compute vector d
  camPointVector = (positionPointToCam1 - vec4(camPosition,1.0)).xyz;
  camPointVector2 = (positionPointToCam1 - vec4(cam2Position,1.0)).xyz;

  float d_z = (positionPointToCam1).z;

  vec3 DIj_DPj ;
  DIj_DPj = gradx.x * DPj_Dx +  grady.x * DPj_Dy;

  gradientPixel = gradSimilarity.x * DIj_DPj;

  float f12 = camPointVector.x * gradientPixel.x + camPointVector.y * gradientPixel.y + camPointVector.z * gradientPixel.z;

  vec3 di  = vecNorm.xyz - cam2Position;
  float zi = length(di);

  float denomGrad = camPointVector.x*normalFacet.x + camPointVector.y*normalFacet.y + camPointVector.z*normalFacet.z;
  float zeroapprox = 0.000000000000001;

  gradientContribution =  shadowCoeff * shadowCoeff2 *vec4(1.0,0.0,0.0,1.0);
  if(denomGrad> zeroapprox || denomGrad < -zeroapprox ){

    /*vec3 x = camPointVector;
    vec3 y = camPointVector2;
    float angle = 2*atan(length(x * length(y) - length(x) * y), length(x * length(y) + length(x) * y));
    float mean = 45*M_PI/180;
    float stddev = M_PI/2;
    float coeff = min(1,1.05*exp(-((mean-angle)*(mean-angle))/(2*stddev*stddev)) );*/
    //float coeff = (1/sqrt(2*M_PI*stddev*stddev))*exp(-((mean-angle)*(mean-angle))/(2*stddev*stddev));

    vec3 x1 = normalize(camPointVector);
    vec3 x2 = normalize(camPointVector2);
    vec3 planeNormal = cross(x1,x2);

    vec3 endNormal = positionPoint.xyz + normalFacet;
    float pointPlaneDist = dot(planeNormal,normalFacet);
    vec3 projected_point = endNormal - pointPlaneDist*planeNormal;
    vec3 normalProj = projected_point - positionPoint.xyz;

    vec3 y = normalProj;

    float ang12 = 2 * atan(length(x1 * length(y) - length(x1) * y), length(x1 * length(y) + length(x1) * y)); 

    float ang1N = 2 * atan(length(x1 * length(y) - length(x1) * y), length(x1 * length(y) + length(x1) * y));
    vec3 cross1N = cross(x1, y);
    
    float ang2N = 2 * atan(length(x2 * length(y) - length(x2) * y), length(x2 * length(y) + length(x2) * y));
    vec3 cross2N = cross(x2, y);

    int sign = 1;
    if (dot(cross1N, cross2N) < 0) { // Or > 0
      sign = -1;
    }
    float diffAngle = ang1N + sign*ang2N;
    
    float mean = 50*M_PI/180;
    float stddev = M_PI/6;
    float stddev2 = M_PI/8;
    float coeffBaseline = exp(-((mean-ang12)*(mean-ang12))/(2*stddev*stddev));
    float coeffNormal = exp(-((0-diffAngle)*(0-diffAngle))/(2*stddev2*stddev2));
    float coeff = 0.25 * min(1,coeffBaseline ) + 0.75*min(1,coeffNormal );

    if(hasW>0){
      gradientContribution = (3.14/1.0339)*coeff*shadowCoeff * shadowCoeff2 * f12/denomGrad * vec4(-normalFacet, 1.0);
      gradientContribution =  shadowCoeff * shadowCoeff2 * f12/denomGrad * vec4(-normalFacet, 1.0);
    }else{
      gradientContribution =  shadowCoeff * shadowCoeff2 * f12/denomGrad * vec4(-normalFacet, 1.0);
      //gradientContribution = 0.6431  * shadowCoeff * shadowCoeff2 * f12/denomGrad * vec4(-normalFacet, 1.0);
    }

  }else{
    gradientContribution =  shadowCoeff * shadowCoeff2 *vec4(0.0,0.0,0.0,1.0);
  }
  
  //gradientContribution = vec4(gradSimilarity.x,0.0,0.0,1.0 );

}
