#ifndef HELPERS_H
#define HELPERS_H
#include <QMatrix4x4>
#include "glm/glm.hpp"
#include "settings.h"
#include "thirdparty/openvr/headers/openvr_mingw.hpp"
struct Helpers
{
    QMatrix4x4  mat4x4ToQMatrix4x4(const glm::mat4x4 & mat) const;

    QMatrix4x4 vrMatrixToQMatrix4x4 (const vr::HmdMatrix44_t & mat) const;

    QVector3D   vec3ToQVector3D(glm::vec3 v) const;

    QVector4D  vec4ToQVector4D(glm::vec4 v) const;

    glm::mat4x4 vrMatrixToGlmMatrixEyeHead(const vr::HmdMatrix44_t & mat);

    glm::mat4x4 vrMatrixToGlmMatrixEyeHead(const vr::HmdMatrix34_t & mat);

    glm::mat4x4 vrMatrixToGlmMatrixPose(const vr::HmdMatrix34_t & mat);

    glm::mat4x4 averageMatrices(const glm::mat4 & m1, const glm::mat4 & m2);

    void saveParticipantsData();
    void addLogData(QString & event, int fs, int fm, int fp, int sd, int x, int y);
    void addLogData(QString & event, int fs, int fm, int fp, int sd);
    void addLogData(QString & event, int fs, int fm, int fp, int sd, glm::mat4 & m);
    void saveLogData();

    void rotationMatrix(float s, float c, float oc, const glm::vec3 & axis, glm::mat4 & result);

    void getArcballVector(const glm::vec2 & cursor, const glm::vec2 & screenDimensions,
                          glm::vec3 & o_p);

    bool        between(float e, float a, float b);

    QString formatAccuracy(float);
    QString formatAccuracy(double);
    QString formatAccuracy(int);
    QString toCamelCase(const QString & s);


    QString trialDataToString(const SessionData & sd, const TrialData & t, QString ID);

    int increaseAnswer(int);
    int decreaseAnswer(int);

    float round(float, int d);
};

extern Helpers H;
#endif // HELPERS_H
