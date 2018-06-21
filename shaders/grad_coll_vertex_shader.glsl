#version 420

in vec4 position;               /*3D vertex position*/

out vec4 shadowCoordV;          /*coordinate for shadow mapping (camera 1)*/
out vec4 shadowCoord2V;         /*coordinate for shadow mapping (camera 2)*/
out vec4 projectorTexCoordV;    /*2D coordinate in camera 1*/
out vec4 projectorTexCoordVFlip;    /*2D coordinate in camera 1 flipped*/
out vec4 projector2TexCoordV;   /*2D coordinate in camera 2*/
out vec4 positionPointV;        /*3D point position*/

uniform mat4 MVPcam1;
uniform mat4 MVPcam2;

void main(){

    gl_Position = MVPcam1 * vec4(position.xyz, 1.0);
    //3D position of the vertex
    positionPointV = vec4(position.xyz, 1.0);

    mat4 biasMatrix = mat4(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
        );
    
    shadowCoordV = biasMatrix * MVPcam1 * vec4(position.xyz, 1.0);
    // shadowCoordV.z -= 0.00000500;
    // shadowCoordV.z -= 0.0500;
    shadowCoord2V = biasMatrix * MVPcam2 * vec4(position.xyz, 1.0);
    // Used to lower moiré pattern and self-shadowing
    // shadowCoord2V.z -= 0.00000500;
    // shadowCoord2V.z -= 0.05000;
    
    projectorTexCoordV = MVPcam1 * vec4(position.xyz, 1.0);
    projectorTexCoordV.y = projectorTexCoordV.y;
    projectorTexCoordV = biasMatrix * projectorTexCoordV;
    projectorTexCoordV.x = projectorTexCoordV.x / projectorTexCoordV.w;
    projectorTexCoordV.y = projectorTexCoordV.y / projectorTexCoordV.w;
    projectorTexCoordV.z = projectorTexCoordV.z / projectorTexCoordV.w;
    projectorTexCoordV.w = projectorTexCoordV.w / projectorTexCoordV.w;

    projectorTexCoordVFlip = MVPcam1 * vec4(position.xyz, 1.0);
    projectorTexCoordVFlip.y = - projectorTexCoordVFlip.y;
    projectorTexCoordVFlip = biasMatrix * projectorTexCoordVFlip;

    projector2TexCoordV = MVPcam2 * vec4(position.xyz, 1.0);
    projector2TexCoordV.y = -projector2TexCoordV.y;
    projector2TexCoordV = biasMatrix * projector2TexCoordV;



}

