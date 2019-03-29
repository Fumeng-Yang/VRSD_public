#include "helpers.h"
#include <iostream>
#include <math.h>
#include <QtMath>
#include <QFile>
#include <QDateTime>

#include "glm/ext.hpp"
Helpers H;

/**
 * @brief Helpers::mat4x4ToQMatrix4x4
 * @param mat
 * @return
 */


int Helpers::increaseAnswer(int n){
    return ( n + 1 ) % 100;
}

int Helpers::decreaseAnswer(int n){
    if(n < 1) {
        return 100;
    } else {
        return n - 1;
    }
}


float Helpers::round(float number, int d) {
    int d10 = 10 ^ d;
    return qRound(number * d10) / float(d10);
}

QString Helpers::toCamelCase(const QString & s)
{
    QStringList parts = s.split(' ', QString::SkipEmptyParts);
    for (int i = 0; i < parts.size(); ++i)
    parts[i].replace(0, 1, parts[i][0].toUpper() );

    return parts.join(" ");
}


QString Helpers::formatAccuracy(float n){
    return QString::number( (int) std::round(n * 100) );
}

QString Helpers::formatAccuracy(double n){
    return QString::number( (int) std::round(n * 100) );
}

glm::mat4x4 Helpers::averageMatrices(const glm::mat4 & m1, const glm::mat4 & m2){
    return glm::mat4( 0.5 * ( m1[0][0] + m2[0][0] ), 0.5 * ( m1[0][1] + m2[0][1] ), 0.5 * ( m1[0][2] + m2[0][2] ), 0.5 * ( m1[0][3] + m2[0][3] ),
                      0.5 * ( m1[1][0] + m2[1][0] ), 0.5 * ( m1[1][1] + m2[1][1] ), 0.5 * ( m1[1][2] + m2[1][2] ), 0.5 * ( m1[1][3] + m2[1][3] ),
                      0.5 * ( m1[2][0] + m2[2][0] ), 0.5 * ( m1[2][1] + m2[2][1] ), 0.5 * ( m1[2][2] + m2[2][2] ), 0.5 * ( m1[2][3] + m2[2][3] ),
                      0.5 * ( m1[3][0] + m2[3][0] ), 0.5 * ( m1[3][1] + m2[3][1] ), 0.5 * ( m1[3][2] + m2[3][2] ), 0.5 * ( m1[3][3] + m2[3][3] )
                      );

}

QString Helpers::formatAccuracy(int n){
    if(n == 0) {
        return "0";
    } else if(n < 10) {
        return "0" + QString::number(n);
    } else {
        return QString::number(n);
    }
}

QMatrix4x4 Helpers::mat4x4ToQMatrix4x4(const glm::mat4x4 & mat) const {
    return QMatrix4x4(mat[0][0], mat[1][0], mat[2][0], mat[3][0],
                      mat[0][1], mat[1][1], mat[2][1], mat[3][1],
                      mat[0][2], mat[1][2], mat[2][2], mat[3][2],
                      mat[0][3], mat[1][3], mat[2][3], mat[3][3]
                      );
}


QMatrix4x4 Helpers::vrMatrixToQMatrix4x4(const vr::HmdMatrix44_t & mat) const {
    return QMatrix4x4( mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
                       mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
                       mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
                       mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]
                       );
}

/**
 * @brief Helpers::between
 * @param e
 * @param a
 * @param b
 * @return
 */
bool Helpers::between(float e, float a, float b) {
    return e >= std::fmin(a, b) && e <= std::fmax(a, b);
}



/**
 * @brief Helpers::vrMatrixToGlmMatrixEyeHead
 * @param mat
 * @return
 */
