#ifndef SETTINGS_H
#define SETTINGS_H
#include <QString>
#include <QVector>
#include <QMatrix4x4>
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include <cstring>
#include <QFont>
#include <QColor>
// Enumeration values for the Brush types from which the user can choose in the GUI.


enum RenderPass {
    MAIN,
    DEPTH,
    HOLDER
};


enum CanvasContent {
    DATA,
    INSTRUCTION,
    ACCURACY,
    CURSOR,
    INPUTANSWER,
    EMPTY
};


enum CameraMode {
    NATIVE,
    VR
};


enum TargetDevice {
    SCREEN,
    LEFTEYE,
    RIGHTEYE
};


enum TrialStatus {
    INTERVALBEFORE,
    SHOWINGDATA,
    BLANKVISANS,
    WAITINGANSWER,
    FEEDBACK,
    INTERVALAFTER,
    HOLDINGTIME
};


struct TrialControl
{
    QString factor_sterescopy  = "undefined";
    QString factor_motion      = "undefined";
    QString factor_shading     = "undefined";
    QString factor_perspective = "undefined";
    QString remark             = "undefined";
    float test_accuracy      = 0;
    int int_accuracy       = 0;
    QString referred_file      = "undefined";
    QString referred_path      = "undefined";
    float bin_lower          = -999;
    float bin_upper          = -999;
    float step        = -999;
    int bin_index   = -999;
    int num_points  = -999;
    int num_classes = -999;
    float   * * p2d        = nullptr;
    float   * * p3d        = nullptr;
    int     * labels      = nullptr;
    bool    * correctness = nullptr;
    QString trial_status = "undefined";
    QString reserved = "undefined";
    QString dataset = "undefined";

    TrialControl() {
    }
    TrialControl(QString fst, QString fm, QString fs, QString fp, QString r,
                 float ta, int ia,
                 QString rf, QString rp,
                 float bl, float bu, float s, int bi,
                 int np, int nc, QString status, QString set) :
    factor_sterescopy(fst), factor_motion(fm), factor_shading(fs), factor_perspective(fp), remark(r),
    test_accuracy(ta), int_accuracy(ia),
    referred_file(rf), referred_path(rp),
    bin_lower(bl), bin_upper(bu), step(s),
    bin_index(bi), num_points(np), num_classes(nc),
    trial_status(status), dataset(set) {
    }

    ~TrialControl() {
    qDebug() << "~TrialControl()";
    }
};

struct SessionControl
{
    int session_num       = -999;
    QString factor_sterescopy = "undefined";
    QString factor_motion     = "undefined";
    QString data_name         = "undefined";

    int num_main         = -999;
    int num_practice     = -999;
    int num_repeats      = -999;
    int num_points       = -999;
    int num_classes      = -999;
    TrialControl * practice_trials = nullptr;
    TrialControl * main_trials     = nullptr;

    SessionControl() {
    }
    SessionControl(int sn, QString fs, QString fm, int np, int nm, int nr, int npt, int nc, QString dn) :
    session_num(sn), factor_sterescopy(fs), factor_motion(fm),
    num_main(nm), num_practice(np), num_repeats(nr), num_points(npt), num_classes(nc),
    data_name(dn) {
    }

    ~SessionControl() {
    qDebug() << "~SessionControl()";
    }
};

struct ParticipantControl
{
    int participantIndex = -999;
    QString participantID    = "undefinied";
    int num_sessions     = 4;
    QString firstDataName = "undefined";
    SessionControl * sessions        = nullptr;

    ~ParticipantControl() {
    qDebug() << "~ParticipantControl()";
    }
};

struct TrialData
{
    qint64 trialStartTimeStamp      = -999;
    qint64 trialVisualizationStartTimeStamp      = -999;
    qint64 trialVisualizationEndTimeStamp      = -999;
    qint64 trialAnswerStartTimeStamp = -999;
    qint64 trialAnswerEndTimeStamp = -999;
    qint64 trialFeedbackStartTimeStamp = -999;
    qint64 trialEndTimeStamp        = -999;
    float trialSolution = -999;
    float trialAnswer = -999;
    int trialIndex = -999;

    TrialControl * trialBack  = nullptr;

    ~TrialData() {
    qDebug() << "~TrialData()";
    }
};

