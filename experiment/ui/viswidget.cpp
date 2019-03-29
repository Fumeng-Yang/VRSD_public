#include "viswidget.h"
#include "settings.h"
#include "viswindow.h"
#include <QtMath>
#include <QMessageBox>
#include <QString>
#include <QTimer>
#include <QFileInfo>
#include <QDir>
#include <QKeyEvent>
#include <QApplication>
#include <QDateTime>
#include <QWindow>
#include <QDateTime>
#include <QPainter>
#include <QThread>
#include <iostream>
#include <math.h>
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include <omp.h>

VisWidget::VisWidget(QWidget * parent) :
    QOpenGLWidget(parent),
    m_hmd(0),
    m_cameraData(new CamtransCamera(1) ),
    m_cameraTextCursor(new CamtransCamera(2) ),                                                                          // CamtransCamera, OrbitingCamera
    m_scene(new Scene ),
    m_currentSession(nullptr),
    m_currentSessionData(nullptr),
    m_currentTrialData(nullptr),
    m_currentSessionIndex(-1),
    m_currentTrialStartTimestamp(-1),
    m_currentTrialInitTimestamp(-1),
    m_lastHandledSpaceTimestamp(0),
    m_lastHandledPressTimestamp(0),
    m_lastFineLogSaveTimestamp(0),
    m_currentTrialNum(0),
    factor_stereoscopy(-1),
    factor_motion(-1),
    m_isThisEnabled(false),
    m_draggingFlagIsDirty(false),
    m_waitingAnswerFlagIsDirty(false),
    m_isDragging(false),
    m_dataIsAssigned(false),
    m_answerUpdated(false),
    m_textFlagIsDirty(false),
    m_sceneIsInited(false),
    m_sessionStatus("undefined"),
    m_sessionName("undefined"),
    m_eventTimer(new QTimer ),
    m_dataLoggingTimer(new QTimer ),
    m_frameTimer(new QTimer )
{
    connect(this, SIGNAL(sendToParent()), parent, SLOT(nextPart()) );
    connect(m_eventTimer, SIGNAL(timeout()), this, SLOT(abstractMousePressUpdateSlot()) );
    connect(m_dataLoggingTimer, SIGNAL(timeout()), this, SLOT(saveLogDataSlot()) );
}


VisWidget::~VisWidget() {
    shutdown();
}


QSize VisWidget::minimumSizeHint() const {
    return QSize(1, 1);
}


void VisWidget::updateFramerate() {
    if (S.shared_frames > 0) {
        emit framesPerSecond(S.shared_frames);
    }
    S.shared_frames = 0;
}


void VisWidget::shutdown() {
    if (m_hmd) {
        vr::VR_Shutdown();
        m_hmd = 0;
    }

    qDebug() << "Shutdown()";
}


void VisWidget::initDevices() {
    this->setEnabled(true);
    this->setUpdatesEnabled(true);

    m_width  = S.fixed_windowWidth;
    m_height = S.fixed_windowHeight;

    if (factor_stereoscopy == 1) {
        initVR();
        setMouseTracking(false);
        m_cameraData->setAspectRatio(m_eyeWidth / float(m_eyeHeight) );
        m_cameraTextCursor->setAspectRatio(m_eyeWidth / float(m_eyeHeight) );
    } else {
        setMouseTracking(true);
        m_cameraData->setAspectRatio(m_width / float(m_height) );
        m_cameraTextCursor->setAspectRatio(m_width / float(m_height) );
    }
    grabKeyboard();

    // init vao vbo...
    m_scene->setDeviceInformation(factor_stereoscopy, factor_motion);
    m_deviceRatio = static_cast<QGuiApplication *>( QCoreApplication::instance() )->devicePixelRatio();
    m_scene->setSceneDimension(m_width, m_height);

    m_cameraData->updateMatrices();
    m_cameraTextCursor->updateMatrices();

    S.screen_fixedViewMatrix = H.mat4x4ToQMatrix4x4( m_cameraTextCursor->getViewMatrix() );
    S.screen_fixedProjectionMatrix = H.mat4x4ToQMatrix4x4( m_cameraTextCursor->getProjectionMatrix() );
    S.camera_revertCamerPosMatrix = H.mat4x4ToQMatrix4x4(S.camera_revertCamerPosMatrixGLM);

    m_dataLoggingTimer->start(S.conf_logSaveInterval);
}


void VisWidget::cleanupSession() {
    if(m_hmd) {
        vr::VR_Shutdown();
        m_hmd = 0;
    }

    makeCurrent();
    m_scene->cleanScene();
    releaseKeyboard();
    doneCurrent();
    m_eventTimer->stop();
    m_dataLoggingTimer->stop();
    m_frameTimer->stop();
}