glm::mat4x4 Helpers::vrMatrixToGlmMatrixEyeHead(const vr::HmdMatrix44_t & mat) {
    return glm::mat4x4(
    mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
    mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
    mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
    mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]
    );
    //    int d = 4;

    //    return glm::mat4x4(
    //        round(mat.m[0][0], d), round(mat.m[1][0], d), round(mat.m[2][0], d), round(mat.m[3][0], d),
    //        round(mat.m[0][1], d), round(mat.m[1][1], d),round(mat.m[2][1], d), round(mat.m[3][1], d),
    //        round(mat.m[0][2], d), round(mat.m[1][2], d), round(mat.m[2][2], d), round(mat.m[3][2], d),
    //        round(mat.m[0][3], d), round(mat.m[1][3], d), round(mat.m[2][3], d), round(mat.m[3][3], d)
    //        );
}

/**
 * @brief Helpers::vrMatrixToGlmMatrixEyeHead
 * @param mat
 * @return
 */
glm::mat4x4 Helpers::vrMatrixToGlmMatrixEyeHead(const vr::HmdMatrix34_t & mat) {
    return glm::mat4x4(
    mat.m[0][0], mat.m[1][0], mat.m[2][0], 0.0,
    mat.m[0][1], mat.m[1][1], mat.m[2][1], 0.0,
    mat.m[0][2], mat.m[1][2], mat.m[2][2], 0.0,
    mat.m[0][3], mat.m[1][3], mat.m[2][3], 1.0f
    );
    //  int d = 4;
    //    return glm::mat4x4(
    //        round(mat.m[0][0], d), round(mat.m[1][0], d), round(mat.m[2][0], d), 0.0,
    //        round(mat.m[0][1], d), round(mat.m[1][1], d),round(mat.m[2][1], d), 0.0,
    //        round(mat.m[0][2], d), round(mat.m[1][2], d), round(mat.m[2][2], d), 0.0,
    //        round(mat.m[0][3], d), round(mat.m[1][3], d), round(mat.m[2][3], d), 1.0f
    //        );
}


/**
 * @brief Helpers::vrMatrixToGlmMatrixPose
 * @param mat
 * @return
 */
glm::mat4x4 Helpers::vrMatrixToGlmMatrixPose(const vr::HmdMatrix34_t & mat) {

    return glm::mat4x4(
    mat.m[0][0], mat.m[1][0], mat.m[2][0], 0.0,
    mat.m[0][1], mat.m[1][1], mat.m[2][1], 0.0,
    mat.m[0][2], mat.m[1][2], mat.m[2][2], 0.0,
    mat.m[0][3], mat.m[1][3], mat.m[2][3], 1.0f
    );
}

/**
 * @brief Helpers::vec3ToQVector3D
 * @param v
 * @return
 */
QVector3D Helpers::vec3ToQVector3D(glm::vec3 v) const {
    return QVector3D(v[0], v[1], v[2]);
}

QVector4D Helpers::vec4ToQVector4D(glm::vec4 v) const {
    return QVector4D(v[0], v[1], v[2], v[3]);
}


void Helpers::getArcballVector(const glm::vec2 & cursor, const glm::vec2 & screenDimensions,
                               glm::vec3 & o_p) {
    glm::vec3 P = glm::vec3(1.0 * cursor[0] / screenDimensions[0] * 2 - 1.0, 1.0 * cursor[1] / screenDimensions[1] * 2 - 1.0, 0);
    P.y = -P.y;
    float OP_squared = P.x * P.x + P.y * P.y;
    if (OP_squared <= 1 * 1) {
        P.z = sqrt(1 * 1 - OP_squared); // Pythagoras
    } else {
        P = glm::normalize(P); // nearest point
    }
    o_p[0] = P[0];
    o_p[1] = P[1];
    o_p[2] = P[2];
}

void Helpers::rotationMatrix(float s, float c, float oc, const glm::vec3 & a, glm::mat4 & result) {
    glm::vec3 axis = glm::normalize(a);
    result[0] = glm::vec4(oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s, 0.0);
    result[1] = glm::vec4(oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s, 0.0);
    result[2] = glm::vec4(oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c, 0.0);
    result[3] = glm::vec4(0.0, 0.0, 0.0, 1.0);
}

