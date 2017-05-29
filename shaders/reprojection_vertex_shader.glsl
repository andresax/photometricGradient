#version 420

in vec3 position;
in vec3 normal;

out vec4 projector2TexCoord;   /*2D coordinate in camera 2*/
out vec4 shadowCoord1;         /*coordinate for shadow mapping (camera 2)*/
out vec4 shadowCoord2;         /*coordinate for shadow mapping (camera 2)*/
out vec3 normalV;
out vec3 posV;
out vec4 oldpos2DV;
out vec4 pos2DV;

uniform mat4 MVP1;
uniform mat4 MVP2;
uniform float alpha;
uniform vec3 camCenter;



vec3 sweepPosition(vec3 p, float a, vec3 n, vec3 c){
  //simply along the normal
 vec3 position_new = p + a * n;

//c=vec3(-6.71999, -14.2551,0.279534);
  //or by considering the current viewing ray
  vec3 camToV1 = normalize(p-c);
  //float angle1 = atan(length(cross(normal,camToV1)),dot(normal,camToV1)); 

  vec3 x = camToV1;
  vec3 y = n;

  float angle1 = 2 * atan(length(x * length(y) - length(x) * y), length(x * length(y) + length(x) * y));
 
  float k_1 = 0.0f;
  if (abs(cos(angle1)) >= 0.05){ 
    //do not consider triangles for which the angle of incidence of the current ray is too narrow 
     k_1 = a * abs(cos(angle1));
  }
  position_new = p + k_1 * camToV1;
  return position_new;
}



void main(){

  vec3 position_new = sweepPosition(position, alpha, normal, camCenter);
  mat4 biasMatrix = mat4(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0
    );
  shadowCoord1 =  MVP1 * vec4(position_new, 1.0);
  shadowCoord1 = biasMatrix * shadowCoord1;
  shadowCoord1.z -= 0.00000500;
  shadowCoord2 =  MVP2 * vec4(position_new, 1.0);
  shadowCoord2 = biasMatrix * shadowCoord2;
  shadowCoord2.z -= 0.00000500;
  
  projector2TexCoord = MVP2 * vec4(position_new, 1.0);
  projector2TexCoord.y = -projector2TexCoord.y;
  projector2TexCoord = biasMatrix * projector2TexCoord;

  oldpos2DV = MVP1 * vec4(position, 1.0);
  pos2DV = MVP1 * vec4(position_new, 1.0);


  posV = position_new;
normalV = normal;
  gl_Position =  MVP1 * vec4(position_new,1.0);
}