void VisWidget::setSessionControl() {

    int partIndex = S.shared_partIndex;
    int sessionIndex = S.shared_sessionIndex;


    m_currentSessionData = &( ppData.sessionData[sessionIndex] );
    m_currentSession     = &( ppControl.sessions[sessionIndex] );

    S.shared_currentTrialIndex       = -1;

    m_sessionName = S.conf_baseInstructions[partIndex][0];

    factor_stereoscopy = ( ( m_currentSession->factor_sterescopy ) == "yes" ) && S.canIRunVR ? 1 : 0;
    factor_motion      = ( ( m_currentSession->factor_motion ) == "yes" ) ? 1 : 0;
    this->setEnabled(true);

    m_dataIsAssigned = false;

    setTrialStatus(TrialStatus::INTERVALBEFORE);

    qint64 cur = QDateTime::currentMSecsSinceEpoch();

    if (m_sessionName.contains("practice") ) {
        m_sessionStatus   = "practice";
        m_currentTrialNum = m_currentSession->num_practice;
        m_isThisEnabled = true;
        S.conf_baseInstructions[S.shared_partIndex][2] = S.interactionReminderInstructions[factor_stereoscopy][factor_motion];

        m_currentSessionData->practiceStartTimestamp = cur;
        m_currentSessionData->sessionStartTimestamp = cur;
    } else if (m_sessionName.contains("interval") ) {
        m_sessionStatus   = "interval";
        m_currentTrialNum = -1;
        m_isThisEnabled = true;
        S.conf_baseInstructions[S.shared_partIndex][2] = S.intervalReminderInstructions[factor_stereoscopy];

        m_currentSessionData->practiceEndTimestamp = cur;

    } else if (m_sessionName.contains("main") ) {
        m_sessionStatus   = "main";
        m_currentTrialNum = m_currentSession->num_main;
        m_isThisEnabled = true;
        S.conf_baseInstructions[S.shared_partIndex][2] = S.interactionReminderInstructions[factor_stereoscopy][factor_motion];

        m_currentSessionData->mainStartTimestamp = cur;

    } else if (m_sessionName.contains("endtrials") ) {
        m_sessionStatus   = "endtrials";
        m_currentTrialNum = -1;
        m_isThisEnabled = true;
        S.conf_baseInstructions[S.shared_partIndex][2] = S.endingInstruactions[factor_stereoscopy];
        setTrialStatus(TrialStatus::INTERVALAFTER);

        m_currentSessionData->mainEndTimestamp = cur;
        m_currentSessionData->sessionFinishTimestamp = cur;
    } else if (m_sessionName.contains("openquestions") ) {
        qDebug() << "should not reach here 1";
        m_sessionStatus   = "openquestions";
        m_currentTrialNum = -1;
        m_isThisEnabled = false;

    } else if (m_sessionName.contains("break") ) {
        qDebug() << "should not reach here 2";
        m_sessionStatus   = "break";
        m_currentTrialNum = -1;
        m_isThisEnabled = false;
    }

    S.shared_sessionStatus = m_sessionStatus;
    S.shared_numTrials = m_currentTrialNum;
}


void VisWidget::initializeGL() {
    initializeOpenGLFunctions();
}


void VisWidget::stopRendering() {
    m_isThisEnabled = false;
}



void VisWidget::startRendering() {
    m_isThisEnabled = true;
}


void VisWidget::prepareNextTrial() {
    m_dataIsAssigned = false;

    // a hack to wait until the next frame is updated.
    QThread::msleep(50);

    if(S.shared_currentTrialIndex >= 0) {
        //---data logging-------
        m_currentTrialData->trialEndTimeStamp = QDateTime::currentMSecsSinceEpoch();
        m_currentTrialData->trialAnswer = m_inputAnswer;
        m_currentTrialData->trialIndex = S.shared_currentTrialIndex;
        m_currentTrialData->trialSolution = m_currentTrialData->trialBack->test_accuracy;
        //----------------------
    }

    if (S.shared_currentTrialIndex < m_currentTrialNum - 1) {
        S.shared_currentTrialIndex++;
        setTrialControl();

        //---data logging-------
        m_currentTrialData->trialStartTimeStamp = QDateTime::currentMSecsSinceEpoch();
        //----------------------
    } else {
        emit sendToParent();
        setSessionControl();
    }
}


void VisWidget::resetAnswer()
{
    updateInputAnswer( S.shared_answerInit);
    m_answerUpdated = false;
    m_scene->setInputAnwer(m_inputAnswer);
}

