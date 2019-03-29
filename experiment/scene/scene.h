#ifndef SCENE_H
#define SCENE_H
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>
#include <QOpenGLDebugMessage>
#include <QOpenGLDebugLogger>
#include <QOpenGLTexture>
#include <QElapsedTimer>
#include <QOpenGLFunctions>
#include <QFont>
#include <QPainter>
#include <QRandomGenerator>

#include <memory>
#include "openvr_mingw.hpp"
#include "glm/glm.hpp"
#include "settings.h"
#include "helpers.h"
#include "modelloader.h"
/**
 * @brief The Scene class
 * The scene knows how to draw all the objects
 * It holds all the opengl calls
 */
class Scene
{
   public:
    Scene();
    ~Scene();
    // initialize the scene
    void initScene();
    void setTrialControl(TrialControl & ts, QString status);
    void initVRScene();
    void setDeviceInformation(int, int);
    void setEyeDimension(uint32_t w, uint32_t h);
    void setSceneDimension(uint32_t w, uint32_t h);
    void setMatrices(const glm::mat4x4 & v, const glm::mat4x4 & p, const glm::mat4x4 & pose);
    void setMatrices(const glm::mat4x4 & v, const glm::mat4x4 & p);
    void setMatrices(const glm::mat4x4 & v, const glm::mat4x4 & p, const glm::mat4x4 & pose, const glm::vec3 & headPosition);
    void setTargetDevice(TargetDevice whichEye);
    // [VR] render the left eye
    void renderLeftEye(CanvasContent flag);

    // [VR] render the right eye
    void renderRightEye(CanvasContent flag);

    void render();

    // render the [comp]uter / [comp]anion window
    void renderScreen(CanvasContent);

    // get the resolve texture
    void * getResolveTexture();

    void cleanScene();

    void setRotationInfo(float, float, float, float, bool);

    void setLastRotationFrame(bool);

    void updateText();

    void buildTextInstruction();

    void showCursor();

    void setInputAnwer(int);

    void buildDepthMap(CanvasContent);

    void setPass(RenderPass);

    void setUpdatingDepthMapInfo(bool);

    void submitOneSphere(int i, glm::mat4 & v, float s, float c, float oc, glm::vec3 & axis_in_camera_coord, QMatrix4x4 &);

    void showHomePosition();

    void buildHomePosition();

    void sendDepthMap();

    void sendUniforms();

   private:
    void renderEye(CanvasContent);

    void showData(CanvasContent);

    void showReferFrames();

    void showFloorFrames();

    void showSkybox();

    void sharedCodeForDataSky();

    void showText(CanvasContent);

    void buildFloorFrames();

    void buildTheSphere();

    void buildTextCanvas();

    void buildSkybox();

    void buildReferFrames();

    void loadCursorModel();

    void getInterationRotation(const glm::mat4 & m, const glm::mat4 & v, float s, float c, float oc, const glm::vec3 & axis, glm::mat4 & result);

    GLuint m_vertexAttributeLocation;
    GLuint m_normalAttributeLocation;
    GLuint m_referFrameAttributeLocation;
    GLuint m_floorFrameAttributeLocation;
    GLuint m_canvasAttributeLocation;
    GLuint m_canvasUVAttributeLocation;
    GLuint m_cursorAttributeLocation;
    GLuint m_cursorNormalAttributeLocation;
    GLuint m_cursorUVAttributeLocation;
    GLuint m_skyboxVertexAttributeLocation;
    GLuint m_homeVertexAttributeLocation;

