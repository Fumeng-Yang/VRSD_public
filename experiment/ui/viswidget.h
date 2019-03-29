#ifndef VRVIDGET_H
#define VRVIDGET_H
#include "glm/glm.hpp"
#include "scene/scene.h"
#include "settings.h"
#include "helpers.h"
#include "OrbitingCamera.h"
#include "thirdparty/openvr/headers/openvr_mingw.hpp" // openvr.h for MSVC
#include "abstractbehavior.h"
#include "CamtransCamera.h"
#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>
#include <QOpenGLDebugMessage>
#include <QOpenGLDebugLogger>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>

#include <memory>

/**
 * @brief The VisWidget class
 */
class VisWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
    Q_OBJECT
   public:
    explicit VisWidget(QWidget * parent = 0);
    virtual ~VisWidget() override;

    void initDevices();
    void setSessionControl();
    void setTrialControl();
    void prepareNextTrial();
    void cleanupSession();
    void stopRendering();
    void startRendering();

    void renderText(const QString &, const QFont &);
    QSize minimumSizeHint() const override;

    void resetAnswer();

   signals:
    void framesPerSecond(float);
    void deviceIdentifier(const QString &);
    void frameSwap();
    void statusMessage(const QString &);
    void sendToParent();

   public slots:

   protected slots:
    void updateFramerate();
    void shutdown();
    void   abstractMousePressUpdateSlot(){
        abstractMousePressedUpdateHandler(0);
    }

    void saveLogDataSlot();

   protected:

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void keyPressEvent(QKeyEvent * event) override;
    void ProcessVREvent(const vr::VREvent_t & event);

    // Overridden from QWidget
    virtual void mousePressEvent(QMouseEvent * event) override;
    virtual void mouseMoveEvent(QMouseEvent * event) override;
    virtual void mouseReleaseEvent(QMouseEvent * event) override;
    virtual void wheelEvent(QWheelEvent * event) override;



   private:
    void initVR();

    void updatePoses();

    void updateInput();

    void abstractSpacePressedHandler(int);

    void abstractMousePressedHandler(int);

    void abstractMousePressStopHandler(int);

    void abstractMousePressedUpdateHandler(int);

    void updateInputAnswer(int);

    void setMatrices(vr::Hmd_Eye eye, CameraMode, CameraMode);

    void setTrialStatus(TrialStatus t);

    bool checkTimeConstraints();

    glm::mat4x4 getPoseMatrix(vr::Hmd_Eye eye, CameraMode camera);

    glm::mat4x4 getHeadViewMatrix(vr::Hmd_Eye eye, CameraMode);

    glm::mat4x4 getProjectionMatrix(vr::Hmd_Eye eye, CameraMode);

    glm::mat4x4 viewProjection(vr::Hmd_Eye eye);

    glm::vec3 getHeadPosition(vr::Hmd_Eye eye, CameraMode camera);

    QString getTrackedDeviceString(vr::TrackedDeviceIndex_t device,
                                   vr::TrackedDeviceProperty prop,
                                   vr::TrackedPropertyError  * error = 0);

    vr::IVRSystem           * m_hmd;
    vr::TrackedDevicePose_t m_trackedDevicePose[vr::k_unMaxTrackedDeviceCount];
    glm::mat4x4 m_matrixDevicePose[vr::k_unMaxTrackedDeviceCount];

    glm::mat4x4 m_leftProjection, m_leftPose;
    glm::mat4x4 m_rightProjection, m_rightPose;
    glm::mat4x4 m_hmdPose;




    std::unique_ptr<Camera> m_cameraData;
    std::unique_ptr<Camera> m_cameraTextCursor;
    std::unique_ptr<Scene>  m_scene;

    TrialStatus m_trialStatus;
    QString m_sessionStatus;
    int factor_stereoscopy;
    int factor_motion;
    int factor_perspective;
    int factor_shading;

    qint64 m_curPullingkTime, m_curPressTime;
    qint64 m_prePullingTime, m_prePressTime;
    SessionControl * m_currentSession;
    SessionData    * m_currentSessionData;
    TrialData * m_currentTrialData;
    QOpenGLFunctions  * glFuncs;

    QString m_sessionName;
    int m_currentSessionIndex;
    int m_currentTrialNum;
    int m_inputAnswer;

    uint32_t m_eyeWidth, m_eyeHeight;
    int m_deviceRatio, m_width, m_height;
    qint64 m_currentTrialStartTimestamp;
    qint64 m_currentTrialInitTimestamp;
    qint64 m_currentTrialVisualizationEndTimestamp;
    qint64 m_lastHandledSpaceTimestamp;
    qint64 m_lastHandledPressTimestamp;
    qint64 m_lastFineLogSaveTimestamp;

    bool m_sceneIsInited;
    bool m_dataIsAssigned;
    bool m_isThisEnabled;

    double m_curX, m_lastX, m_curY, m_lastY, m_curZ, m_lastZ;

    bool m_draggingFlagIsDirty;
    bool m_textFlagIsDirty;
    bool m_waitingAnswerFlagIsDirty;
    bool m_isDragging;
    bool m_answerUpdated;

    QTimer          * m_eventTimer;
    QTimer          * m_dataLoggingTimer;
    QTimer          * m_frameTimer;
};

#endif // VRVIDGET_H
