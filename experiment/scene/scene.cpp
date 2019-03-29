#include <QDir>
#include <QDateTime>
#include <QOpenGLShaderProgram>
#include <QGuiApplication>
#include <QBitmap>
#include <QOpenGLPaintDevice>
#include <QImage>
#include <QThread>
#include <QDateTime>
#include <QOpenGLFunctions_4_3_Core>
#include <QRandomGenerator>
#include "scene.h"
#include "glm/ext.hpp"
#include "glm/gtx/transform.hpp"
#include "settings.h"
#include <omp.h>

#define PI         3.14159265
#define TWOPI      6.28318531
#define HALFPI     1.570795
#define FOURPI     12.5663706144
#define EPSILON    0.0001

Scene::Scene() :
    m_vvbo(new QOpenGLBuffer),
    m_tvbo(new QOpenGLBuffer),
    m_cursorvbo(new QOpenGLBuffer),
    m_rvbo(new QOpenGLBuffer),
    m_skyvbo(new QOpenGLBuffer),
    m_homevbo(new QOpenGLBuffer),
    m_floorvbo(new QOpenGLBuffer),
    m_leftBuffer(nullptr),
    m_rightBuffer(nullptr),
    m_resolveBuffer(nullptr),
    m_depthResolvedBuffer(nullptr),
    m_depthBuffer(nullptr),
    m_modelLoader(new ModelLoader ),
    m_viewMatrix(glm::mat4x4() ),
    m_poseMatrix(glm::mat4x4() ),
    m_inputAnswer(0),
    m_projectionMatrix(glm::mat4x4() ),
    m_shader(new QOpenGLShaderProgram ),
    m_depthShader(new QOpenGLShaderProgram ),
    m_frame(0),
    m_pass(RenderPass::HOLDER),
    factor_stereoscopy(-1),
    factor_motion(-1),
    factor_shading(-1),
    factor_perspective(-1),
    m_trial(nullptr),
    m_textImage(nullptr),
    m_textTexture(nullptr),
    m_depthTexture(nullptr),
    m_qrand(QRandomGenerator() ),
    m_deviceRatio(1),
    m_remark(""),
    m_sphere(QVector<GLfloat>() ),
    m_referFrames(QVector<GLfloat>() ),
    m_floorFrames(QVector<GLfloat>() ),
    m_cursorModel(QVector<GLfloat>() ),
    m_homePosition(QVector<GLfloat>() ),
    m_skybox(QVector<GLfloat>() ),
    m_sphereModelMatricesGLM(QVector<glm::mat4>() ),
    m_sphereModelMatrices(QVector<QMatrix4x4>() ),
    m_sphereLastRotMatrices(QVector<QMatrix4x4>() ),
    m_sphereLeftRightLastRotMatrices(QVector<QMatrix4x4>() ),
    m_sphereColors(QVector<QVector3D>() ){
}


Scene::~Scene() {
    if (m_leftBuffer && ( m_leftBuffer != nullptr ) ) {
        delete m_leftBuffer;
    }
    if (m_rightBuffer && ( m_rightBuffer != nullptr ) ) {
        delete m_rightBuffer;
    }
    if (m_resolveBuffer && ( m_resolveBuffer != nullptr ) ) {
        delete m_resolveBuffer;
    }
    if (m_depthMap && ( m_depthMap != nullptr ) ) {
        delete m_depthMap;
    }

    if (m_depthResolvedBuffer && ( m_depthResolvedBuffer != nullptr ) ) {
        delete m_depthResolvedBuffer;
    }

    m_shader->bind();

    m_cursorvbo->destroy();
    delete m_cursorvbo;

    m_vvbo->destroy();
    delete m_vvbo;

    m_skyvbo->destroy();
    delete m_skyvbo;

    m_homevbo->destroy();
    delete m_homevbo;

    m_floorvbo->destroy();
    delete m_floorvbo;

    m_vao.destroy();
    m_cursorvao.destroy();
    m_skyvao.destroy();

    delete m_modelLoader;
}

void Scene::buildReferFrames(){
    m_referFrames.clear();
    float div = 15;
    float s = S.conf_floorPosition, e = -s;
    float step = ( e - s ) / div;
    m_numReferFramesVertices = 0;

    // X-Y
    for (int i = 0; i < div + 1; i++) {
        m_referFrames.append(s + step * i);
        m_referFrames.append(s);
        m_referFrames.append(s);

        m_referFrames.append(s + step * i);
        m_referFrames.append(e);
        m_referFrames.append(s);
        m_numReferFramesVertices += 2;
    }

    for (int i = 0; i < div + 1; i++) {
        m_referFrames.append(s);
        m_referFrames.append(s + step * i);
        m_referFrames.append(s);

        m_referFrames.append(e);
        m_referFrames.append(s + step * i);
        m_referFrames.append(s);

        m_numReferFramesVertices += 2;
    }

    // X-Z
    for (int i = 0; i < div + 1; i++) {
        m_referFrames.append(s);
        m_referFrames.append(s);
        m_referFrames.append(s + step * i);

        m_referFrames.append(e);
        m_referFrames.append(s);
        m_referFrames.append(s + step * i);

        m_numReferFramesVertices += 2;
    }

    for (int i = 0; i < div + 1; i++) {
        m_referFrames.append(s + step * i);
        m_referFrames.append(s);
        m_referFrames.append(s);

        m_referFrames.append(s + step * i);
        m_referFrames.append(s);
        m_referFrames.append(e);

        m_numReferFramesVertices += 2;
    }

    // Y-Z
    for (int i = 0; i < div + 1; i++) {
        m_referFrames.append(s);
        m_referFrames.append(s + step * i);
        m_referFrames.append(s);

        m_referFrames.append(s);
        m_referFrames.append(s + step * i);
        m_referFrames.append(e);

        m_numReferFramesVertices += 2;
    }

    for (int i = 0; i < div + 1; i++) {
        m_referFrames.append(s);
        m_referFrames.append(s );
        m_referFrames.append(s + step * i);

        m_referFrames.append(s);
        m_referFrames.append(e);
        m_referFrames.append(s + step * i);

        m_numReferFramesVertices += 2;
    }

}


