#version 450
/////////////////////////////////////////////////////////
uniform sampler2D  canvasTexture;
/////////////////////////////////////////////////////////
in vec2 o_canvasUV;
in vec3 o_canvasVertex;
in vec4 o_factors;
in float o_what;
// data from vertex shader
in vec3	o_toCamera;
in vec3 o_vertexColor;
in vec2 o_screenDimensions;
in vec2 o_depthMapResolution;
in vec4 o_referFramesColor;
in vec2 o_texDepthCoord;
in vec3 o_near_far_zero;
in vec3 o_Idif;
in vec3 o_Iamb;
in float o_att;
//in mat4 o_PerspToView;
/////////////////////////////////////////////////////////
// color for framebuffer
out vec4 resultingColor;
/////////////////////////////////////////////////////////
uniform sampler2D depthTexture;
uniform sampler2D testTexture;
//uniform sampler2D geometryTexture;
/////////////////////////////////////////////////////////


float zNear = o_near_far_zero[0];
float zFar = o_near_far_zero[1];

float zNear2 = zNear * 2;
float zFarPluszNear = zFar + zNear;
float zFarMinuszNear = zFar - zNear;
float mw = o_depthMapResolution.x;
float mh = o_depthMapResolution.y;
float pixelw = 1.0/mw;
float pixelh = 1.0/mh;
float hw = mw/2.0;
float hh = mh/2.0;
float PI =   3.14159265f;
vec2 texCoord = vec2(o_texDepthCoord.x , o_texDepthCoord.y);
float depthCoordX = o_texDepthCoord.x > 0 && o_texDepthCoord.x < 1 ? o_texDepthCoord.x : 0;
float depthCoordY = o_texDepthCoord.y > 0 && o_texDepthCoord.y < 1 ? o_texDepthCoord.y : 0;

////////////////////////////////////////////////////
const float MAX_KERNEL_SIZE = 16;
const float total_strength = 1.0;
const float radius = 5;

float miststart = 0.0; //mist start
float mistend = o_near_far_zero[1]; //mist end

float aoclamp = 0.1; //depth clamp - reduces haloing at screen edges
bool noise = true; //use noise instead of pattern for sample dithering
float noiseamount = 0.0003; //dithering amount

float diffarea = 0.3; //self-shadowing reduction
float gdisplace = 0.4; //gauss bell center //0.4
float garea = 2; //gauss bell width
float depthDiffScaler = 100;
float delta = 0.0001;
float depthThreshold = 20;

float doMist()
{
  float zdepth = texture2D(depthTexture,texCoord).r;
  float depth = -zFar * zNear / (zdepth * (zFar - zNear) - zFar);
  return clamp((depth-miststart)/mistend,0.0,1.0);
}

vec2 rand(vec2 coord) //generating noise/pattern texture for dithering
{
//  float noiseX = ((fract(1.0-coord.s*hw)*0.25)+(fract(coord.t*hh)*0.75))*2.0-1.0;
//  float noiseY = ((fract(1.0-coord.s*hw)*0.75)+(fract(coord.t*hh)*0.25))*2.0-1.0;

  float noiseX = clamp(fract(sin(dot(coord,vec2(12.9898,78.233))) * 43758.5453),0.0,1.0)*2.0-1.0;
  float noiseY = clamp(fract(sin(dot(coord,vec2(12.9898,78.233)*2.0)) * 43758.5453),0.0,1.0)*2.0-1.0;

  return vec2(noiseX,noiseY) * noiseamount;
}



float readDepth(vec2 coord){
 if(texture2D(depthTexture, coord).a < 0.9){
     return 1;
 }else{
     float z = texture2D(depthTexture, coord).r;
     float z_n = 2.0 * z - 1.0;
     return zNear2 / (zFarPluszNear - z_n * zFarMinuszNear);
 }
}

vec2 compareDepthFar(float depth1, float depth2){
    float area = garea;
    float diff = (depth1 - depth2) * depthDiffScaler; //depth difference (0-100)

    // magic operations to prevent black halo when depth diff is big
    if(abs(diff) > 2)
        diff /= 4;

    float diffG = diff - gdisplace;
    //reduce left bell width to avoid self-shadowing
    float far = 1;
    if (diffG < 0){
      area = diffarea;
      far = 0;
    }

     float gauss = pow(2.71828182846,-2.0*diffG * diffG/(area*area));
     return vec2(gauss, far);

}