void VisWidget::setTrialControl() {

    TrialControl * t;

    if (m_sessionStatus == "practice") {
        t = &( ( m_currentSession->practice_trials )[S.shared_currentTrialIndex] );
        m_currentTrialData = &( ( m_currentSessionData->practiceTrials )[S.shared_currentTrialIndex] );
    } else {
        t = &( ( m_currentSession->main_trials )[S.shared_currentTrialIndex] );
        m_currentTrialData = &( ( m_currentSessionData->mainTrials )[S.shared_currentTrialIndex] );
    }



    factor_perspective =  t->factor_perspective == "yes" ? 1 : 0;
    factor_shading = ( t->factor_shading ).contains("2") ? 2 : ( ( t->factor_shading ).contains("1") ? 1 : 0 );

    qDebug() << "Trial " << S.shared_currentTrialIndex << ", accuracy " << t->test_accuracy
             << ", motion " << t->factor_motion << ", perspective " << t->factor_perspective
             << ", shading " << t->factor_shading << ":" << factor_shading << ", " << t->referred_path;

    m_scene->setTrialControl(*t, m_sessionStatus);

    m_currentTrialStartTimestamp = 0;
    m_currentTrialInitTimestamp  = 0;

    m_dataIsAssigned = true;
    m_draggingFlagIsDirty  = false;



    resetAnswer();
}


void VisWidget::updateInputAnswer(int ans){
    m_inputAnswer =   ans;
    m_answerUpdated = true; // for resetting, it will be updated later.
}

void VisWidget::setMatrices(vr::Hmd_Eye eye, CameraMode viewCamera, CameraMode projectionCamera) {
    glm::mat4x4 v = getHeadViewMatrix(eye, viewCamera);
    glm::mat4x4 p = getProjectionMatrix(eye, projectionCamera);
    glm::mat4x4 pose =  getPoseMatrix(eye, projectionCamera);
    glm::vec3 head = getHeadPosition(eye, viewCamera);
    m_scene->setMatrices(v, p, pose, head);
}

glm::vec3 VisWidget::getHeadPosition(vr::Hmd_Eye eye, CameraMode camera){
    if(camera == CameraMode::VR) {
        return glm::vec3(S.vr_varyingHeadPositionMatrix[3][0], S.vr_varyingHeadPositionMatrix[3][1], S.vr_varyingHeadPositionMatrix[3][2]);
    } else {
        return glm::vec3(m_cameraData->getPosition() );
    }

}


glm::mat4x4 VisWidget::getPoseMatrix(vr::Hmd_Eye eye, CameraMode camera){
    if(eye == vr::Eye_Left && camera == CameraMode::VR) {
        return m_leftPose;
    } else if(eye == vr::Eye_Right && camera == CameraMode::VR) {
        return m_rightPose;
    } else {
        return glm::mat4x4(1);
    }

}

void VisWidget::abstractMousePressedUpdateHandler(int which){

    if( m_trialStatus == TrialStatus::WAITINGANSWER) {
        if(factor_stereoscopy == 0) {// screen
            if(S.shared_mouseY > m_height / 2) {
                updateInputAnswer(H.decreaseAnswer(m_inputAnswer) );
            } else {
                updateInputAnswer(H.increaseAnswer(m_inputAnswer) );
            }
        } else { // VR
            double y = S.vr_varyingControllerPositionMatrix[3][1];
            double y_head = S.vr_varyingHeadPositionMatrix[3][1];

            if(y > y_head - S.magic_VR_InputThreshold ) {
                updateInputAnswer(H.increaseAnswer(m_inputAnswer) );
            } else {
                updateInputAnswer(H.decreaseAnswer(m_inputAnswer) );
            }
        }
        m_textFlagIsDirty = true;
        m_scene->setInputAnwer(m_inputAnswer);

        update();
    }

}

void VisWidget::abstractMousePressStopHandler(int which){
    m_eventTimer->stop();
    //    qDebug() << "stop";
}

void VisWidget::abstractMousePressedHandler(int which){
    //--------- data logging ----------
    if(which == 0 && factor_stereoscopy == 0) {
        QString event = "MousePressHandled";
        H.addLogData(event, factor_perspective, factor_motion, factor_perspective, factor_shading);
    } else if (which == 1 && factor_stereoscopy == 1) {
        QString event = "PadPressHandled";
        H.addLogData(event, factor_perspective, factor_motion, factor_perspective, factor_shading);
    } else {
        QString event = "PressingHandling" + QString::number(which);
        H.addLogData(event, factor_perspective, factor_motion, factor_perspective, factor_shading);
    }
    //---------------------------------

    if(!m_eventTimer->isActive() ) {
        m_eventTimer->start(S.conf_pressingHandleThreshold[factor_stereoscopy]);
    }

}