void Scene::buildFloorFrames(){
    m_floorFrames.clear();
    float div = 15;
    float s = S.conf_floorPosition, e = -s;
    float step = ( e - s ) / div;
    m_numFloorFramesVertices = 0;

    // X-Z
    for (int i = 0; i < div + 1; i++) {
        m_floorFrames.append(s);
        m_floorFrames.append(s);
        m_floorFrames.append(s + step * i);

        m_floorFrames.append(e);
        m_floorFrames.append(s);
        m_floorFrames.append(s + step * i);

        m_numFloorFramesVertices += 2;
    }

    for (int i = 0; i < div + 1; i++) {
        m_floorFrames.append(s + step * i);
        m_floorFrames.append(s);
        m_floorFrames.append(s);

        m_floorFrames.append(s + step * i);
        m_floorFrames.append(s);
        m_floorFrames.append(e);

        m_numFloorFramesVertices += 2;
    }



}
void Scene::buildTextCanvas(){

    int w = S.fixed_windowWidth * S.magic_textPlaneScaleFactor;
    int h = S.fixed_windowHeight * S.magic_textPlaneScaleFactor;

    m_textCanvas.clear();
    m_textCanvas.append(-w);
    m_textCanvas.append(h);
    m_textCanvas.append(S.magic_textPlaneZ);

    m_textCanvas.append(0);
    m_textCanvas.append(0);

    //
    m_textCanvas.append(-w);
    m_textCanvas.append(-h);
    m_textCanvas.append(S.magic_textPlaneZ);

    m_textCanvas.append(0);
    m_textCanvas.append(1);

    //
    m_textCanvas.append(w);
    m_textCanvas.append(h);
    m_textCanvas.append(S.magic_textPlaneZ);

    m_textCanvas.append(1);
    m_textCanvas.append(0);

    //
    m_textCanvas.append(w);
    m_textCanvas.append(-h);
    m_textCanvas.append(S.magic_textPlaneZ);

    m_textCanvas.append(1);
    m_textCanvas.append(1);

    m_numTextPlaneVertices = 4;

}

void Scene::buildHomePosition()
{
    m_homePosition.clear();

    float x = S.camera_cameraDataPosition.x;
    float y = S.camera_cameraDataPosition.y;
    float z = S.camera_cameraDataPosition.z;
    float f = S.conf_floorPosition;

    float size = 0.4;
    m_homePosition.append(x - 1.8 * size + size);
    m_homePosition.append(f + 0.001);
    m_homePosition.append(z - 1.8 * size+ size);

    m_homePosition.append(x - size + size);
    m_homePosition.append(f + 0.001);
    m_homePosition.append(z + size + size);

    m_homePosition.append(x + size + size);
    m_homePosition.append(f + 0.001);
    m_homePosition.append(z - size + size);

//    m_homePosition.append(x + size);
//    m_homePosition.append(f + 0.001);
//    m_homePosition.append(z + size);
}

void Scene::buildSkybox(){

    float radius = 10;

    float points[] = {
    -radius,  radius, -radius,
    -radius, -radius, -radius,
    radius, -radius, -radius,
    radius, -radius, -radius,
    radius,  radius, -radius,
    -radius,  radius, -radius,

    -radius, -radius,  radius,
    -radius, -radius, -radius,
    -radius,  radius, -radius,
    -radius,  radius, -radius,
    -radius,  radius,  radius,
    -radius, -radius,  radius,

    radius, -radius, -radius,
    radius, -radius,  radius,
    radius,  radius,  radius,
    radius,  radius,  radius,
    radius,  radius, -radius,
    radius, -radius, -radius,

    -radius, -radius,  radius,
    -radius,  radius,  radius,
    radius,  radius,  radius,
    radius,  radius,  radius,
    radius, -radius,  radius,
    -radius, -radius,  radius,

    -radius,  radius, -radius,
    radius,  radius, -radius,
    radius,  radius,  radius,
    radius,  radius,  radius,
    -radius,  radius,  radius,
    -radius,  radius, -radius,

    -radius, -radius, -radius,
    -radius, -radius,  radius,
    radius, -radius, -radius,
    radius, -radius, -radius,
    -radius, -radius,  radius,
    radius, -radius,  radius
    };

    for(auto v : points) {
        m_skybox.append(v);
    }

    m_numSkyBoxVertices = 36;
}

void Scene::buildTheSphere() {
    m_numSphereVertices = 0;
    double r = S.conf_pointSize;

    m_sphere.clear();

    double step_p1 = PI / S.conf_sphereDiv1;         // "height", where the axis is
    double step_p2 = 2.0 * PI / S.conf_sphereDiv2;     // the position on the circle

    for (int cp1 = 0; cp1 < S.conf_sphereDiv1; cp1++) {
        for (int cp2 = 0; cp2 <= S.conf_sphereDiv2; cp2++) {
            // the sphere is drawn a bar by a bar, both theta of "height" and "position" should change
            double ctheta = step_p2 * cp2, ntheta = step_p2 * cp2,
                   cphi = step_p1 * cp1, nphi = step_p1 * ( cp1 + 1 );

            // vertex 1
            double z1 = r * cos(ctheta) * sin(cphi),
                   y1 = r * cos(cphi),
                   x1 = r * sin(ctheta) * sin(cphi);

            // vertex 2
            double z2 = r * cos(ntheta) * sin(nphi),
                   y2 = r * cos(nphi),
                   x2 = r * sin(ntheta) * sin(nphi);

            double x1n = x1, y1n = y1, z1n = z1,
                   x2n = x2, y2n = y2, z2n = z2;

            m_numSphereVertices += 2;

            m_sphere.append(x1);
            m_sphere.append(y1);
            m_sphere.append(z1);

            m_sphere.append(x1n);
            m_sphere.append(y1n);
            m_sphere.append(z1n);

            m_sphere.append(x2);
            m_sphere.append(y2);
            m_sphere.append(z2);

            m_sphere.append(x2n);
            m_sphere.append(y2n);
            m_sphere.append(z2n);

        }
    }
}

void Scene::setPass(RenderPass rp){
    m_pass = rp;
}
void Scene::loadCursorModel()
{
    m_modelLoader->loadVerticesNormals(S.conf_cursorModelPath, m_cursorModel);
    m_cursorModelNumVertices = m_cursorModel.length() / 6;
}