float calAO(float depth, float ddw, float ddh){
  float temp = 1;
      vec2 coord1 = vec2(depthCoordX + ddw, depthCoordY + ddh);
      vec2 gaussFar= compareDepthFar(depth, readDepth(coord1));
      temp = gaussFar[0];
      if (gaussFar[1] > 0){
        vec2 coorda = vec2(depthCoordX - ddw, depthCoordY - ddh);
        float temp2 = compareDepthFar(readDepth(coorda),depth)[0];
        temp += (1.0-temp)*temp2;
      }
  return temp;
}




float dws16[16] = float[16](0.1767766952966369, -0.22577979282638214, 0.03458701007725054, 0.28453027372165623, -0.5222095951380513, 0.4947532383785339, -0.16560172116300356, -0.3154050676060234, 0.6845685825311724, -0.7123533443287733, 0.3436242695323877, 0.2534030290286435, -0.7645502647498938, 0.8972281608231754, -0.5479077316191072, -0.12594838746423043);
float dhs16[16] = float[16](0.0, 0.20681751654846825, -0.3937686360464939, 0.3712041531770346, -0.09245073685889445, -0.3145937588604606, 0.6154884807596733, -0.6076756069881691, 0.2502316043413811, 0.29377323367456754, -0.733602318281733, 0.8090345511034177, -0.44352327184812673, -0.19680352493251255, 0.7784902810131957, -0.9761593126611865);

float dws32[32] = float[32](0.125, -0.15965042256242865, 0.024456709366591313, 0.20119328600144767, -0.3692579459227976, 0.34984336987146575, -0.11709810001052362, -0.22302506212482062, 0.48406308689505473, -0.5037098803757913, 0.2429790511666253, 0.17918300019936534, -0.5406186767626201, 0.6344361167896015, -0.3874292724924096, -0.08905895885546808, 0.5487994101704881, -0.7388974266836175, 0.5393099866760925, -0.03660296685249369, -0.512365466938843, 0.8122749510155172, -0.6886978264466193, 0.18879306068927576, 0.43436312041726255, -0.8501986417562586, 0.8264669202042259, -0.3587370213617865, -0.31849287437589835, 0.8494318317810496, -0.9443085741983264, 0.5375467219085146);
float dhs32[32] = float[32](0.0, 0.14624206841958293, -0.27843647276705347, 0.2624809739160911, -0.06537254295861737, -0.22245138020919722, 0.43521607848737087, -0.4296915424629858, 0.17694046429697968, 0.20772904566238695, -0.5187351739511853, 0.572073817299441, -0.313618313137855, -0.1391611070411954, 0.5504757567922516, -0.6902488695011242, 0.46308121036868305, 0.030093069639505917, -0.5359755015590105, 0.7797661334128299, -0.6149037553052846, 0.10992908601793745, 0.4783516529162435, -0.8359020159298429, 0.7595746702078596, -0.2720979043576286, -0.3808903120429052, 0.8547998300798106, -0.8883621384164109, 0.4475941947312212, 0.24780297959371636, -0.8339175749229717);


float dws24[24] = float[24](0.14433756729740657, -0.18434842888531136, 0.028240175472587867, 0.2323179956641623, -0.4263823489578708, 0.403964327538993, -0.13521323912533903, -0.2575271593075963, 0.5589479070472418, -0.5816340700568743, 0.2805680411969818, 0.2069027067986166, -0.6242526771156759, 0.7325837256241944, -0.44736478953086695, -0.10283642773790459, 0.6336989743860781, -0.8532052563986164, 0.6227415319681909, -0.04226546553085235, -0.5916286805212185, 0.9379343232495978, -0.7952397509785315, 0.2179994488201734);
float dhs24[24] = float[24](0.0, 0.16886579513778774, -0.3215107450085363, 0.3030869218952206, -0.0754857105495362, -0.2568647284907675, 0.5025442401406737, -0.49616505541902056, 0.20431324938479709, 0.2398648408633662, -0.5989837846376881, 0.6605739448283379, -0.36213523502587375, -0.16068940522192093, 0.6356346527327387, -0.7970307412286176, 0.5347201229260337, 0.03474848371422178, -0.6188912002082787, 0.9003963740617018, -0.7100296973024356, 0.12693517480845143, 0.5523529111569911, -0.9652165078264913);