void VisWidget::abstractSpacePressedHandler(int which){
    qint64 currTimeStamp = QDateTime::currentMSecsSinceEpoch();

    if(m_lastHandledSpaceTimestamp > 0) {
        if( ( m_trialStatus == TrialStatus::SHOWINGDATA && currTimeStamp - m_lastHandledSpaceTimestamp > 2 * S.conf_abstractSpaceHandlingThreshold ) ||
            ( m_trialStatus == TrialStatus::WAITINGANSWER && ( m_inputAnswer > 0 || currTimeStamp - m_lastHandledSpaceTimestamp > 3 * S.conf_abstractSpaceHandlingThreshold ) ) ||
            ( m_trialStatus != TrialStatus::WAITINGANSWER  && m_trialStatus != TrialStatus::SHOWINGDATA && currTimeStamp - m_lastHandledSpaceTimestamp > S.conf_abstractSpaceHandlingThreshold ) ) {

        } else {
            //--------- data logging ----------
            QString event = "GoNextNotHandling-" + QString::number(which);
            H.addLogData(event, factor_perspective, factor_motion, factor_perspective, factor_shading);
            //---------------------------------
            return;
        }
    }

    //--------- data logging ----------
    QString event = "GoNextHandling-" + QString::number(which) + "-TrialStatus-" + QString::number(m_trialStatus);
    H.addLogData(event, factor_perspective, factor_motion, factor_perspective, factor_shading);
    //---------------------------------

    m_lastHandledSpaceTimestamp = currTimeStamp;

    if( m_trialStatus == TrialStatus::INTERVALBEFORE || m_trialStatus == TrialStatus::INTERVALAFTER) {
        setTrialStatus(TrialStatus::HOLDINGTIME);
        prepareNextTrial();
    } else if(m_sessionStatus.contains("practice") && m_trialStatus == TrialStatus::SHOWINGDATA) {
        setTrialStatus(TrialStatus::BLANKVISANS);

        //--------- data logging ----------
        m_currentTrialData->trialVisualizationEndTimeStamp = m_currentTrialVisualizationEndTimestamp = currTimeStamp;
        //---------------------------------

    } else if(m_sessionStatus.contains("practice") && m_trialStatus == TrialStatus::WAITINGANSWER) {
        //feedback
        //--------- data logging ----------
        m_currentTrialData->trialAnswerEndTimeStamp = QDateTime::currentMSecsSinceEpoch();
        //---------------------------------
        setTrialStatus(TrialStatus::FEEDBACK);
    } else if(m_sessionStatus.contains("practice") && m_trialStatus == TrialStatus::FEEDBACK) {
        setTrialStatus(TrialStatus::HOLDINGTIME);
        prepareNextTrial();
    } else if(m_sessionStatus.contains("main") && m_trialStatus == TrialStatus::SHOWINGDATA) {
        setTrialStatus(TrialStatus::BLANKVISANS);
        //--------- data logging ----------
        m_currentTrialData->trialVisualizationEndTimeStamp = m_currentTrialVisualizationEndTimestamp = currTimeStamp;
        //---------------------------------
    } else if(m_sessionStatus.contains("main") && m_trialStatus == TrialStatus::WAITINGANSWER) {
        //--------- data logging ----------
        m_currentTrialData->trialAnswerEndTimeStamp = QDateTime::currentMSecsSinceEpoch();
        //---------------------------------
        setTrialStatus(TrialStatus::HOLDINGTIME);
        prepareNextTrial();
        //        setTrialStatus(TrialStatus::FEEDBACK);
    }
    //    else if(m_sessionStatus.contains("main") && m_trialStatus == TrialStatus::FEEDBACK) {
    //        setTrialStatus(TrialStatus::HOLDINGTIME);
    //        prepareNextTrial();
    //    }

    m_textFlagIsDirty = true;
    update();

}

bool VisWidget::checkTimeConstraints() {
    bool timeIsGoodForData = false;

    if (m_currentTrialStartTimestamp == 0) {
        m_currentTrialStartTimestamp = QDateTime::currentMSecsSinceEpoch();
    }

    qint64 currMillSec = QDateTime::currentMSecsSinceEpoch();

    if ( ( m_currentTrialInitTimestamp == 0 ) &&
         ( currMillSec - m_currentTrialStartTimestamp > S.conf_trialBlankPeriod ) ) {
        m_currentTrialInitTimestamp = currMillSec;
        setTrialStatus(TrialStatus::SHOWINGDATA);
    }

    if ( ( m_currentTrialInitTimestamp > 0 ) ) {
        if ( m_trialStatus == TrialStatus::SHOWINGDATA &&
             ( ( currMillSec - m_currentTrialInitTimestamp < S.conf_trialExploredPeriod ) ||
               ( m_sessionStatus.contains("practice") && S.shared_currentTrialIndex < S.conf_practiceInifTrials ) ) )   {
            timeIsGoodForData = true;
        } else if(currMillSec - m_currentTrialInitTimestamp > S.conf_trialExploredPeriod ) {
            if(m_trialStatus == TrialStatus::SHOWINGDATA) {
                timeIsGoodForData = false;
                setTrialStatus(TrialStatus::BLANKVISANS);
                m_currentTrialData->trialVisualizationEndTimeStamp =  m_currentTrialVisualizationEndTimestamp = currMillSec;
            }
        }

        if(m_trialStatus == TrialStatus::BLANKVISANS &&
           ( currMillSec - m_currentTrialVisualizationEndTimestamp > S.conf_trialBlankPeriod ) ) {
            setTrialStatus(TrialStatus::WAITINGANSWER);
        }
    }

    return timeIsGoodForData;
}