void Scene::buildTextInstruction(){
    m_textImage = new QImage(S.fixed_windowWidth, S.fixed_windowHeight, QImage::Format_RGBA64);
}

void Scene::setTrialControl(TrialControl & ts, QString status) {
    m_trial            = &ts;
    m_numDataPoints    = ts.num_points;
    m_remark           = ts.remark;

    m_interactionRotation = glm::mat4(1 );

    factor_stereoscopy = ts.factor_sterescopy == "yes" && S.canIRunVR ? 1 : 0;
    factor_motion      = ts.factor_motion == "yes" ? 1 : 0;
    factor_shading     = ( ts.factor_shading == "flat" || ts.factor_shading == "no" ) ? 0 : ( ts.factor_shading.contains("1") ? 1 : 2 );
    factor_perspective = ts.factor_perspective == "yes" ? 1 : 0;
    factor_dim = ts.remark  == "2d" ? 2 : 3;
    m_textIsReady = false;

    m_sphereModelMatrices.clear();
    m_sphereColors.clear();
    m_sphereModelMatricesGLM.clear();

    m_sphereLastRotMatrices.clear();
    m_sphereLeftRightLastRotMatrices.clear();

    float * * pd    = ( m_trial->remark == "" ) ? ( m_trial->p3d ) : ( m_trial->p2d );
    int   * labels = m_trial->labels;
    for(int i = 0; i < m_numDataPoints; i++) {
        int c = labels[i];
        QColor qc(S.fixed_tableau10[c]);
        int r, g, b;
        qc.getRgb(&r, &g, &b);
        QVector3D qvc = QVector3D(r / 255.0f, g / 255.0f, b / 255.0f);

        float x = pd[i][0];
        float y = pd[i][1];
        float z = m_trial->remark == "2d" ? 0 : pd[i][2];

        m_sphereModelMatrices.push_back(QMatrix4x4(1, 0, 0, x, 0, 1, 0, y, 0, 0, 1, z, 0, 0, 0, 1) );

        m_sphereModelMatricesGLM.push_back(glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, x, y, z, 1) );

        m_sphereLastRotMatrices.push_back(QMatrix4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) );

        if(factor_stereoscopy == 1) {
            m_sphereLeftRightLastRotMatrices.push_back(QMatrix4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) );
        }

        m_sphereColors.push_back(qvc);
    }
}


void Scene::setDeviceInformation(int fs, int fm ){
    factor_stereoscopy = fs;
    factor_motion      = fm;
}

void Scene::updateText(){
    m_textIsReady = false;
}

void Scene::cleanScene() {
    m_shader->bind();
    m_shader->removeAllShaders();
    m_vao.bind();
    m_vvbo->bind();
    m_vvbo->destroy();
    m_tvbo->bind();
    m_tvbo->destroy();
    m_skyvbo->bind();
    m_skyvbo->destroy();
    m_vao.destroy();
    m_skyvao.destroy();
    m_textIsReady = false;
    m_textTexture->release();

    if(m_resolveBuffer)
    m_resolveBuffer->release();

    if(m_leftBuffer)
    m_leftBuffer->release();

    if(m_rightBuffer)
    m_rightBuffer->release();

    if(m_textImage != nullptr)
    delete m_textImage;

}