struct SessionData
{
    qint64 sessionStartTimestamp  = -999;
    qint64 practiceStartTimestamp = -999;
    qint64 practiceEndTimestamp   = -999;
    qint64 mainStartTimestamp     = -999;
    qint64 mainEndTimestamp       = -999;
    qint64 quesStartTimestamp     = -999;
    qint64 quesEndTimestamp       = -999;
    qint64 sessionFinishTimestamp = -999;
    TrialData      * practiceTrials        = nullptr;
    TrialData      * mainTrials            = nullptr;
    QString question1Answer        = "undefined";
    QString question2Answer        = "undefined";
    SessionControl * sessionBack           = nullptr;
    ~SessionData() {
    qDebug() << "~SessionData()";
    }
};

struct ParticipantData
{
    QString participantID    = "undefined";
    int numSessions = 4;
    int participantIndex = -999;
    int age         = -999;
    QString gender      = "undefined";
    int familiarML  = -999;
    int familiarDR  = -999;
    int familiarVIS = -999;
    int familiarVR  = -999;
    float participantHeight = -999;

    QString postComments = "undefined";
    QString postGlasses = "undefined";
    QString postSickness = "undefined";
    QString postDataFamiliarity = "undefined";

    qint64 overviewFinishTimestamp    = -999;
    qint64 prequestionStartTimestamp = -999;
    qint64 prequestionFinishTimestamp = -999;

    qint64 taskIntro1StartTimestamp   = -999;
    qint64 taskIntro1FinishTimestamp   = -999;
    qint64 taskIntro2FinishTimestamp   = -999;

    qint64 session1OverviewFinishTimestamp   = -999;
    qint64 session2OverviewFinishTimestamp   = -999;
    qint64 session3OverviewFinishTimestamp   = -999;
    qint64 session4OverviewFinishTimestamp   = -999;

    qint64 session1OverviewStartTimestamp   = -999;
    qint64 session2OverviewStartTimestamp   = -999;
    qint64 session3OverviewStartTimestamp   = -999;
    qint64 session4OverviewStartTimestamp   = -999;

    qint64 postquestionStartTimestamp = -999;
    qint64 postquestionEndTimestamp = -999;
    SessionData * sessionData = nullptr;

    QString interactionLogs = "";


    ~ParticipantData() {
    qDebug() << "~ParticipantData";
    }
};

struct Settings
{
    const bool ifDebug = false;
    const bool canIRunVR = true;

    const int fixed_samples    = 4;
    const int fixed_windowWidth = 1920;
    const int fixed_windowHeight = 1200;

    const int fixed_windowTopBottomMargin = 30;
    const int fixed_windowLeftRightMargin = 200;
    const int fixed_componentMargin       = 50;

    const QString fixed_windowName    = "~Experiment~";

    const QString fixed_tableau10[10]  =
    {
    // "#E15759", "#FF9D9A", "#79706E", "#BAB0AC", "#D37295",
    // "#FABFD2", "#B07AA1", "#D4A6C8", "#9D7660", "#D7B5A6"
    "#4E79A7", "#F28E2B", "#E15759", "#76B7B2", "#59A14F",
    "#EDC948", "#B07AA1", "#FF9DA7", "#9C755F", "#BAB0AC"
    };

    const QString fixed_tableau20[20] = {
    "#4E79A7", "#A0CBE8", "#F28E2B", "#FFBE7D", "#59A14F",
    "#8CF17D", "#B6992D", "#EDC948", "#499894", "#76B7B2",
    "#E15759", "#FF9D9A", "#79706E", "#BAB0AC", "#D37295",
    "#FABFD2", "#B07AA1", "#D4A6C8", "#9D7660", "#D7B5A6"
    };

    static QString conf_baseInstructions[][3];
    static const QString conf_sessionsIntroductions[][4];

    const QString conf_cursorModelPath = ":/resources/models/hand-right.obj";
    const QString conf_taskIntrodcutionImagePath1Babi = ":/resources/img/babi-examples.png";
    const QString conf_taskIntrodcutionImagePath1Cifar10 = ":/resources/img/cifar10-examples.png";
    const QString conf_taskIntrodcutionImagePath2 = ":/resources/img/interaction_examples.png";
    const QString conf_participantsPath          = "datasets/participants.json";