void VisWidget::setTrialStatus(TrialStatus t){
    if(t == TrialStatus::FEEDBACK) {
        //        qDebug() << m_sessionStatus + " go to feedback";

        //---data logging-------
        m_currentTrialData->trialFeedbackStartTimeStamp = QDateTime::currentMSecsSinceEpoch();
        //----------------------

    }

    if(t == TrialStatus::BLANKVISANS) {
        //          qDebug() <<  m_sessionStatus + " blank";
    }

    if(t == TrialStatus::HOLDINGTIME) {
        //          qDebug() <<  m_sessionStatus + " go to holdling time";
    }

    if(t == TrialStatus::SHOWINGDATA) {
        //        qDebug() <<  m_sessionStatus + " go to showing data";

        //---data logging-------
        m_currentTrialData->trialVisualizationStartTimeStamp = QDateTime::currentMSecsSinceEpoch();
        //----------------------
    }
    if(t == TrialStatus::WAITINGANSWER) {
        if(factor_stereoscopy == 0) {
            m_cameraData->resetCamera();
        }
        // qDebug() <<  m_sessionStatus + " go to waiting answer";

        //---data logging-------
        m_currentTrialData->trialAnswerStartTimeStamp = QDateTime::currentMSecsSinceEpoch();
        //----------------------
    }
    if(t == TrialStatus::INTERVALBEFORE) {
        // qDebug() <<  m_sessionStatus + " go to interval before";
    }

    if(t == TrialStatus::INTERVALAFTER) {
        // qDebug() <<  m_sessionStatus + " go to interval before";
    }

    m_trialStatus = t;


    m_textFlagIsDirty = true;
    update();
}

//*************************************************
// the big loop
//*************************************************
void VisWidget::paintGL() {

    if(m_isThisEnabled == false)
    return;

    if (m_sceneIsInited == false) {
        m_scene->initScene();
        m_sceneIsInited = true;

        if(factor_shading == 2) {
            m_scene->setUpdatingDepthMapInfo(true);
        }
    }

    bool timeIsOk       = checkTimeConstraints();
    CanvasContent whatToRender = CanvasContent::EMPTY;

    if(timeIsOk && m_dataIsAssigned && m_trialStatus == TrialStatus::SHOWINGDATA) {
        whatToRender = CanvasContent::DATA;
    } else if (m_trialStatus == TrialStatus::INTERVALBEFORE || m_trialStatus == TrialStatus::INTERVALAFTER ) {
        whatToRender = CanvasContent::INSTRUCTION;
    } else if ( m_trialStatus == TrialStatus::FEEDBACK) {
        whatToRender = CanvasContent::ACCURACY;
    } else if(m_trialStatus == TrialStatus::WAITINGANSWER) {
        whatToRender = CanvasContent::INPUTANSWER;
    } else {
    }

    if(m_textFlagIsDirty) {
        m_scene->updateText();
        m_textFlagIsDirty = false;
    }

    if (m_hmd) {
        updatePoses();
        updateInput();
    }
    //    qDebug() << m_lastX << " " << m_curX << " " << m_lastY << " " << m_curY << " " << m_isDragging << " " << m_draggingFlagIsDirty;

    bool moved = factor_motion != 0 && ( std::abs( m_lastX - m_curX ) > S.delta || std::abs( m_lastY - m_curY ) > S.delta );

    if (whatToRender == CanvasContent::DATA) {
        if(factor_stereoscopy == 1)
        m_scene->setRotationInfo(m_curX,m_lastY, m_lastX, m_curY, m_isDragging && moved);
        else
        m_scene->setRotationInfo(m_curX, m_curY, m_lastX, m_lastY, m_isDragging && moved);
    } else {
        m_scene->setRotationInfo(0, 0, 0, 0, false);
    }


    if(whatToRender == CanvasContent::DATA && factor_shading == 2) {
        m_scene->setUpdatingDepthMapInfo(true);
    }

    if (m_hmd) {
        CameraMode viewCamera = ( factor_stereoscopy == 1 && factor_motion == 1 && whatToRender == CanvasContent::DATA ) ?
                                CameraMode::VR : CameraMode::NATIVE;
        CameraMode projectionCamera = ( factor_stereoscopy == 1 && factor_motion == 1 ) ?
                                      CameraMode::VR : CameraMode::NATIVE;

        m_scene->setLastRotationFrame(m_draggingFlagIsDirty && moved);
        setMatrices(vr::Eye_Left, viewCamera, projectionCamera);
        m_scene->setTargetDevice(TargetDevice::LEFTEYE);
        if(factor_shading == 2 && whatToRender == CanvasContent::DATA) {
            m_scene->buildDepthMap(whatToRender);
        }
        m_scene->renderLeftEye(whatToRender);

        setMatrices(vr::Eye_Right, viewCamera, projectionCamera);
        m_scene->setTargetDevice(TargetDevice::RIGHTEYE);
        if(factor_shading == 2 && whatToRender == CanvasContent::DATA) {
            m_scene->buildDepthMap(whatToRender);
        }
        m_scene->renderRightEye(whatToRender);
        m_scene->setUpdatingDepthMapInfo(false);
    }

    if(!m_hmd || ( m_hmd  && whatToRender != CanvasContent::DATA ) ) {
        m_scene->setLastRotationFrame(m_draggingFlagIsDirty && moved);
        setMatrices(vr::Eye_Left, CameraMode::NATIVE, CameraMode::NATIVE);
        m_scene->setTargetDevice(TargetDevice::SCREEN);
        if(factor_shading == 2 && whatToRender == CanvasContent::DATA) {
            m_scene->buildDepthMap(whatToRender);
        }
        m_scene->renderScreen(whatToRender);
        m_scene->setUpdatingDepthMapInfo(false);
    }

    if (m_hmd) {
        void * resolveTex = m_scene->getResolveTexture();
        vr::VRTextureBounds_t leftRect  = { 0.0f, 0.0f, 0.5f, 1.0f };
        vr::VRTextureBounds_t rightRect = { 0.5f, 0.0f, 1.0f, 1.0f };
        vr::Texture_t composite = { resolveTex, vr::API_OpenGL, vr::ColorSpace_Gamma };

        vr::VRCompositor()->Submit(vr::Eye_Left, &composite, &leftRect);
        vr::VRCompositor()->Submit(vr::Eye_Right, &composite, &rightRect);
    }

    m_draggingFlagIsDirty = false;

    S.shared_frames++;
    update();

}