void Scene::initScene() {

    buildTextInstruction();
    buildTheSphere();
    buildTextCanvas();
    buildSkybox();
    buildReferFrames();
    buildFloorFrames();
    buildHomePosition();
    loadCursorModel();
    m_headPosition = QVector3D(S.camera_cameraDataPosition.x, S.camera_cameraDataPosition.y, S.camera_cameraDataPosition.z);

    m_depthMapWidth = m_width * S.magic_depthMapScaleFactor;
    m_depthMapHeight = m_height * S.magic_depthMapScaleFactor;

    m_deviceRatio = static_cast<QGuiApplication *>( QCoreApplication::instance() )->devicePixelRatio();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    //   glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);

    m_shader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/resources/shaders/_shader.vert");
    m_shader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/resources/shaders/_shader.frag");

    m_shader->link();

    m_vertexAttributeLocation = m_shader->attributeLocation("vertex");
    m_normalAttributeLocation  = m_shader->attributeLocation("normal");

    m_skyboxVertexAttributeLocation = m_shader->attributeLocation("skyBoxVertex");
    m_referFrameAttributeLocation = m_shader->attributeLocation("referFramesVertex");
    m_floorFrameAttributeLocation = m_shader->attributeLocation("floorFramesVertex");

    m_canvasAttributeLocation  = m_shader->attributeLocation("canvasVertex");
    m_canvasUVAttributeLocation  = m_shader->attributeLocation("canvasUV");

    m_cursorAttributeLocation  = m_shader->attributeLocation("cursorVertex");
    m_cursorNormalAttributeLocation  = m_shader->attributeLocation("cursorNormal");
    m_cursorUVAttributeLocation  = m_shader->attributeLocation("cursorUV");

    m_homeVertexAttributeLocation  = m_shader->attributeLocation("homeVertex");

    m_cursorsLoc = m_shader->uniformLocation("cursors");
    m_vertexColorLoc = m_shader->uniformLocation ("vertexColor");
    m_rvLoc = m_shader->uniformLocation("rotationReferViewMatrix");
    m_mLoc = m_shader->uniformLocation("m");
    m_rLoc = m_shader->uniformLocation("r");
    m_whatLoc = m_shader->uniformLocation("what");
    m_headPositionLoc = m_shader->uniformLocation("headPosition");
    m_eyePoseLoc = m_shader->uniformLocation("eyePose");
    m_eyeProjectionLoc = m_shader->uniformLocation("eyeProjection");
    m_screenDimensionsLoc = m_shader->uniformLocation("screenDimensions");
    m_headViewMatrixLoc = m_shader->uniformLocation("headViewMatrix");
    m_headProjectionMatrixLoc = m_shader->uniformLocation("headProjectionMatrix");
    m_revertCamerPosMatrixLoc = m_shader->uniformLocation("revertCameraPosMatrix");
    m_people_heightLoc = m_shader->uniformLocation("people_height");
    m_cursorPositionOnScreenLoc =  m_shader->uniformLocation("cursorPositionOnScreen");
    m_controllerPositionLoc = m_shader->uniformLocation("controllerPosition");
    m_depthTextureLoc = m_shader->uniformLocation("depthTexture");
    m_depthMapResolutionLoc = m_shader->uniformLocation("depthMapResolution");
    m_factorsLoc = m_shader->uniformLocation("factors");
    m_near_far_zeroLoc = m_shader->uniformLocation("near_far_zero");
    m_canvasTextureLoc = m_shader->uniformLocation("canvasTexture");
    m_referFramesColorLoc = m_shader->uniformLocation("referFramesColor");
    m_cameraOriginalPositionLoc = m_shader->uniformLocation("cameraOriginalPosition");
    m_cameraPositionLoc = m_shader->uniformLocation("cameraPosition");
    m_dimLoc = m_shader->uniformLocation("dim");

    m_vao.create();
    m_vao.bind();


    //--------
    // sphere
    //--------
    m_vvbo->create();
    m_vvbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vvbo->bind();
    m_vvbo->allocate(m_sphere.data(), m_sphere.length() * sizeof( GLfloat ) );

    m_shader->enableAttributeArray("vertex");
    m_shader->setAttributeBuffer(m_vertexAttributeLocation, GL_FLOAT, 0, 3, 6 * sizeof( GLfloat ) );

    m_shader->enableAttributeArray("normal");
    m_shader->setAttributeBuffer(m_normalAttributeLocation, GL_FLOAT, 3 * sizeof( GLfloat ), 3, 6 * sizeof( GLfloat ) );

    m_vvbo->release();

    //--------
    // skybox
    //--------

    m_skyvbo->create();
    m_skyvbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_skyvbo->bind();
    m_skyvbo->allocate(m_skybox.data(), m_skybox.length() * sizeof( GLfloat ) );

    m_shader->enableAttributeArray("skyBoxVertex");
    m_shader->setAttributeBuffer(m_skyboxVertexAttributeLocation, GL_FLOAT, 0, 3, 3 * sizeof( GLfloat ) );
    m_skyvbo->release();

    //--------
    // home position
    //--------

    m_homevbo->create();
    m_homevbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_homevbo->bind();
    m_homevbo->allocate(m_homePosition.data(), m_homePosition.length() * sizeof( GLfloat ) );

    m_shader->enableAttributeArray("homeVertex");
    m_shader->setAttributeBuffer(m_homeVertexAttributeLocation, GL_FLOAT, 0, 3, 3 * sizeof( GLfloat ) );
    m_homevbo->release();

    //--------
    // refer frame
    //--------
    m_rvbo->create();
    m_rvbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_rvbo->bind();
    m_rvbo->allocate(m_referFrames.data(), m_referFrames.length() * sizeof( GLfloat ) );

    m_shader->enableAttributeArray("referFramesVertex");
    m_shader->setAttributeBuffer(m_referFrameAttributeLocation, GL_FLOAT, 0, 3);
    m_rvbo->release();

    //--------
    // floor frame
    //--------
    m_floorvbo->create();
    m_floorvbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_floorvbo->bind();
    m_floorvbo->allocate(m_floorFrames.data(), m_floorFrames.length() * sizeof( GLfloat ) );

    m_shader->enableAttributeArray("floorFramesVertex");
    m_shader->setAttributeBuffer(m_floorFrameAttributeLocation, GL_FLOAT, 0, 3);
    m_floorvbo->release();

    //--------
    // text instructions
    //--------
    m_tvbo->create();
    m_tvbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_tvbo->bind();
    m_tvbo->allocate(m_textCanvas.data(), m_textCanvas.length() * sizeof( GLfloat ) );

    m_shader->enableAttributeArray("canvasVertex");
    m_shader->setAttributeBuffer(m_canvasAttributeLocation, GL_FLOAT, 0, 3, 5 * sizeof( GLfloat ) );

    m_shader->enableAttributeArray("canvasUV");
    m_shader->setAttributeBuffer(m_canvasUVAttributeLocation, GL_FLOAT, 3 * sizeof( GLfloat ), 2, 5 * sizeof( GLfloat ) );

    m_tvbo->release();

    m_vao.release();

    //--------
    // cursor
    //--------

    m_cursorvao.create();
    m_cursorvao.bind();

    m_cursorvbo->create();
    m_cursorvbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_cursorvbo->bind();
    m_cursorvbo->allocate(m_cursorModel.data(), m_cursorModel.length() * sizeof( GLfloat ) );

    m_shader->enableAttributeArray("cursorVertex");
    m_shader->setAttributeBuffer(m_cursorAttributeLocation, GL_FLOAT, 0, 3, 6 * sizeof( GLfloat ) );

    m_shader->enableAttributeArray("cursorNormal");
    m_shader->setAttributeBuffer(m_cursorNormalAttributeLocation, GL_FLOAT, 3 * sizeof( GLfloat ), 3, 6 * sizeof( GLfloat ) );

    m_cursorvbo->release();
    m_cursorvao.release();
    m_shader->release();

}


void Scene::initVRScene() {
    QOpenGLFramebufferObjectFormat buffFormat;
    buffFormat.setAttachment(QOpenGLFramebufferObject::Depth);
    buffFormat.setInternalTextureFormat(GL_RGBA16);
    buffFormat.setSamples(S.fixed_samples);

    m_leftBuffer  = new QOpenGLFramebufferObject(m_eyeWidth, m_eyeHeight, buffFormat);
    m_rightBuffer = new QOpenGLFramebufferObject(m_eyeWidth, m_eyeHeight, buffFormat);

    QOpenGLFramebufferObjectFormat resolveFormat;
    resolveFormat.setInternalTextureFormat(GL_RGBA);
    resolveFormat.setSamples(0);

    m_resolveBuffer = new QOpenGLFramebufferObject(m_eyeWidth * 2, m_eyeHeight, resolveFormat);
}


void Scene::setEyeDimension(uint32_t w, uint32_t h) {
    m_width  = m_eyeWidth = w;
    m_height = m_eyeHeight = h;

    m_screenDimensions = ( factor_stereoscopy == 1 ) ?
                         glm::vec2(m_eyeWidth, m_eyeHeight) : glm::vec2(m_width, m_height);
}


void Scene::setSceneDimension(uint32_t w, uint32_t h) {
    m_width  = w;
    m_height = h;
    m_screenDimensions = ( factor_stereoscopy == 1 ) ?
                         glm::vec2(m_eyeWidth, m_eyeHeight) : glm::vec2(m_width, m_height);
}