float getAO(vec2 uv){
    vec2 noise = rand(uv);
    float depth = readDepth(uv);
    float cDepth = clamp(depth,aoclamp,1.0);

    float w = pixelw/cDepth+(noise.x*(1.0-noise.x));
    float h = pixelh/cDepth+(noise.y*(1.0-noise.y));

    float pw = 0.0;
    float ph = 0.0;
    float ao = 0.0;
    float dd = (1.0-depth)*radius;
    float ddw = dd * w;
    float ddh = dd * h;

    for (int i = 0; i < MAX_KERNEL_SIZE; i++)
    {
      pw = dws16[i] * ddw;
      ph = dhs16[i] * ddh;

      ao += calAO(depth, pw, ph);
      //increase sampling area:
    }

    ao /= MAX_KERNEL_SIZE;
    ao = 1.0 - ao;

    return ao;
}

void main(void){
    // shading - flat
   if(o_what > -1.5 && o_what < -0.3){ // depth pass
//       if (o_factors[2] > -0.1 && o_factors[2] < 0.1) {
//           float d = distance(vec3(o_toCamera.xyz), vec3(0));
//           d = 1 - (1.f / zFarMinuszNear)* (1.f / d);
//           resultingColor.rgb = vec3(d, d, d);
//       }else{
//           resultingColor.rgb = vec3(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z);
//      }
           float d = distance(vec3(o_toCamera.xyz), vec3(0));
           d = 1 - (1.f / zFarMinuszNear)* (1.f / d);
           resultingColor.rgb = vec3(d, d, d);

           resultingColor.a = abs(o_what);

   }else if(o_what > -.1 && o_what < 0.1){ // data
        if(o_factors[3] > -0.5 && o_factors[3] < 0.5){
            resultingColor.rgb = o_vertexColor;
            // shading 1
        } else if(o_factors[3] > 0.5 && o_factors[3] < 1.5){

            // combination of all components and diffuse color of the object
            resultingColor.rgb = clamp(o_Iamb + o_Idif, 0, 1);
            resultingColor.a = 1;

         } else if(o_factors[3] > 1.5 && o_factors[3] < 2.5){
            float ao, deltax = 1.0 / mw, deltay = 1.0 / mh;
            float d = texture2D(depthTexture, texCoord).r;
            ao = getAO(texCoord);
            ao = pow(ao, 1.35);
            // get Blinn-Phong reflectance components
            vec3 Idif = clamp(o_Idif, 0, 1);
            vec3 Iamb = clamp(ao * o_Iamb, 0, 1);

            resultingColor.rgb = clamp(Iamb + Idif, 0, 1);
//            resultingColor.rgb = vec3(ao, ao, ao);
            resultingColor.a = 1;
        }

    }else if(o_what > 0.6 && o_what < 1.5){//text texture
        resultingColor.rgb = texture2D(canvasTexture, o_canvasUV).rgb;
        resultingColor.a = 1.f;

    }else if(o_what > 1.6 && o_what < 2.5 || o_what > 3.5 && o_what < 4.5){ // cursor or home position

        resultingColor.rgb = vec3(0.8,0.8,0.8);
        resultingColor.a = 1;

    } else if (o_what > 2.5 && o_what < 3.5 || o_what > 4.5 && o_what < 5.5) {// reference frame ot floor frame
       resultingColor.rgb = o_referFramesColor.rgb;
       resultingColor.a = 1;
    } else {
       resultingColor = vec4(1, 0, 0, 1);
    }

/*   float z = abs(o_z.z);
   resultingColor.rgb = vec3(z, z, z);
   resultingColor.a = 1*/;
}
