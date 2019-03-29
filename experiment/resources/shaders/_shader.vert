#version 450
float people_eye = 0.2;
float magic_VR_InstructionPlaneYOff  = -1.5f;
float magic_VR_screenCoordinatesScaling = 0.00224;
float otho_ScalingFactor = 1.0;
float coef = 0.0001;
in vec3 canvasVertex;
in vec2 canvasUV;

in vec3 cursorVertex;
in vec3 cursorNormal;
in vec2 cursorUV;

in vec3 vertex; // xyz - position
in vec3 normal; // xyz - normal
in vec3 referFramesVertex;
in vec3 floorFramesVertex;
in vec3 skyBoxVertex;
in vec3 homeVertex; // xyz - position

uniform vec4 cursors;
uniform float people_height;
uniform float what;
uniform float dim = 3.f;
uniform vec3 cameraOriginalPosition;
uniform vec3 cameraPosition;
uniform vec3 vertexColor;
uniform bool needRotationUpdate;
uniform vec3 near_far_zero;
uniform vec4 factors;
vec4 referFramesColor = vec4(0.93, 0.93, 0.93, 1.0f);
// matrices
uniform mat4 m;
uniform mat4 r;
uniform mat4 rotationReferViewMatrix;

uniform mat4 revertCameraPosMatrix;
uniform mat4 headPosition;
uniform mat4 controllerPosition;
uniform mat4 headViewMatrix;
uniform mat4 headProjectionMatrix;
uniform mat4 eyePose;
uniform mat4 eyeProjection;
uniform vec2 cursorPositionOnScreen;
uniform vec2 screenDimensions;
uniform vec2 depthMapResolution;

// position of light and camera

// data for fragment shader

out vec3 o_toLight;
out vec3 o_toCamera;
out float o_att;
out vec3 o_vertexColor;
out vec3 o_near_far_zero;

out float o_what;
out vec4 o_factors;
out vec2 o_screenDimensions;
float screenDimensionsX2Pixel = 2.0 / screenDimensions[0];
float screenDimensionsY2Pixel = 2.0 / screenDimensions[1];
out vec2 o_depthMapResolution;
out vec2 o_canvasUV;
out vec3 o_canvas;
out vec4 o_referFramesColor;
out vec2 o_texDepthCoord;
out vec3 o_Idif;
out vec3 o_Iamb;

// parameters of the light and possible values
vec3 u_lightAmbientIntensity = vertexColor;
vec3 u_lightDiffuseIntensity = vertexColor;
vec3 u_lightSpecularIntensity = vec3(0.1, 0.1, 0.1);

// parameters of the material and possible values
vec3 u_matAmbientReflectance = vec3(0.55);
vec3 u_matDiffuseReflectance = vec3(0.55);
vec3 u_matSpecularReflectance = vec3(0.1, 0.1, 0.1);
float u_matShininess = 125;

float near = near_far_zero[0];
float far = near_far_zero[1];
float orthoPlane = -1 + near + 0.11; // where it is and adjust size
float zoff = 0;//0.2; // avoid being clipped out
////////////////////////////////////////////////////

// returns intensity of reflected ambient lighting
vec3 ambientLighting(){
   return u_matAmbientReflectance * u_lightAmbientIntensity;
}

// returns intensity of diffuse reflection
vec3 diffuseLighting(in vec3 N, in vec3 L){
   // calculation as for Lambertian reflection
   float diffuseTerm = clamp(dot(N, L), 0, 1) ;
   return u_lightDiffuseIntensity * u_matDiffuseReflectance * diffuseTerm;
}

// returns intensity of specular reflection
vec3 specularLighting(in vec3 N, in vec3 L, in vec3 V){
   float specularTerm = 0;

   // calculate specular reflection only if
   // the surface is oriented to the light source
   if(dot(N, L) > 0){
      // half vector
      vec3 H = normalize(L + V);
      specularTerm = pow(dot(N, H), u_matShininess);
   }
   return u_matSpecularReflectance * u_lightSpecularIntensity * specularTerm;
}

///////////////////////////////////////////////////////////////////

vec3 getArcballVector(vec2 cursor) {
    vec3 P = vec3(cursor[0] * screenDimensionsX2Pixel - 1.0, cursor[1] * screenDimensionsY2Pixel - 1.0, 0);
    P.y = -P.y;
    float OP_squared = P.x * P.x + P.y * P.y;
    if (OP_squared <= 1) {
        P.z = sqrt(1 - OP_squared); // Pythagoras
    } else {
        P = normalize(P); // nearest point
    }
    return P;
}