void Scene::setMatrices(const glm::mat4x4 & v, const glm::mat4x4 & p) {
    m_viewMatrix       = v;
    m_projectionMatrix = p;
}

void Scene::setMatrices(const glm::mat4x4 & v, const glm::mat4x4 & p, const glm::mat4x4 & pose){
    m_viewMatrix       = v;
    m_projectionMatrix = p;
    m_poseMatrix = pose;
}


void Scene::setMatrices(const glm::mat4x4 & v, const glm::mat4x4 & p, const glm::mat4x4 & pose, const glm::vec3 & headPosition){
    m_viewMatrix       = v;
    m_projectionMatrix = p;
    m_poseMatrix = pose;
    m_headPosition = QVector3D(headPosition.x, headPosition.y, headPosition.z);
}

void Scene::renderLeftEye(CanvasContent whatToShow) {
    glViewport(0, 0, m_eyeWidth, m_eyeHeight);
    glEnable(GL_MULTISAMPLE);

    QRect sourceRect(0, 0, m_eyeWidth, m_eyeHeight);
    QRect targetLeft(0, 0, m_eyeWidth, m_eyeHeight);


    m_leftBuffer->bind();
    m_pass = RenderPass::MAIN;
    renderEye(whatToShow);
    m_leftBuffer->release();

    QOpenGLFramebufferObject::blitFramebuffer(m_resolveBuffer, targetLeft,
                                              m_leftBuffer, sourceRect);

}


void Scene::renderRightEye(CanvasContent whatToShow) {
    glViewport(0, 0, m_eyeWidth, m_eyeHeight);
    glEnable(GL_MULTISAMPLE);

    QRect sourceRect(0, 0, m_eyeWidth, m_eyeHeight);
    QRect targetRight(m_eyeWidth, 0, m_eyeWidth, m_eyeHeight);

    m_rightBuffer->bind();
    m_pass = RenderPass::MAIN;
    renderEye(whatToShow);
    m_rightBuffer->release();

    QOpenGLFramebufferObject::blitFramebuffer(m_resolveBuffer, targetRight,
                                              m_rightBuffer, sourceRect);
}


void Scene::renderScreen(CanvasContent whatToShow) {
    glViewport(0, 0, m_width, m_height);
    glEnable(GL_MULTISAMPLE);
    m_pass = RenderPass::MAIN;
    renderEye(whatToShow);
}


void * Scene::getResolveTexture() {
    return (void *)m_resolveBuffer->texture();
}


void Scene::setRotationInfo(float px, float py, float lx, float ly, bool f) {
    m_lastCursor         = glm::vec2(px, py);
    m_currCursor         = glm::vec2(lx, ly);
    m_needRotationUpdate = f;
}

void Scene::setLastRotationFrame(bool b){
    m_lastRotationUpdate = b;
}

void Scene::render() {
    float degree = 0, angle = 0;


    glm::vec3 axis_in_camera_coord;
    glm::mat4 v = m_viewMatrix;

    if(m_lastRotationUpdate && factor_motion == 1 && ( m_whichEye == TargetDevice::LEFTEYE || m_whichEye == TargetDevice::SCREEN ) ) {
        glm::vec3 va;
        H.getArcballVector(m_lastCursor, m_screenDimensions, va);

        glm::vec3 vb;
        H.getArcballVector(m_currCursor, m_screenDimensions, vb);

        angle = std::acos(std::min(1.0f, glm::dot(va, vb) ) );
        angle = angle * 0.2;
        degree = glm::degrees(angle);

        axis_in_camera_coord = glm::cross(va, vb);
        float s = std::sin(degree);
        float c =  std::cos(degree);
        float oc = 1.0f - c;

        for (int i = 0; i < m_numDataPoints; i++) {
            glm::mat4 m;
            if(m_whichEye == TargetDevice::LEFTEYE) {
                getInterationRotation(m_sphereModelMatricesGLM[i], v, s, c, oc, axis_in_camera_coord, m);
                m_sphereLeftRightLastRotMatrices[i] = H.mat4x4ToQMatrix4x4(m) * m_sphereLeftRightLastRotMatrices[i];
            } else if(m_whichEye == TargetDevice::SCREEN) {
                getInterationRotation(m_sphereModelMatricesGLM[i],v, s, c, oc, axis_in_camera_coord, m);
                m_sphereLastRotMatrices[i] = H.mat4x4ToQMatrix4x4(m) * m_sphereLastRotMatrices[i];
            }
        }
    }

    QMatrix4x4 qv4 = H.mat4x4ToQMatrix4x4(v);
    m_shader->setUniformValue(m_rvLoc, qv4);

    if(m_needRotationUpdate) {
        m_shader->setUniformValue(m_cursorsLoc, QVector4D(m_lastCursor.x, m_lastCursor.y, m_currCursor.x, m_currCursor.y) );
    }

    for (int i = 0; i < m_numDataPoints; i++) {
        m_shader->setUniformValue(m_vertexColorLoc, m_sphereColors[i]);
        m_shader->setUniformValue(m_mLoc, m_sphereModelMatrices[i] );
        if(m_whichEye == TargetDevice::SCREEN) {
            m_shader->setUniformValue(m_rLoc, m_sphereLastRotMatrices[i] );
        } else {
            m_shader->setUniformValue(m_rLoc, m_sphereLeftRightLastRotMatrices[i] );
        }
        glDrawArrays(GL_TRIANGLE_STRIP, 0, m_numSphereVertices);
    }

    m_lastRotationUpdate = false;
}

void Scene::getInterationRotation(const glm::mat4 & m, const glm::mat4 & v, float s, float c, float oc, const glm::vec3 & axis, glm::mat4 & result){
    glm::mat3 m2 = glm::mat3(v) * glm::mat3(m);
    glm::mat3 camera2object = glm::inverse(m2);
    glm::vec3 axis_in_object_coord = camera2object * axis;
    H.rotationMatrix(s, c, oc, axis_in_object_coord, result);
}

void Scene::setTargetDevice(TargetDevice whichEye){
    m_whichEye = whichEye;
}

void Scene::setInputAnwer(int ans){
    m_inputAnswer = ans;
}

void Scene::setUpdatingDepthMapInfo(bool b){
    m_needDepthMapUpdate = b;
}