void Helpers::addLogData(QString & event, int fs, int fm, int fp, int sd, int x, int y){
    ppData.interactionLogs += ppData.participantID + "," + QString::number(QDateTime::currentMSecsSinceEpoch() ) + "," + \
                              QString::number(S.shared_sessionIndex) + "," + \
                              S.shared_sessionStatus + "," + \
                              QString::number(S.shared_currentTrialIndex) + "," + \
                              event + "," + \
                              QString::number(fs) + "," + QString::number(fm) + "," + QString::number(fp) + "," + QString::number(sd) + "," + \
                              QString::number(x) + "," + QString::number(y) + ", , ," + ", , , ," + ", , , ," + ", , , ," + "\n";

}

void Helpers::addLogData(QString & event, int fs, int fm, int fp, int sd){

    ppData.interactionLogs += ppData.participantID + "," + QString::number(QDateTime::currentMSecsSinceEpoch() ) + "," + \
                              QString::number(S.shared_sessionIndex) + "," + \
                              S.shared_sessionStatus + "," + \
                              QString::number(S.shared_currentTrialIndex) + "," + \
                              event + "," + \
                              QString::number(fs) + "," + QString::number(fm) + "," + QString::number(fp) + "," + QString::number(sd) + "," + \
                              ", , , ," +  ", , , ," + ", , , ," + ", , , ," + "\n";

}

void Helpers::addLogData(QString & event,  int fs, int fm, int fp, int sd, glm::mat4 & m){
    ppData.interactionLogs += ppData.participantID + "," + QString::number(QDateTime::currentMSecsSinceEpoch() ) + "," + \
                              QString::number(S.shared_sessionIndex) + "," + \
                              S.shared_sessionStatus + "," + \
                              QString::number(S.shared_currentTrialIndex) + "," + \
                              event + "," + \
                              QString::number(fs) + "," + QString::number(fm) + "," + QString::number(fp) + "," + QString::number(sd) + "," + \
                              QString::number(m[0][0]) + "," + QString::number(m[0][1]) + "," + QString::number(m[0][2]) + "," + QString::number(m[0][3]) + "," + \
                              QString::number(m[1][0]) + "," + QString::number(m[1][1]) + "," + QString::number(m[1][2]) + "," + QString::number(m[1][3]) + "," + \
                              QString::number(m[2][0]) + "," + QString::number(m[2][1]) + "," + QString::number(m[2][2]) + "," + QString::number(m[2][3]) + "," + \
                              QString::number(m[3][0]) + "," + QString::number(m[3][1]) + "," + QString::number(m[3][2]) + "," + QString::number(m[3][3]) + "\n";

}

void Helpers::saveLogData(){
    QString interacttionLogsHeader = "ParticipantID,Time,Session,Status,Trial,Event,"
                                     "FactorStereo,FactorMotion,FactorPerspective,FactorShading,"
                                     "m11,m12,m13,m14,"
                                     "m21,m22,m23,m24,"
                                     "m31,m32,m33,m34,"
                                     "m41,m42,m43,m44";
    QString logPath = S.shared_headerPath + "/participants_data/logs/" + ppData.participantID + "-logs.csv";
    QFile file( logPath );

    if(!file.exists() || !S.shared_logFileInited ) {
        file.open(QIODevice::WriteOnly);
        QTextStream out(&file);
        out << interacttionLogsHeader + "\n" + ppData.interactionLogs;
        S.shared_logFileInited = true;
    } else {
        file.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream out(&file);
        out <<  ppData.interactionLogs;
    }

    file.close();

    ppData.interactionLogs = ""; // clean

}