void VisWidget::resizeGL(int x, int y) {
    // do nothing
}


void VisWidget::keyPressEvent(QKeyEvent * event) {
    bool handlePressed = factor_motion;
    switch (event->key() )
    {
    case Qt::Key_W:
        if(handlePressed) m_cameraData->translate(glm::vec4(0,0, S.magic_keyboardCameraMove, 0) );
        break;
    case Qt::Key_S:
        if(handlePressed) m_cameraData->translate(glm::vec4(0,0,-S.magic_keyboardCameraMove, 0) );
        break;
    case Qt::Key_A:
        if(handlePressed) m_cameraData->translate(glm::vec4(-S.magic_keyboardCameraMove, 0, 0, 0) );
        break;
    case Qt::Key_D:
        if(handlePressed) m_cameraData->translate(glm::vec4(S.magic_keyboardCameraMove, 0, 0, 0) );
        break;
    case Qt::Key_Space:
        abstractSpacePressedHandler(0);
        break;

    default:
        event->ignore();
    }
}


void VisWidget::initVR() {
    vr::EVRInitError error = vr::VRInitError_None;
    m_hmd = vr::VR_Init(&error, vr::VRApplication_Scene);

    if (error != vr::VRInitError_None) {
        m_hmd = 0;

        QString message = vr::VR_GetVRInitErrorAsEnglishDescription(error);
        qCritical() << message;
        QMessageBox::critical(this, "Unable to init VR", message);

        return;
    } else {

        // get eye matrices
        m_rightProjection = H.vrMatrixToGlmMatrixEyeHead(m_hmd->GetProjectionMatrix(vr::Eye_Right, S.camera_near, S.camera_far, vr::API_OpenGL) );
        m_rightPose       = glm::inverse(H.vrMatrixToGlmMatrixEyeHead(m_hmd->GetEyeToHeadTransform(vr::Eye_Right) ) );

        m_leftProjection = H.vrMatrixToGlmMatrixEyeHead(m_hmd->GetProjectionMatrix(vr::Eye_Left, S.camera_near, S.camera_far, vr::API_OpenGL) );
        m_leftPose       = glm::inverse(H.vrMatrixToGlmMatrixEyeHead(m_hmd->GetEyeToHeadTransform(vr::Eye_Left) ) );

        S.vr_leftPoseMatrix = H.mat4x4ToQMatrix4x4(m_leftPose);
        S.vr_rightPoseMatrix = H.mat4x4ToQMatrix4x4(m_rightPose);

        S.vr_leftProjection  = H.mat4x4ToQMatrix4x4(m_leftProjection);
        S.vr_rightProjection  = H.mat4x4ToQMatrix4x4(m_rightProjection);

        S.vr_averageProjection = H.averageMatrices(m_leftProjection, m_rightProjection);

        // setup frame buffers for eyes
        m_hmd->GetRecommendedRenderTargetSize(&m_eyeWidth, &m_eyeHeight);
        m_scene->setEyeDimension(m_eyeWidth, m_eyeHeight);
        m_scene->initVRScene();

        // turn on compositor
        if (!vr::VRCompositor() ) {
            QString message = "Compositor initialization failed. See log file for details";
            qCritical() << message;
            QMessageBox::critical(this, "Unable to init VR", message);
            return;
        }
    }
}