void Scene::buildDepthMap(CanvasContent whatToShow){
    if(m_needDepthMapUpdate) {
        if(m_depthBuffer == nullptr) {
            QOpenGLFramebufferObjectFormat buffFormat;
            buffFormat.setAttachment(QOpenGLFramebufferObject::Depth);
            buffFormat.setInternalTextureFormat(GL_RGBA16);
            buffFormat.setSamples(S.fixed_samples);
            m_depthBuffer = new QOpenGLFramebufferObject(m_depthMapWidth, m_depthMapHeight, buffFormat);
        }
        if(m_depthResolvedBuffer == nullptr) {
            QOpenGLFramebufferObjectFormat resolveFormat;
            resolveFormat.setInternalTextureFormat(GL_RGBA16);
            resolveFormat.setSamples(0);
            m_depthResolvedBuffer = new QOpenGLFramebufferObject(m_depthMapWidth, m_depthMapHeight, resolveFormat);
        }

        m_depthBuffer->bind();
        m_pass = RenderPass::DEPTH;
        glEnable(GL_MULTISAMPLE);
        glViewport(0, 0, m_depthMapWidth, m_depthMapHeight);
        renderEye(whatToShow);
        m_depthBuffer->release();

        QRect rect(0, 0, m_depthMapWidth, m_depthMapHeight);

        QOpenGLFramebufferObject::blitFramebuffer(m_depthResolvedBuffer, rect, m_depthBuffer, rect);

    }

}

void Scene::sendDepthMap()
{
    if(m_pass == RenderPass::MAIN && factor_shading == 2 && m_depthBuffer != nullptr) {
        GLuint d = m_depthResolvedBuffer->texture();
        m_glfuncs->glBindTexture(GL_TEXTURE_2D, d);
        m_glfuncs->glActiveTexture(GL_TEXTURE10);
        m_shader->setUniformValue(m_depthTextureLoc, 10);
        m_shader->setUniformValue(m_depthMapResolutionLoc, QVector2D(m_depthMapWidth, m_depthMapHeight) );
    }
}


void Scene::sendUniforms(){
    m_shader->setUniformValue(m_people_heightLoc, S.shared_participantHeight);
    m_shader->setUniformValue(m_revertCamerPosMatrixLoc, S.camera_revertCamerPosMatrix );

    m_shader->setUniformValue(m_cameraOriginalPositionLoc,
                              QVector3D(S.camera_cameraDataPosition[0], S.camera_cameraDataPosition[1], S.camera_cameraDataPosition[2]) );
    m_shader->setUniformValue(m_cameraPositionLoc, m_headPosition);

    if(factor_stereoscopy == 1) {
        m_shader->setUniformValue(m_headPositionLoc, H.mat4x4ToQMatrix4x4(S.vr_varyingHeadPositionMatrix) );

        if(factor_motion == 1) {
            m_shader->setUniformValue(m_headViewMatrixLoc, H.mat4x4ToQMatrix4x4(S.vr_varyingHeadPoseMatrix) );
            m_shader->setUniformValue(m_headProjectionMatrixLoc,S.screen_fixedProjectionMatrix );
        } else {
            if(factor_dim == 2) {
                m_shader->setUniformValue(m_headViewMatrixLoc,S.screen_fixedViewMatrix );
                m_shader->setUniformValue(m_headProjectionMatrixLoc,S.screen_fixedProjectionMatrix  );
            } else {
                m_shader->setUniformValue(m_headViewMatrixLoc, H.mat4x4ToQMatrix4x4(m_viewMatrix) );
                m_shader->setUniformValue(m_headProjectionMatrixLoc, H.mat4x4ToQMatrix4x4(m_projectionMatrix) );
            }
        }

        if(m_whichEye == TargetDevice::LEFTEYE) {
            m_shader->setUniformValue(m_eyePoseLoc, S.vr_leftPoseMatrix );
            m_shader->setUniformValue(m_eyeProjectionLoc, S.vr_leftProjection );
        } else {
            m_shader->setUniformValue(m_eyePoseLoc, S.vr_rightPoseMatrix );
            m_shader->setUniformValue(m_eyeProjectionLoc, S.vr_rightProjection );
        }

    } else if(factor_stereoscopy == 0 && factor_dim == 2) {
        m_shader->setUniformValue(m_headViewMatrixLoc,S.screen_fixedViewMatrix );
        m_shader->setUniformValue(m_headProjectionMatrixLoc,S.screen_fixedProjectionMatrix  );
    } else if(factor_stereoscopy == 0 && factor_dim == 3) {
        m_shader->setUniformValue(m_headViewMatrixLoc, H.mat4x4ToQMatrix4x4(m_viewMatrix) );
        m_shader->setUniformValue(m_headProjectionMatrixLoc,H.mat4x4ToQMatrix4x4(m_projectionMatrix ) );
    }
    m_shader->setUniformValue(m_factorsLoc, QVector4D(factor_stereoscopy, factor_motion, factor_perspective, factor_shading) );
    m_shader->setUniformValue(m_near_far_zeroLoc, QVector3D(S.camera_near, S.camera_far, 0) );
    m_shader->setUniformValue(m_screenDimensionsLoc, QVector2D(m_screenDimensions.x, m_screenDimensions.y) );
}

void Scene::sharedCodeForDataSky(){
    sendDepthMap();
    sendUniforms();
}

void Scene::showData(CanvasContent flag){
    m_vao.bind();
    m_shader->bind();

    if(m_pass == RenderPass::MAIN) {
        m_shader->setUniformValue(m_whatLoc, 0.f);
    } else if (m_pass == RenderPass::DEPTH) {
        m_shader->setUniformValue(m_whatLoc, -1.f);
    }

    sharedCodeForDataSky();
    m_shader->setUniformValue("needRotationUpdate", m_needRotationUpdate);
    render();

    m_vao.release();
    m_shader->release();

}