    const QString conf_accuracyFeedbackHeader1 = "The true accuracy is ";
    const QString conf_accuracyFeedbackHeader2 = "(The accuracy is ";
    const QString conf_inputHeader = "Your estimation of accuracy is ";

    const QString inputInstructions[2][3] = {
    {"+ Press upper area on the screen", "- Press lower area on the screen", "To submit this trial press <space>"},
    {"+ Higher up the controller and press", "- Lower down the controller and press", "To submit this trial pull the trigger"}
    };

    const QString accuracyInstructions[2] =
    {"When ready, press <space> to go to the next trial.", "When ready, pull the trigger go to the next trial."};


    const QString intervalReminderInstructions[2] =
    {"Press <space> to proceed", "Pull the trigger to proceed."};


    const QString interactionReminderInstructions[2][2] =
    {
    {
        "Press <space> to proceed",
        "You can press & drag the mouse or use <WASD> to interact with the scatterplot\nWhen ready, press <space> to proceed."

    },
    {
        "Pull the trigger to proceed.",
        "You can walk or press & drag the controller to interact with the scatterplot\nWhen ready, pull the trigger to proceed."
    }
    };


    const QString endingInstruactions[2] = {
    "When ready, press <space> to proceed. \nYou will answer a few questions.",
    "Please take off the headset and come back to the computer.\nWhen ready, press <space> to proceed. \nYou will answer a few questions. "
    };

