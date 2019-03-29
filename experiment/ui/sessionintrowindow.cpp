#include "sessionintrowindow.h"
#include "ui_sessionintrowindow.h"
#include "settings.h"
#include <QDateTime>
#include <iostream>

SessionIntroWindow::SessionIntroWindow(QWidget * parent, AbstractBehavior * next) :
    AbstractBehavior(parent, next),
    ui(new Ui::SessionIntroWindow) {
    this->setWindowState(Qt::WindowMaximized);
    ui->setupUi(this);

    connect(this, SIGNAL(callNextWindow()), next, SLOT(receiveCall()) );
}


SessionIntroWindow::~SessionIntroWindow() {
    delete ui;
}


void SessionIntroWindow::keyPressEvent(QKeyEvent * event) {
    //   qDebug() << "keyPressEvent captured in SessionIntroWindow";
}


void SessionIntroWindow::setComponents() {
    int sessionIndex = S.shared_sessionIndex;
    int partIndex    = S.shared_partIndex;
    QString sessionName  = S.conf_baseInstructions[partIndex][0];
    QString instr1       = S.conf_baseInstructions[partIndex][1];
    QString instr2       = S.conf_baseInstructions[partIndex][2];
    QString imagePath = "";

    SessionControl session = ppControl.sessions[sessionIndex];

    this->setWindowTitle(S.fixed_windowName + "Session" + "~" + ppData.participantID);

    QString instr1Suffix;
    if ( ( session.factor_sterescopy == "no" ) && ( session.factor_motion == "no" ) ) {
        instr1 += S.conf_sessionsIntroductions[0][1];
        instr2 = S.conf_sessionsIntroductions[0][3];
    } else if ( ( session.factor_sterescopy == "no" ) && ( session.factor_motion == "yes" ) ) {
        instr1 += S.conf_sessionsIntroductions[1][1];
        instr2 = S.conf_sessionsIntroductions[1][3];
    } else if ( ( session.factor_sterescopy == "yes" ) && ( session.factor_motion == "no" ) ) {
        instr1 += S.conf_sessionsIntroductions[2][1];
        instr2 = S.conf_sessionsIntroductions[2][3];
    } else if ( ( session.factor_sterescopy == "yes" ) && ( session.factor_motion == "yes" ) ) {
        instr1 += S.conf_sessionsIntroductions[3][1];
        instr2 = S.conf_sessionsIntroductions[3][3];
    } else {
        qDebug() << "can find the factors from SessionIntroWindow";
    }
    if (sessionName.contains("session") ) {
        ui->instructionsBox1->setHtml(instr1);
        ui->instructionsBox1->setReadOnly(true);

//        ui->instructionsBox2->setHtml(instr2);
//        ui->instructionsBox2->setReadOnly(true);
    }
}


void SessionIntroWindow::prepareWindow() {
    this->setWindowTitle(S.fixed_windowName + ppData.participantID);

    this->setEnabled(true);

    this->show();

    //   grabKeyboard();

    int w = this->width(), h = this->height();
    ui->instructionsBox1->setFixedSize( S.fixed_windowWidth - S.fixed_windowLeftRightMargin * 2,
                                        S.fixed_windowHeight / 2); // no change to height
//    ui->instructionsBox2->setFixedSize(S.fixed_windowWidth - S.fixed_windowLeftRightMargin * 2,
//                                       S.fixed_windowHeight / 4); // no change to height
    ui->imageBox->setFixedSize(S.fixed_windowWidth - S.fixed_windowLeftRightMargin * 2,
                               0);                    // no change to height
    ui->imageBox->hide();

    int sessionIndex = S.shared_sessionIndex;

    qint64 curr = QDateTime::currentMSecsSinceEpoch();

    if(sessionIndex == 0) {
        ppData.session1OverviewStartTimestamp = curr;
    } else if(sessionIndex == 1) {
        ppData.session2OverviewStartTimestamp = curr;
    } else if(sessionIndex == 2) {
        ppData.session3OverviewStartTimestamp = curr;
    } else {
        ppData.session4OverviewStartTimestamp = curr;
    }
}


void SessionIntroWindow::on_nextButton_clicked() {
    int sessionIndex = S.shared_sessionIndex;

    qint64 curr = QDateTime::currentMSecsSinceEpoch();

    if(sessionIndex == 0) {
        ppData.session1OverviewFinishTimestamp = curr;
    } else if(sessionIndex == 1) {
        ppData.session2OverviewFinishTimestamp = curr;
    } else if(sessionIndex == 2) {
        ppData.session3OverviewFinishTimestamp = curr;
    } else {
        ppData.session4OverviewFinishTimestamp = curr;
    }

    S.shared_partIndex        += 1;
    S.shared_currentTrialIndex = -1;

    this->setEnabled(false);

    emit callNextWindow();

    this->hide();
    this->close();
    this->destroy();
}