QString Helpers::trialDataToString(const SessionData & sd, const TrialData & t, QString ID){
    QString sessionStart = QString::number(sd.sessionStartTimestamp);
    QString sessionEnd = QString::number(sd.sessionFinishTimestamp);

    QString practiceStart = QString::number(sd.practiceStartTimestamp);
    QString practiceEnd = QString::number(sd.practiceEndTimestamp);

    QString mainStart = QString::number(sd.mainStartTimestamp);
    QString mainEnd = QString::number(sd.mainEndTimestamp);

    QString questionStart = QString::number(sd.quesStartTimestamp);
    QString questionEnd = QString::number(sd.quesEndTimestamp);

    QString trialsAnswer = ID + "," + \
                           QString::number(sd.sessionBack->session_num) + "," + \
                           sessionStart + "," + \
                           sessionEnd + "," + \
                           practiceStart + "," + \
                           practiceEnd + "," + \
                           mainStart + "," + \
                           mainEnd + "," + \
                           questionStart + "," + \
                           questionEnd + "," + \
                           t.trialBack->trial_status + "," + \
                           QString::number(t.trialIndex) + "," + \
                           QString::number(t.trialStartTimeStamp)  + "," + \
                           QString::number(t.trialVisualizationStartTimeStamp)  + "," + \
                           QString::number(t.trialVisualizationEndTimeStamp)  + "," + \
                           QString::number(t.trialAnswerStartTimeStamp)  + "," + \
                           QString::number(t.trialAnswerEndTimeStamp)  + "," + \
                           QString::number(t.trialFeedbackStartTimeStamp)  + "," + \
                           QString::number(t.trialEndTimeStamp)  + "," + \
                           QString::number(t.trialAnswer) + "," + \
                           QString::number(t.trialBack->int_accuracy) + "," + \
                           QString::number(t.trialSolution) + "," + \
                           t.trialBack->dataset + "," + \
                           t.trialBack->factor_sterescopy + "," + \
                           t.trialBack->factor_motion + "," + \
                           t.trialBack->factor_perspective + "," + \
                           t.trialBack->factor_shading + "," + \
                           ( ( t.trialBack->remark ).contains("2d") ? "2" : "3" ) + "," + \
                           QString::number(t.trialBack->step) + "," + \
                           QString::number(t.trialBack->bin_index) + "," + \
                           QString::number(t.trialBack->bin_lower) + "," + \
                           QString::number(t.trialBack->bin_upper) + "," + \
                           t.trialBack->referred_file + "," + \
                           +"\n";

    return trialsAnswer;
}