    const float camera_near           = 0.018f;//0.018
    const float camera_far            = 75.f; // 75
    const float camera_angleLeftRight = 110.f;
    const float camera_fov            = 110.f;
    const float camera_angleTopBottom         = 110.f;
    const glm::vec4 camera_look;
    const glm::vec4 camera_up = glm::vec4(0, 1, 0, 0);
    const glm::vec4 camera_u;
    const glm::vec4 camera_v;
    const glm::vec4 camera_w;
    const float camera_aspectRatio     = float(fixed_windowWidth) / fixed_windowHeight;
    const glm::vec3 camera_lightDirection  = glm::vec3(1.0f, 1.0f, 1.0f);
    const glm::mat4 camera_lightProjection = glm::ortho(-5.f, 5.f, -5.f, 5.f,
                                                        camera_near, camera_far);
    const glm::mat4 camera_lightView   = glm::lookAt(camera_lightDirection,
                                                     glm::vec3(0.0f, 0.0f, -1.0f),
                                                     glm::vec3(0.0f, 1.0f, 0.0f) );
    const glm::mat4 camera_lightSpaceMatrix = camera_lightProjection * camera_lightView;
    const QVector3D camera_lightColor      = QVector3D(1.0f, 1.0f, 1.0f);
    const QVector3D camera_lightPosition  = QVector3D(1.0f, 0.1f, 1.0f); // don't change?
    const glm::vec4 camera_cameraDataPosition = glm::vec4(1.15f, 0.3f, 1.15f, 1);// glm::vec4(1.26f, 1.26f, 1.26f, 1);
    const glm::vec4 camera_cameraTextCursorScreenPosition = glm::vec4(0, 0, 2.0f, 1);

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -camera_cameraDataPosition.z);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::normalize(-glm::cross(up, cameraDirection) );
    glm::vec3 cameraUp = glm::cross(cameraDirection, -cameraRight);
    glm::mat4 t = glm::translate(-cameraPos);

    glm::mat4 r = glm::mat4(cameraRight.x, cameraUp.x, cameraDirection.x, 0,
                            cameraRight.y, cameraUp.y, cameraDirection.y, 0,
                            cameraRight.z, cameraUp.z, cameraDirection.z, 0,
                            0, 0, 0, 1);

    glm::mat4 camera_zeroViewMatrix = r * t;

    const int conf_sphereDiv1         = 12;//12
    const int conf_sphereDiv2         = 16;//17
    const int conf_practiceInifTrials  = 3;
    const float conf_floorPosition = -3;
    const qint64 conf_pullingHandleThreshold = 50;
    const qint64 conf_pressingHandleThreshold[2] = {50, 70};
    const qint64 conf_abstractSpaceHandlingThreshold = 1000;
    const qint64 conf_trialBlankPeriod    = 1000;
    //------------------------------------------------
    const qint64 conf_trialExploredPeriod = 60000; // mill sec
    const int conf_breakingTime        = 60 * 2; // sec
    const int conf_fineLogAddInterval = 100;
    const int conf_logSaveInterval = 5000;
    //------------------------------------------------
    const double conf_pointSize = 0.015;
    const QFont conf_instructionFontLarge = QFont("AvenirNext LT Pro Regular", 62);
    const QFont conf_instructionFont = QFont("AvenirNext LT Pro Regular", 40);
    const QFont conf_instructionFontSmall = QFont("AvenirNext LT Pro Regular", 26);
    const QFont conf_instructionFontTiny = QFont("AvenirNext LT Pro Regular", 20);
    const QColor conf_backGround = QColor(250, 250, 250, 255);
    const QVector4D conf_framesColor = QVector4D(0.85f, 0.85f, 0.85f, 1.0f);

    QString shared_projectionMethod          = "undefined";
    QString shared_headerPath                = "undefined";
    long int shared_frames = 0;

    int shared_currentTrialIndex     = -999;
    int shared_partIndex    = 0;
    int shared_sessionIndex = -999;
    int shared_numTrials = -999;
    float shared_participantHeight = -camera_cameraDataPosition.y - 0.1; // 0.1 is eye to top
    QString shared_sessionStatus = "undefined";

    float shared_mouseX = 0, shared_mouseY = 0;
    int shared_answerInit = 0;

    QMatrix4x4 screen_fixedViewMatrix;
    QMatrix4x4 screen_fixedProjectionMatrix;

    glm::mat4 vr_varyingHeadPositionMatrix = glm::mat4(1);
    QMatrix4x4 vr_leftPoseMatrix;
    QMatrix4x4 vr_rightPoseMatrix;
    glm::mat4 vr_rightPoseMatrixGLM;

    glm::mat4 vr_draggingStartHeadPose  = glm::mat4(1);
    glm::mat4 vr_varyingControllerPositionMatrix  = glm::mat4(1);
    QMatrix4x4 vr_leftProjection;
    QMatrix4x4 vr_rightProjection;
    glm::mat4 vr_rightProjectionGLM;
    glm::mat4 vr_averageProjection = glm::mat4(1);
    glm::mat4 vr_varyingHeadPoseMatrix = glm::mat4(1);

    const float magic_VR_InputThreshold = 0.2;
    const float magic_VR_projectionOffset = 0.5;
    const float magic_VR_draggingScaleFactor = 1000;
    const float magic_textPlaneScaleFactor = 1.0f / 300.0f;
    const float magic_depthMapScaleFactor = 1.2f;
    const float magic_textPlaneZ = -30.f;
    const float magic_keyboardCameraMove = 20.0f / magic_VR_draggingScaleFactor;
    const float magic_scalingFactor = 0.25;
    const float magic_textOff = 50.f;
    const float magic_revertZ = glm::length(glm::vec3(camera_cameraDataPosition) );//1.717f;//float(fixed_windowHeight) / fixed_windowWidth;//1.717f; // real magic number
    const float magic_revertY = 0.1f;
    double delta = 1;

    glm::vec3 cameraPos2 = glm::vec3(0.0, 0, -magic_revertZ);
    glm::vec3 cameraTarget2 = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraDirection2 = glm::normalize(cameraPos2 - cameraTarget2);

    glm::vec3 up2 = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight2 = glm::normalize(-glm::cross(up2, cameraDirection2) );
    glm::vec3 cameraUp2 =  glm::normalize(glm::cross(cameraDirection2, -cameraRight2) );
    glm::mat4 t2 = glm::translate(-cameraPos2);

    glm::mat4 r2 = glm::mat4(cameraRight2.x, cameraUp2.x, cameraDirection2.x, 0,
                             cameraRight2.y, cameraUp2.y, cameraDirection2.y, 0,
                             cameraRight2.z, cameraUp2.z, cameraDirection2.z, 0,
                             0, 0, 0, 1);

    glm::mat4 camera_revertCamerPosMatrixGLM = r2 * t2;
    QMatrix4x4 camera_revertCamerPosMatrix;
    bool shared_logFileInited  = false;

};
extern Settings S;
extern ParticipantData ppData;
extern ParticipantControl ppControl;
#endif // SETTINGS_H