void VisWidget::updatePoses() {
    vr::VRCompositor()->WaitGetPoses(m_trackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);
    for (unsigned int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
        if (m_trackedDevicePose[i].bPoseIsValid) {
            vr::ETrackedDeviceClass trackedDeviceClass = vr::VRSystem()->GetTrackedDeviceClass(i);
            m_matrixDevicePose[i] = H.vrMatrixToGlmMatrixPose(m_trackedDevicePose[i].mDeviceToAbsoluteTracking);

            // use the last one
            if (trackedDeviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_Controller) {
                S.vr_varyingControllerPositionMatrix
                    = H.vrMatrixToGlmMatrixPose(m_trackedDevicePose[i].mDeviceToAbsoluteTracking);
            }

            if (trackedDeviceClass ==  vr::ETrackedDeviceClass::TrackedDeviceClass_HMD) {
                S.vr_varyingHeadPositionMatrix = H.vrMatrixToGlmMatrixPose(m_trackedDevicePose[i].mDeviceToAbsoluteTracking);
                m_scene->setUpdatingDepthMapInfo(true);
            }
        }
    }


    if (m_trackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid) {
        m_hmdPose = glm::inverse(m_matrixDevicePose[vr::k_unTrackedDeviceIndex_Hmd]);
    }

    //--- data logging--------
    qint64 cur = QDateTime::currentMSecsSinceEpoch();

    if(cur - m_lastFineLogSaveTimestamp > S.conf_fineLogAddInterval) {
        QString event1 = "Controller";
        H.addLogData(event1, factor_stereoscopy, factor_motion, factor_perspective, factor_shading, S.vr_varyingControllerPositionMatrix);

        QString event2 = "Hmd";
        H.addLogData(event2, factor_stereoscopy, factor_motion, factor_perspective, factor_shading, S.vr_varyingHeadPositionMatrix);

        m_lastFineLogSaveTimestamp = cur;
    }
    //--- data logging-----
}


void VisWidget::updateInput() {

    if(m_sessionStatus.contains("endtrials") )
    return;

    vr::VREvent_t event;
    while (m_hmd->PollNextEvent(&event, sizeof( event ) ) )
    {
        ProcessVREvent(event);
    }

    bool isPullingTrigger = false, isPressing = false, startToPress = false;

    for (vr::TrackedDeviceIndex_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
        vr::VRControllerState_t state;
        if (m_hmd->GetControllerState(i, &state ) ) {
            //msvc and openvr.h
            if (state.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger) ) {

                m_curPullingkTime = QDateTime::currentMSecsSinceEpoch();

                if(m_prePullingTime == 0) {
                    m_prePullingTime = m_curPullingkTime;
                }

                if(m_curPullingkTime - m_prePullingTime > S.conf_pullingHandleThreshold) {
                    abstractSpacePressedHandler(1);
                    m_prePullingTime = 0;
                }

                isPullingTrigger = true;
            } // end of light on

            if (state.ulButtonPressed & vr::ButtonMaskFromId( vr:: k_EButton_SteamVR_Touchpad) ) {
                m_curPressTime = QDateTime::currentMSecsSinceEpoch();

                if(m_prePressTime == 0) {
                    m_prePressTime = m_curPressTime;
                    startToPress = true;
                }
                glm::mat4 m = m_matrixDevicePose[i];

                if(m_isDragging == false) {
                    m_isDragging = true;
                    S.vr_draggingStartHeadPose = m_hmdPose;
                    glm::vec4 tmp = S.vr_averageProjection * S.vr_draggingStartHeadPose * m[3];

                    m_lastX      = m_curX = ( tmp[0] + S.magic_VR_projectionOffset ) * S.magic_VR_draggingScaleFactor;
                    m_lastY      = m_curY = ( tmp[1] + S.magic_VR_projectionOffset ) * S.magic_VR_draggingScaleFactor;

//                                    qDebug() << m_lastX << " " << m_lastY << " " << tmp[2];
                } else {

                    glm::vec4 tmp = S.vr_averageProjection * S.vr_draggingStartHeadPose * m[3];
                    m_curX = ( tmp[0] + S.magic_VR_projectionOffset ) * S.magic_VR_draggingScaleFactor;
                    m_curY = ( tmp[1] + S.magic_VR_projectionOffset ) * S.magic_VR_draggingScaleFactor;
                }

                isPressing = true;
            }
            if (state.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_Grip) ) {} // end of next sky
        } // end of get state
    } // end of for

    if(!isPullingTrigger) {
        m_curPullingkTime = m_prePullingTime = 0;
    }

    if(m_isDragging == true && isPressing == false) {
        m_draggingFlagIsDirty = true;
    }

    if( isPressing == false) {
        m_prePressTime = m_curPressTime = 0;
        abstractMousePressStopHandler(1);
    }

    if(isPressing == true && startToPress == true) {
        abstractMousePressedHandler(1);
    }

    m_isDragging = isPressing;

}          // end of function