void Helpers::saveParticipantsData(){
    QString trialsHeader = "PariticipantID, "
                           "SessionIndex, "
                           "SessionStart, "
                           "SessionEnd, "
                           "PracticeStart,"
                           "PracticeEnd,"
                           "MainStart,"
                           "MainEnd,"
                           "QuestionStart,"
                           "QuestionEnd,"
                           "TrialStatus,"
                           "TrialIndex, "
                           "TrialStart, "
                           "TrialVisualizationStart, "
                           "TrialVisualizationEnd, "
                           "TrialAnswerStart, "
                           "TrialAnswerEnd, "
                           "TrialFeedbackStart, "
                           "TrialEnd, "
                           "TrialAnswer, "
                           "TrialIntAccuracy, "
                           "TrialSolution, "
                           "TrialDataset, "
                           "FactorStereo, "
                           "FactorMotion, "
                           "FactorPerspective, "
                           "FactorShading, "
                           "Dim,"
                           "Step,"
                           "BinIndex,"
                           "BinLower,"
                           "BinUpper,"
                           "File";

    QString trialsAnswer = "";
    QString ID = ppData.participantID;

    for(int i = 0; i < ppData.numSessions; i++) {

        SessionData * sd = &( ppData.sessionData[i] );

        for(int p = 0; p < sd->sessionBack->num_practice; p++) {
            TrialData * t =  &( sd->practiceTrials[p] );
            trialsAnswer += trialDataToString(*sd, *t, ID);

        }

        for(int m = 0; m < sd->sessionBack->num_main; m++) {
            TrialData * t =  &( sd->mainTrials[m] );
            trialsAnswer += trialDataToString(*sd, *t, ID);
        }
    }


    QString metaHeader = "PariticipantID, "
                         "Age,"
                         "Gender,"
                         "familiarML,"
                         "familiarDR,"
                         "familiarVIS,"
                         "familiarVR,"
                         "Session1Q1,"
                         "Session1Q2,"
                         "Session2Q1,"
                         "Session2Q2,"
                         "Session3Q1,"
                         "Session3Q2,"
                         "Session4Q1,"
                         "Session4Q2,"
                         "PostDataFamiliarity,"
                         "PostComments,"
                         "PostGlasses,"
                         "PostSickness,"
                         "OverviewFinish, "
                         "PrequestionStart, "
                         "PrequestionFinish, "
                         "TaskIntro1Start, "
                         "TaskIntro1Finish, "
                         "TaskIntro2Finish, "
                         "Session1OverviewStart, "
                         "Session1OverviewFinish, "
                         "Session2OverviewStart, "
                         "Session2OverviewFinish, "
                         "Session3OverviewStart, "
                         "Session3OverviewFinish, "
                         "Session4OverviewStart, "
                         "Session4OverviewFinish, "
                         "PostquestionStart, "
                         "PostQuestionEnd";

    QString metaAnswer = ppData.participantID + "," + \
                         QString::number(ppData.age) + "," + \
                         ppData.gender + "," + \
                         QString::number(ppData.familiarML) + "," + \
                         QString::number(ppData.familiarDR) + "," + \
                         QString::number(ppData.familiarVIS) + "," + \
                         QString::number(ppData.familiarVR) + "," + \
                         "\"" + ppData.sessionData[0].question1Answer + "\"" + "," + \
                         "\"" + ppData.sessionData[0].question2Answer + "\"" + "," + \
                         "\"" + ppData.sessionData[1].question1Answer + "\"" + "," + \
                         "\"" + ppData.sessionData[1].question2Answer + "\"" + "," + \
                         "\"" + ppData.sessionData[2].question1Answer + "\"" + "," + \
                         "\"" + ppData.sessionData[2].question2Answer + "\"" +  "," + \
                         "\"" + ppData.sessionData[3].question1Answer + "\"" + "," + \
                         "\"" + ppData.sessionData[3].question2Answer + "\"" +  "," + \
                         "\"" + ppData.postDataFamiliarity + "\"" +  "," + \
                         "\"" + ppData.postComments + "\"" +  "," + \
                         "\"" + ppData.postGlasses + "\"" + "," + \
                         "\"" + ppData.postSickness + "\"" + "," + \
                         QString::number(ppData.overviewFinishTimestamp) + "," + \
                         QString::number(ppData.prequestionStartTimestamp) + "," + \
                         QString::number(ppData.prequestionFinishTimestamp) + "," + \
                         QString::number(ppData.taskIntro1StartTimestamp) + "," + \
                         QString::number(ppData.taskIntro1FinishTimestamp) + "," + \
                         QString::number(ppData.taskIntro2FinishTimestamp) + "," + \
                         QString::number(ppData.session1OverviewStartTimestamp) + "," + \
                         QString::number(ppData.session1OverviewFinishTimestamp) + "," + \
                         QString::number(ppData.session2OverviewStartTimestamp) + "," + \
                         QString::number(ppData.session2OverviewFinishTimestamp) + "," + \
                         QString::number(ppData.session3OverviewStartTimestamp) + "," + \
                         QString::number(ppData.session3OverviewFinishTimestamp) + "," + \
                         QString::number(ppData.session4OverviewStartTimestamp) + "," + \
                         QString::number(ppData.session4OverviewFinishTimestamp) + "," + \
                         QString::number(ppData.postquestionStartTimestamp) + "," + \
                         QString::number(ppData.postquestionEndTimestamp);

    QString judgmentsPath = S.shared_headerPath + "/participants_data/judgments/" + ID + "-master.csv";
    QString metaPath = S.shared_headerPath + "/participants_data/meta/" + ID + "-meta.csv";
    QFile file1( judgmentsPath );
    QFile file2( metaPath );

    file1.open(QIODevice::WriteOnly);
    QTextStream out1(&file1);
    out1 << trialsHeader + "\n" + trialsAnswer;
    file1.close();

    file2.open(QIODevice::WriteOnly);
    QTextStream out2(&file2);
    out2 << metaHeader + "\n" + metaAnswer;
    file2.close();

}
