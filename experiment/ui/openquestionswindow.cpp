#include "openquestionswindow.h"
#include "ui_openquestionswindow.h"
#include "settings.h"
#include "helpers.h"
#include <QDateTime>
#include <QMessageBox>

OpenQuestionsWindow::OpenQuestionsWindow(QWidget * parent, AbstractBehavior * next) :
    AbstractBehavior(parent, next),
    ui(new Ui::OpenQuestionsWindow) {

    this->setWindowState(Qt::WindowMaximized);

    ui->setupUi(this);
    if(next && next != nullptr)
        connect(this, SIGNAL(callNextWindow()), next, SLOT(receiveCall()) );

}


OpenQuestionsWindow::~OpenQuestionsWindow() {
    delete ui;
}


void OpenQuestionsWindow::keyPressEvent(QKeyEvent * event) {
    //   qDebug() << "keyPressEvent captured in OpenQuestionsWindow";
}


void OpenQuestionsWindow::setComponents() {
    QString session = S.conf_baseInstructions[S.shared_partIndex][0];
    QString remark  = S.conf_baseInstructions[S.shared_partIndex][1];

    if (session.contains("openquestion") ) {
        ui->instructionsBox->setHtml(remark);
        ui->instructionsBox->setReadOnly(true);
    }                                 // no change to height
}


void OpenQuestionsWindow::prepareWindow() {
    this->setWindowTitle(S.fixed_windowName + "Open questions" + "~" + ppData.participantID);
    this->setEnabled(true);
    this->show();

    //--------
    ppData.sessionData[S.shared_sessionIndex].quesStartTimestamp = QDateTime::currentMSecsSinceEpoch();
    //--------

    ui->instructionsBox->setFixedSize(S.fixed_windowWidth - S.fixed_windowLeftRightMargin * 2,
                                      S.fixed_windowHeight / 10);

    ui->questionBox1->setFixedSize(S.fixed_windowWidth - S.fixed_windowLeftRightMargin * 2,
                                   S.fixed_windowHeight / 4 );

    ui->answer1->setFixedSize(S.fixed_windowWidth  - S.fixed_windowLeftRightMargin * 2 - S.fixed_componentMargin * 2,
                              S.fixed_windowHeight / 4 - S.fixed_componentMargin * 1.2);

    ui->questionBox2->setFixedSize(S.fixed_windowWidth - S.fixed_windowLeftRightMargin * 2,
                                   S.fixed_windowHeight / 4 );

    ui->answer2->setFixedSize(S.fixed_windowWidth  - S.fixed_windowLeftRightMargin * 2 - S.fixed_componentMargin * 2,
                              S.fixed_windowHeight / 4 - S.fixed_componentMargin * 1.2);

    update();
}


void OpenQuestionsWindow::setNextWindow(AbstractBehavior * next) {
    this->next = next;

    connect(this, SIGNAL(callNextWindow1()), next, SLOT(receiveCall()) );
}


void OpenQuestionsWindow::setNextSecondWindow(AbstractBehavior * next2) {
    this->m_next2 = next2;

    connect(this, SIGNAL(callNextWindow2()), next2, SLOT(receiveCall()) );
}


void OpenQuestionsWindow::on_nextButton_clicked() {

    QString q1 = ui->answer1->toPlainText();
    q1 = q1.trimmed();

    QString q2 = ui->answer2->toPlainText();
    q2 = q2.trimmed();


    if(q1 != "") {

        QString sessionName = S.conf_baseInstructions[S.shared_partIndex][0];

        //--------
        ppData.sessionData[S.shared_sessionIndex].quesEndTimestamp = QDateTime::currentMSecsSinceEpoch();
        ppData.sessionData[S.shared_sessionIndex].question1Answer = q1;
        ppData.sessionData[S.shared_sessionIndex].question2Answer = q2;
        //--------

        S.shared_partIndex += 1;

        // ---------important---------
        H.saveParticipantsData();
        // ---------------------------

        this->setEnabled(false);

        // collect data here

        emit callNextWindow();

        this->hide();
        this->close();
        this->destroy();
    } else {
        QMessageBox msgBox;
        msgBox.setText("Please answer the questions");
        msgBox.exec();
    }

}