//-----------------------------------------------------------------------------
// Purpose: Processes a single VR event
//-----------------------------------------------------------------------------
void VisWidget::ProcessVREvent(const vr::VREvent_t & event) {
    switch (event.eventType)
    {
    case vr::VREvent_TrackedDeviceActivated:
        qDebug() << "Device " << int(event.trackedDeviceIndex) << " attached. Set it visible.";
        break;

    case vr::VREvent_TrackedDeviceDeactivated:
        qDebug() << "Device " << int(event.trackedDeviceIndex) << " deattached. Invisiable.";
        break;

    case vr::VREvent_TrackedDeviceUpdated:
        qDebug() << "Device " << int(event.trackedDeviceIndex) << " updated.";
        break;
    }
}


glm::mat4x4 VisWidget::getHeadViewMatrix(vr::Hmd_Eye eye, CameraMode whichCamera) {

    if(m_hmd) {// for cursor
        S.vr_varyingHeadPoseMatrix = m_hmdPose;
    }

    if (factor_stereoscopy == 1 && factor_motion == 1 && whichCamera == CameraMode::VR) { // for motion in VR
        return m_hmdPose;
    } else  {// for screen
        return m_cameraData->getViewMatrix();
    }
}


glm::mat4x4 VisWidget::getProjectionMatrix(vr::Hmd_Eye eye, CameraMode whichCamera) {
    if (whichCamera == CameraMode::VR) {
        if (eye == vr::Eye_Left) {
            return m_leftProjection;
        } else {
            return m_rightProjection;
        }
    } else {
        return m_cameraData->getProjectionMatrix();
    }
}


QString VisWidget::getTrackedDeviceString(vr::TrackedDeviceIndex_t device, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError * error) {
    uint32_t len = m_hmd->GetStringTrackedDeviceProperty(device, prop, NULL, 0, error);

    if (len == 0) {
        return "";
    }

    char * buf = new char[len];
    m_hmd->GetStringTrackedDeviceProperty(device, prop, buf, len, error);

    QString result = QString::fromLocal8Bit(buf);
    delete [] buf;

    return result;
}


void VisWidget::mousePressEvent(QMouseEvent * event) {
    if (event->button() == Qt::LeftButton && S.shared_sessionIndex >= 0 && factor_stereoscopy == 0) {
        m_isDragging = true;
        m_lastX      = m_curX = event->x();
        m_lastY      = m_curY = event->y();
    } else {
        m_isDragging = false;
    }

    if(m_trialStatus == TrialStatus::WAITINGANSWER) {

    }
    abstractMousePressedHandler(0);
}


void VisWidget::mouseMoveEvent(QMouseEvent * event) {
    if (m_isDragging) {
        m_curX = event->x();
        m_curY = event->y();
    }
    if(factor_stereoscopy == 0) {//the target is the screen
        S.shared_mouseX = event->x();
        S.shared_mouseY = event->y();
        update();
    }

    //------ data logging------
    qint64 cur = QDateTime::currentMSecsSinceEpoch();
    if(cur - m_lastFineLogSaveTimestamp > S.conf_fineLogAddInterval ) {
        QString sevent = "MouseMove";
        H.addLogData(sevent, factor_stereoscopy, factor_motion, factor_perspective, factor_shading, event->x(), event->y() );
        m_lastFineLogSaveTimestamp = cur;
    }
    //-------
}


void VisWidget::mouseReleaseEvent(QMouseEvent * event) {
    m_isDragging = false;
    m_draggingFlagIsDirty = true;
    abstractMousePressStopHandler(0);
}


void VisWidget::wheelEvent(QWheelEvent * event) {
}

void VisWidget::saveLogDataSlot(){
    H.saveLogData();
}


void VisWidget::renderText(const QString & str, const QFont & font) {
    // Render text
    QPainter painter(this);
    painter.setPen(Qt::black);
    painter.setFont(font);
    painter.drawText(0, 0, width(), height(), Qt::AlignCenter, str);
    painter.end();
}