void Scene::showSkybox(){

    m_vao.bind();
    m_shader->bind();

    if(m_pass == RenderPass::MAIN) {
        m_shader->setUniformValue(m_whatLoc, 0.4f);
    } else if (m_pass == RenderPass::DEPTH) {
        m_shader->setUniformValue(m_whatLoc, -.4f);
        sharedCodeForDataSky();
        //        m_shader->setUniformValue("needRotationUpdate", false);
        glDrawArrays(GL_TRIANGLES, 0, m_numSkyBoxVertices);
    }

    m_vao.release();
    m_shader->release();

}
void Scene::showReferFrames(){
    m_vao.bind();
    m_shader->bind();
    m_shader->setUniformValue(m_whatLoc, 3.f);

    sendUniforms();
    //    m_shader->setUniformValue(m_referFramesColorLoc, S.conf_framesColor );
    //    m_shader->setUniformValue("needRotationUpdate", false);
    glLineWidth(1.0f);
    glDrawArrays(GL_LINES, 0, m_numReferFramesVertices);

    m_vao.release();
    m_shader->release();

}


void Scene::showFloorFrames(){
    m_vao.bind();
    m_shader->bind();
    m_shader->setUniformValue(m_whatLoc, 5.f);

    sendUniforms();
    //    m_shader->setUniformValue(m_referFramesColorLoc, S.conf_framesColor );
    //    m_shader->setUniformValue("needRotationUpdate", false);
    glLineWidth(1.0f);
    glDrawArrays(GL_LINES, 0, m_numFloorFramesVertices);

    m_vao.release();
    m_shader->release();

}

void Scene::showText(CanvasContent flag){

    if(!m_textIsReady) {
        if(m_textTexture != nullptr)
        delete m_textTexture;

        QPainter painter(m_textImage);
        // background
        painter.fillRect(0, 0, m_textImage->width(), m_textImage->height(), Qt::white);

        if(flag == CanvasContent::INSTRUCTION) {

            // main text
            QString targetText = S.conf_baseInstructions[S.shared_partIndex][1];
            painter.setPen(Qt::black);
            painter.setFont(S.conf_instructionFont);
            painter.drawText(0, 0, m_textImage->width(), m_textImage->height(),  Qt::AlignCenter, targetText);

            painter.setPen(Qt::gray);
            painter.setFont(S.conf_instructionFontTiny);
            painter.drawText(0, m_textImage->height() / 2, m_textImage->width(), m_textImage->height() / 2,  Qt::AlignCenter,
                             S.conf_baseInstructions[S.shared_partIndex][2]);


        } else if(flag == CanvasContent::INPUTANSWER) {

            // main text
            painter.setPen(Qt::black);
            painter.setFont(S.conf_instructionFont);
            painter.drawText(0, 0, m_textImage->width(), m_textImage->height() / 2,  Qt::AlignHCenter | Qt::AlignBottom,
                             S.conf_inputHeader );

            painter.setFont(S.conf_instructionFontLarge);
            painter.drawText(0, m_textImage->height() / 2, m_textImage->width(), m_textImage->height() / 2,  Qt::AlignHCenter | Qt::AlignTop,
                             H.formatAccuracy(m_inputAnswer) + "%");


            // input
            //            if(S.shared_sessionStatus.contains("practice") ) {
            QString reminder = S.inputInstructions[factor_stereoscopy][2];
            if(factor_stereoscopy && factor_motion) {
                reminder += "\nIf you moved, please come back to the home position.";
            }

            painter.setPen(Qt::gray);
            painter.setFont(S.conf_instructionFontSmall);
            painter.drawText(0, m_textImage->height() / 4 + 10, m_textImage->width(),
                             m_textImage->height() / 4,  Qt::AlignCenter, S.inputInstructions[factor_stereoscopy][0]);
            painter.drawText(0, m_textImage->height() / 2 - 10,
                             m_textImage->width(), m_textImage->height() / 4,  Qt::AlignCenter, S.inputInstructions[factor_stereoscopy][1]);
            //            }

            //footnote
            painter.setPen(Qt::gray);
            painter.setFont(S.conf_instructionFontTiny);
            painter.drawText(0,m_textImage->height() / 2 + 100, m_textImage->width(), m_textImage->height() / 3,
                             Qt::AlignCenter,
                             H.toCamelCase(S.shared_sessionStatus) + " trial: " + QString::number(S.shared_currentTrialIndex + 1) +
                             "/" + QString::number( S.shared_numTrials) + "       " +
                             reminder);

        } else if(flag == CanvasContent::ACCURACY) {
            // main text
            painter.setPen(Qt::black);
            painter.setFont(S.conf_instructionFont);
            painter.drawText(0, 0, m_textImage->width(),
                             m_textImage->height() / 2,  Qt::AlignHCenter | Qt::AlignBottom,
                             S.conf_accuracyFeedbackHeader1 );

            painter.setFont(S.conf_instructionFontLarge);
            painter.drawText(0, m_textImage->height() / 2, m_textImage->width(),
                             m_textImage->height() / 2,  Qt::AlignHCenter | Qt::AlignTop,
                             QString::number( (int) std::round(m_trial->test_accuracy * 100) ) + "%");

            // footnote
            painter.setPen(Qt::gray);
            painter.setFont(S.conf_instructionFontTiny);
            painter.drawText(0, m_textImage->height() / 2, m_textImage->width(), m_textImage->height() / 2,
                             Qt::AlignCenter,
                             S.accuracyInstructions[factor_stereoscopy]);
        }

        painter.end();

        m_textTexture = new QOpenGLTexture(*m_textImage);
        m_textTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
        m_textTexture->setMagnificationFilter(QOpenGLTexture::Linear);

        m_textIsReady = true;

    }

    if(m_textIsReady) {
        m_vao.bind();
        m_shader->bind();

        m_shader->setUniformValue(m_whatLoc, 1.f);
        m_shader->setUniformValue(m_factorsLoc, QVector4D(factor_stereoscopy, factor_motion, factor_perspective, factor_shading) );
        m_shader->setUniformValue(m_near_far_zeroLoc, QVector3D(S.camera_near, S.camera_far, 0) );
        m_shader->setUniformValue(m_headPositionLoc, H.mat4x4ToQMatrix4x4(S.vr_varyingHeadPositionMatrix) );
        m_shader->setUniformValue(m_headViewMatrixLoc, S.screen_fixedViewMatrix );
        m_shader->setUniformValue(m_headProjectionMatrixLoc, S.screen_fixedProjectionMatrix );
        m_shader->setUniformValue(m_screenDimensionsLoc, QVector2D(m_screenDimensions.x,m_screenDimensions.y) );

        if(factor_stereoscopy == 1) {
            if(m_whichEye == TargetDevice::LEFTEYE) {
                m_shader->setUniformValue(m_eyePoseLoc, S.vr_leftPoseMatrix);
                m_shader->setUniformValue(m_eyeProjectionLoc, S.vr_leftProjection);
            } else {
                m_shader->setUniformValue(m_eyePoseLoc, S.vr_rightPoseMatrix);
                m_shader->setUniformValue(m_eyeProjectionLoc, S.vr_rightProjection);
            }
        }
        m_shader->setUniformValue(m_revertCamerPosMatrixLoc, S.camera_revertCamerPosMatrix );
        m_shader->setUniformValue(m_canvasTextureLoc,0);
        m_textTexture->bind(0);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, m_numTextPlaneVertices);

        m_vao.release();
        m_shader->release();
        m_textTexture->release();
    }
}