    int m_cursorsLoc;
    int m_vertexColorLoc;
    int m_rvLoc;
    int m_mLoc;
    int m_rLoc;
    int m_updateFlagLoc;
    int m_whatLoc;
    int m_homeLoc;
    int m_headPositionLoc;
    int m_eyePoseLoc;
    int m_eyeProjectionLoc;
    int m_screenDimensionsLoc;
    int m_headViewMatrixLoc;
    int m_headProjectionMatrixLoc;
    int m_revertCamerPosMatrixLoc;
    int m_people_heightLoc;
    int m_cursorPositionOnScreenLoc;
    int m_controllerPositionLoc;
    int m_depthTextureLoc;
    int m_depthMapResolutionLoc;
    int m_factorsLoc;
    int m_near_far_zeroLoc;
    int m_canvasTextureLoc;
    int m_referFramesColorLoc;
    int m_cameraOriginalPositionLoc;
    int m_cameraPositionLoc;
    int m_dimLoc;
    // shadow shader, generate the depth map
    QOpenGLShaderProgram * m_depthShader;
    // phong shader, used in the regular rendering
    QOpenGLShaderProgram * m_shader;
    // vertex buffer for ALL vertices
    QOpenGLBuffer * m_vvbo, * m_rvbo, * m_tvbo, * m_cursorvbo, * m_skyvbo, * m_homevbo, * m_floorvbo;
    // VAO
    QOpenGLVertexArrayObject m_vao, m_cursorvao, m_skyvao;

    QOpenGLFramebufferObject * m_leftBuffer;
    QOpenGLFramebufferObject * m_rightBuffer;
    QOpenGLFramebufferObject * m_depthBuffer;
    QOpenGLFramebufferObject * m_depthResolvedBuffer;

    // the combined buffer for left and right one
    QOpenGLFramebufferObject * m_resolveBuffer;

    // the depth map
    QOpenGLTexture * m_depthMap;

    QOpenGLTexture * tt;

    // the current view matrix, should be set up before drawing a frame
    glm::mat4x4 m_viewMatrix;
    // the current project matrix, different for different devices / eyes
    // should be updates before drawing a frame
    glm::mat4x4 m_projectionMatrix;

    glm::mat4x4 m_poseMatrix;
    // the width and height for the current eye buffer
    uint32_t m_eyeWidth, m_eyeHeight;
    // the width and height for the screen framebuffer
    uint32_t m_width, m_height;

    SessionControl * m_session;
    SessionData    * m_sessionData;
    ModelLoader * m_modelLoader;

    GLint factor_stereoscopy;
    GLint factor_motion;
    GLint factor_shading;
    GLint factor_perspective;
    GLint factor_dim;

    TrialControl     * m_trial;
    TargetDevice m_whichEye;

    QVector<GLfloat> m_sphere;
    QVector<GLfloat> m_textCanvas;
    QVector<GLfloat> m_referFrames;
    QVector<GLfloat> m_floorFrames;
    QVector<GLfloat> m_cursorModel;
    QVector<GLfloat> m_skybox;
    QVector<GLfloat> m_homePosition;

    QVector<QMatrix4x4> m_sphereModelMatrices;
    QVector<glm::mat4> m_sphereModelMatricesGLM;
    QVector<QMatrix4x4> m_sphereLastRotMatrices;
    QVector<QMatrix4x4> m_sphereLeftRightLastRotMatrices;
    QVector<QVector3D> m_sphereColors;
    QVector3D m_headPosition;

    QString m_remark;
    glm::vec2 m_lastCursor;
    glm::vec2 m_currCursor;
    QOpenGLTexture * m_textTexture;
    QImage * m_textImage;
    glm::mat4 m_headPoseMatrix;
    glm::vec2 m_screenDimensions;
    glm::mat4 m_interactionRotation;

    bool m_needRotationUpdate = false;
    bool m_lastRotationUpdate = false;
    bool m_rotFlag = false;
    bool m_shouldStoreRot = false;
    bool m_canvasIsReady = false;
    bool m_textIsReady = false;
    bool m_isCameraPostionSetted;
    bool m_needDepthMapUpdate = true;
    bool m_draggingFlagIsDirty;

    qint64 m_frame;
    int m_trialIndex;
    int m_numDataPoints;
    int m_cursorModelNumVertices;
    int m_numSphereVertices;
    int m_numTextPlaneVertices;
    int m_numSkyBoxVertices;
    int m_numReferFramesVertices;
    int m_numFloorFramesVertices;
    int m_inputAnswer;
    int m_depthMapWidth;
    int m_depthMapHeight;

    RenderPass m_pass;
    float m_deviceRatio;

    QOpenGLTexture * m_depthTexture;
    QRandomGenerator m_qrand;
    QOpenGLFunctions_4_5_Core   * m_glfuncs;
};

#endif // SCENE_H