mat4 rotationMatrix(float angle, vec3 a) {
    vec3 axis = normalize(a);
    float degree = degrees(angle);
    float s = sin(degree);
    float c = cos(degree);
    float oc = 1.0 - c;

    float x = axis.x, y = axis.y, z = axis.z;
    float ocx = oc * x;
    float ocy = oc * y;


    return mat4(ocx * x + c, ocx * y - z * s, ocx * z + y * s, 0.0,
        ocx * y + z * s, ocy * y + c, ocy * z - x * s, 0.0,
        ocx * z - y * s, ocy * z + x * s, oc * z * z + c, 0.0,
        0.0, 0.0, 0.0, 1.0);
}

mat4 getInterationRotation(vec2 lastCursor, vec2 currCursor){
    vec3 va = getArcballVector(lastCursor);
    vec3 vb = getArcballVector(currCursor);

    float angle = acos(min(1.0f, dot(va, vb) ) );
    mat4 v = rotationReferViewMatrix;
    vec3 axis_in_camera_coord = cross(va, vb);
    mat3 m2 = mat3(v) * mat3(m);
    mat3 camera2object = inverse(m2);
    vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;

    return rotationMatrix(angle * 0.2, axis_in_object_coord);
}

mat4 getOrthoProjectionMatrix(mat4 op, float scale, int respect){
     if(respect == 0){ // aspect ratio = 1, not compress depth
        return mat4( op[0][0] / near, 0.0f, 0.0f, 0.0f,
                    0.0f,  op[0][0] / near, 0.0f, 0.0f,
                    0.0f, 0.0f, -2.f / (far - near), 0.0f,
                    -1.0f * op[2][0], -1.0f * op[2][0], -1.0 * (far + near) / (far - near), 1.0f);
    }
    else if(respect == 1){ // respect aspect rati, not compress depth
        return mat4( op[0][0] / near, 0.0f, 0.0f, 0.0f,
                    0.0f, op[1][1] / near, 0.0f, 0.0f,
                    0.0f, 0.0f, -2.f / (far - near), 0.0f,
                    -1.0f * op[2][0], -1.0f * op[2][1], -1.0 * (far + near) / (far - near), 1.0f);
    }else if(respect == 2){ // aspect ratio = 1, compress depth
        float near_ = near;
        float far_ = far * 10 ;
        return mat4( op[0][0] / near_, 0.0f, 0.0f, 0.0f,
                    0.0f, op[0][0] / near_, 0.0f, 0.0f,
                    0.0f, 0.0f, -2.f / (far_ - near_), 0.0f,
                   -1.0f * op[2][0], -1.0f * op[2][0], -1.0 * far_ / (far_ - near_), 1.0f);
    }
}

vec3 getPosition(mat4 mat){
    return vec3(mat[3][0], mat[3][1], mat[3][2]);
}

vec4 getRotation(mat4 mat){
    vec4 q;
    q.w = sqrt(max(0, 1 + mat[0][0] + mat[1][1] + mat[2][2])) / 2.0f;
    q.x = sqrt(max(0, 1 + mat[0][0] - mat[1][1] - mat[2][2])) / 2.0f;
    q.y = sqrt(max(0, 1 - mat[0][0] + mat[1][1] - mat[2][2])) / 2.0f;
    q.z = sqrt(max(0, 1 - mat[0][0] - mat[1][1] + mat[2][2])) / 2.0f;
    q.x = sign(mat[2][1] - mat[1][2]) * abs(q.x);
    q.y = sign(mat[0][2] - mat[2][0]) * abs(q.y);
    q.z = sign(mat[1][0] - mat[0][1]) * abs(q.z);
    return q;
}


/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
float getAttenuation(vec3 toCamera){
    float c1 = 0.5;
    float c2 = 0.12;
    float c3 = 0.12;
    float d = distance(toCamera, vec3(0)) + 0.3;
    float attenuation = 1.0f / (c1 + c2 * d + c3 * d * d);
    return clamp(attenuation, 0.0, 1.0);
}