void Scene::showCursor(){

    m_cursorvao.bind();
    m_shader->bind();

    m_shader->setUniformValue(m_whatLoc, 2.f);
    m_shader->setUniformValue(m_factorsLoc, QVector4D(factor_stereoscopy, factor_motion, factor_perspective, factor_shading) );
    m_shader->setUniformValue(m_near_far_zeroLoc, QVector3D(S.camera_near, S.camera_far, 0) );

    m_shader->setUniformValue(m_dimLoc, float(factor_dim) );

    if(factor_stereoscopy == 1) {
        m_shader->setUniformValue(m_headPositionLoc, H.mat4x4ToQMatrix4x4(S.vr_varyingHeadPositionMatrix) );
        m_shader->setUniformValue(m_controllerPositionLoc, H.mat4x4ToQMatrix4x4(S.vr_varyingControllerPositionMatrix) );
        m_shader->setUniformValue(m_headViewMatrixLoc, H.mat4x4ToQMatrix4x4(S.vr_varyingHeadPoseMatrix) );

        if(m_whichEye == TargetDevice::LEFTEYE) {
            m_shader->setUniformValue(m_eyePoseLoc, S.vr_leftPoseMatrix );
            m_shader->setUniformValue(m_eyeProjectionLoc, S.vr_leftProjection );
        } else {
            m_shader->setUniformValue(m_eyePoseLoc, S.vr_rightPoseMatrix );
            m_shader->setUniformValue(m_eyeProjectionLoc, S.vr_rightProjection );
        }

    } else {
        m_shader->setUniformValue(m_cursorPositionOnScreenLoc, QVector2D(S.shared_mouseX, S.shared_mouseY) );
        m_shader->setUniformValue(m_headViewMatrixLoc, S.screen_fixedViewMatrix );

    }

    m_shader->setUniformValue(m_screenDimensionsLoc, QVector2D(m_width, m_height) );
    //    m_shader->setUniformValue("needRotationUpdate", false);
    m_shader->setUniformValue(m_headProjectionMatrixLoc, S.screen_fixedProjectionMatrix );
    m_shader->setUniformValue(m_revertCamerPosMatrixLoc, S.camera_revertCamerPosMatrix );
    glDrawArrays(GL_TRIANGLES, 0, m_cursorModelNumVertices);

    m_cursorvao.release();
    m_shader->release();
}



void Scene::showHomePosition()
{
    m_vao.bind();
    m_shader->bind();
    m_shader->setUniformValue(m_whatLoc, 4.f);
    m_shader->setUniformValue(m_factorsLoc, QVector4D(factor_stereoscopy, factor_motion, factor_perspective, factor_shading) );
    m_shader->setUniformValue(m_near_far_zeroLoc, QVector3D(S.camera_near, S.camera_far, 0) );

    if(factor_stereoscopy == 1) {
        m_shader->setUniformValue(m_headPositionLoc, H.mat4x4ToQMatrix4x4(S.vr_varyingHeadPositionMatrix) );
        m_shader->setUniformValue(m_controllerPositionLoc, H.mat4x4ToQMatrix4x4(S.vr_varyingControllerPositionMatrix) );
        m_shader->setUniformValue(m_headViewMatrixLoc, H.mat4x4ToQMatrix4x4(S.vr_varyingHeadPoseMatrix) );

        if(m_whichEye == TargetDevice::LEFTEYE) {
            m_shader->setUniformValue(m_eyePoseLoc, S.vr_leftPoseMatrix );
            m_shader->setUniformValue(m_eyeProjectionLoc, S.vr_leftProjection );
        } else {
            m_shader->setUniformValue(m_eyePoseLoc, S.vr_rightPoseMatrix );
            m_shader->setUniformValue(m_eyeProjectionLoc, S.vr_rightProjection );
        }

    } else {
        m_shader->setUniformValue(m_cursorPositionOnScreenLoc, QVector2D(S.shared_mouseX, S.shared_mouseY) );
        m_shader->setUniformValue(m_headViewMatrixLoc, S.screen_fixedViewMatrix );
    }

    m_shader->setUniformValue(m_headProjectionMatrixLoc,S.screen_fixedProjectionMatrix );
    m_shader->setUniformValue(m_screenDimensionsLoc, QVector2D(m_width, m_height) );
    //    m_shader->setUniformValue("needRotationUpdate", false);
    m_shader->setUniformValue(m_revertCamerPosMatrixLoc, S.camera_revertCamerPosMatrix );

    glDrawArrays(GL_TRIANGLES, 0, m_homePosition.length() / 3);
    m_vao.release();
    m_shader->release();
}


void Scene::renderEye(CanvasContent whatToRender) {

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    m_glfuncs =  QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();

    if(whatToRender == CanvasContent::DATA) {
        if(m_remark.contains("2d") ) {
            glDisable(GL_DEPTH_TEST);
        }
        if(m_pass == RenderPass::MAIN) {
            showReferFrames();
        }

        showData(whatToRender);
        showSkybox();
    } else if(whatToRender == CanvasContent::INSTRUCTION || whatToRender == CanvasContent::ACCURACY || whatToRender == CanvasContent::INPUTANSWER) {
        showText(whatToRender);
        if(factor_stereoscopy == 1 && factor_motion == 1) {
            showHomePosition();
            showFloorFrames();
        }
    }

    if(m_pass == RenderPass::MAIN) {
        showCursor();
    }
}