void main(void) {
    // 0: stereoscopy, 1:motion, 2:perspective, 3: shading (0, 1, 2)
    o_factors = factors;

    o_what = what;

    o_screenDimensions = screenDimensions;

    o_depthMapResolution = depthMapResolution;

    o_near_far_zero = near_far_zero;

    mat4 t = mat4(1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);

    mat4 r_f = r;

    if (factors[0] > 0.9 && factors[0] < 1.1 && factors[1] > 0.9 && factors[1] < 1.1) { // VR and motion
        t[3][1] = people_height;
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////// show frames, be careful that these have to be consistent with data one
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 o_referFramesColor = referFramesColor;

 if (what > 2.5 && what < 3.5) {

     vec4 worldSpace = t * vec4(referFramesVertex, 1.0f);

     if (factors[2] > -0.1 && factors[2] < 0.1) { // orthographic projection
         mat4 p_o = getOrthoProjectionMatrix(headProjectionMatrix, otho_ScalingFactor, 0);
         vec4 p2 =  p_o * headViewMatrix * worldSpace;
         p2 = p2 / p2.w;
         float old_z = p2.z;
         p2.z = orthoPlane;

         vec4 p3;
         if(factors[0] > 0.9 && factors[0] < 1.1){ // VR
            p3 = eyeProjection * eyePose * revertCameraPosMatrix * p2;
         }else if(factors[0] > -0.1 && factors[0] < 0.1){ // screen
            p3 = headProjectionMatrix * revertCameraPosMatrix * p2;
         }

         p3 = p3 / p3.w;
         p3.z = old_z + zoff;
         gl_Position = p3;

     } else if (factors[2] > 0.9 && factors[2] < 1.1) { // linear perspective
          vec4 pilot = headViewMatrix * worldSpace;

          if(factors[0] > 0.9 && factors[0] < 1.1){ // VR
              gl_Position = eyeProjection * eyePose * pilot;
          }else if(factors[0] > -0.1 && factors[0] < 0.1){ // screen
              gl_Position = headProjectionMatrix * pilot;
          }
     }


 } else if (what > -1.5 && what < 0.5) { // depth or regular, data or skybox
        vec3 normal_f = normal;

        ////////////////////////////////////////////////////////////
        ////// motion
        ////////////////////////////////////////////////////////////
        if (factors[1] > 0.9 && factors[1] < 1.1) {

            if (needRotationUpdate == true)
                 r_f = getInterationRotation(vec2(cursors[0], cursors[1]), vec2(cursors[2], cursors[3])) * r;

            normal_f = vec3(r_f * vec4(normal, 1.0));
        }

        // top level
        vec4 worldSpace = t * r_f * m * vec4(vertex, 1.0f);
        if((what  > -0.5 && what < -0.3) || (what  > 0.3 && what < 0.5)){ // skybox
            worldSpace = t * vec4(skyBoxVertex, 1.0f);
        }

        ////////////////////////////////////////////////////////////
        ////// orthographic projection
        ////////////////////////////////////////////////////////////

//        1. P2 = orthographic (P1)
//        P2 = P2/P2.w
//        old_z = P2.z
//        P2.z = constant
//        at the end of the shader : P3 = persp(P2)
//        P3 = P3/P3.w
//        P3.z = old_z
//        return P3

        vec3 toLight;
        if (factors[2] > -0.1 && factors[2] < 0.1) {
                mat4 p_o = getOrthoProjectionMatrix(headProjectionMatrix, otho_ScalingFactor, 0);
                vec4 p2 =  p_o * headViewMatrix * worldSpace;
                p2 = p2 / p2.w;
                float old_z = p2.z;
                p2.z = orthoPlane;

                vec4 p3;
                if(factors[0] > 0.9 && factors[0] < 1.1){ // VR
                   p3 = eyeProjection * eyePose * revertCameraPosMatrix * p2;                                                                                                    p2;
                }else if(factors[0] > -0.1 && factors[0] < 0.1){ // screen
                   p3 = headProjectionMatrix * revertCameraPosMatrix * p2;
                }

                p3 = p3 / p3.w;
                p3.z = old_z + zoff; // does nothing except to avoid clipping
                gl_Position = p3;

            ////////////////////////////////////////////////////////////
            ////// linear perspective
            ////////////////////////////////////////////////////////////
            } else if (factors[2] > 0.9 && factors[2] < 1.1) {
                vec4 pilot = headViewMatrix * worldSpace;
                // direction to camera            
                if(factors[0] > 0.9 && factors[0] < 1.1){ // VR
                    gl_Position =  eyeProjection * eyePose * pilot;
                }else if(factors[0] > -0.1 && factors[0] < 0.1){ // screen
                    gl_Position =  headProjectionMatrix * pilot;
                }
            }

            o_toCamera = cameraPosition - worldSpace.xyz;
            toLight = cameraPosition - worldSpace.xyz; // original??/

            // AO shading
            vec4 p = vec4(gl_Position);
            vec2 uv = vec2(p.x / p.w, p.y / p.w);
            o_texDepthCoord = vec2(uv.x * 0.5 + 0.5, uv.y * 0.5 + 0.5);

            // normal in world space
            vec3 N = normalize(normal_f);
            vec3 L = normalize(toLight);

            o_att = getAttenuation(toLight);
            // output color
            o_vertexColor = vertexColor;

            // get Blinn-Phong reflectance components
            o_Iamb = ambientLighting();
            o_Idif = diffuseLighting(N, L);

     //////////////////////////////////////////////////////////
     ////// show text
     //////////////////////////////////////////////////////////
    } else if (what > .5 && what < 1.5) { // show text
         t[3][2] = -far;
         mat4 p_o = getOrthoProjectionMatrix(headProjectionMatrix, otho_ScalingFactor, 0);
         vec4 worldSpace;

         if(factors[0] > -0.1 && factors[0] < 0.1){ // screen
             worldSpace = t * vec4(canvasVertex, 1.0f);
             vec4 v =  p_o * headViewMatrix * worldSpace;
             gl_Position = headProjectionMatrix * revertCameraPosMatrix * v;
         }else if(factors[0] > 0.9 && factors[0] < 1.1){ //VR
              t[3][1] = -4 * people_eye;
              worldSpace = t * vec4(canvasVertex, 1.0f);
              vec4 v =  p_o * headViewMatrix * worldSpace;
              gl_Position =  eyeProjection * eyePose * revertCameraPosMatrix * v;
         }

        o_canvasUV = canvasUV;
        o_canvas = canvasVertex;

    //////////////////////////////////////////////////////////
    ////// show cursor
    //////////////////////////////////////////////////////////
    }else if (what > 1.5 && what < 2.5) {

        vec4 worldSpace;
        mat4 p_o = getOrthoProjectionMatrix(headProjectionMatrix, otho_ScalingFactor, 0);

        if (factors[0] > 0.9 && factors[0] < 1.1) { // VR
            worldSpace =  controllerPosition * vec4(cursorVertex, 1.0f);

            if(factors[2] > -0.1 && factors[2] < .1){  // orthographic
               vec4 p2 =  p_o * headViewMatrix * worldSpace;
               p2 = p2 / p2.w;
               float old_z = p2.z;
               p2.z = orthoPlane;
               vec4 p3 = eyeProjection * eyePose * revertCameraPosMatrix * p2;

               p3 = p3 / p3.w;
               p3.z = old_z + zoff;
               gl_Position = p3;
            }else if(factors[2] > .9 && factors[2] < 1.1){ // linear
               gl_Position = eyeProjection * eyePose * headViewMatrix * worldSpace;
            }
        } else if (factors[0] > -0.1 && factors[0] < 0.1) { // screen
            float xOff = cursorPositionOnScreen.x - screenDimensions.x / 2.0;
            float yOff = cursorPositionOnScreen.y - screenDimensions.y / 2.0;

            xOff = xOff * magic_VR_screenCoordinatesScaling;
            yOff = yOff * magic_VR_screenCoordinatesScaling;

            mat4 t_cursor = mat4(
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                xOff, -yOff, 1.f, 1);

            mat4 r_cursor = rotationMatrix(degrees(90), vec3(1,0,0));

            worldSpace = t_cursor * r_cursor * vec4(cursorVertex, 1.0f);


            vec4 p2 =  p_o * headViewMatrix * worldSpace;
            p2 = p2 / p2.w;
            float old_z = p2.z;
            p2.z = orthoPlane;

            vec4 p3 = headProjectionMatrix * revertCameraPosMatrix * p2;

            p3 = p3 / p3.w;
            p3.z = old_z + zoff;
            gl_Position = p3;
        }

 }else if (what > 3.5 && what < 4.5){ // home position
     vec4 worldSpace = t * vec4(homeVertex, 1.0f);
     gl_Position = eyeProjection * eyePose * headViewMatrix * worldSpace;
 }else if (what > 4.5 && what < 5.5){ // floor position
     vec4 worldSpace = t * vec4(floorFramesVertex, 1.0f);
     gl_Position = eyeProjection * eyePose * headViewMatrix * worldSpace;
 }
} // main
