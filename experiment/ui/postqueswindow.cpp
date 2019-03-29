#include "postqueswindow.h"
#include "ui_postqueswindow.h"
#include "settings.h"
#include "helpers.h"
#include <QDateTime>
#include <QMessageBox>
PostQuesWindow::PostQuesWindow(QWidget * parent, AbstractBehavior * next) :
    AbstractBehavior(parent, next),
    ui(new Ui::PostQuesWindow) {
    this->setWindowState(Qt::WindowMaximized);

    ui->setupUi(this);

    connect(this, SIGNAL(callNextWindow()), next, SLOT(receiveCall()) );
}


PostQuesWindow::~PostQuesWindow() {
    delete ui;
}


void PostQuesWindow::keyPressEvent(QKeyEvent * event) {
    //   qDebug() << "keyPressEvent captured in PostQuesWindow";
}


void PostQuesWindow::setComponents() {
    QString session = S.conf_baseInstructions[S.shared_partIndex][0];
    QString remark  = S.conf_baseInstructions[S.shared_partIndex][1];


    if (session.contains("post") ) {

    }
}


void PostQuesWindow::prepareWindow() {
    this->setWindowTitle(S.fixed_windowName + "Post-questions" + "~" + ppData.participantID);

    this->setEnabled(true);

    this->show();

    ui->instructionBox->setFixedSize(S.fixed_windowWidth - S.fixed_windowLeftRightMargin * 2,
                                     S.fixed_windowHeight / 10);

    //   ui->instructionBox->setFixedSize(S.fixed_windowWidth - S.fixed_windowLeftRightMargin * 2,
    //                                    S.fixed_windowHeight / 10);

    //------
    ppData.postquestionStartTimestamp = QDateTime::currentMSecsSinceEpoch();
    //------
}


void PostQuesWindow::on_nextButton_clicked() {


    //------
    QString dataFamiliar     = ( ui->datafamilarField->toPlainText() ).trimmed();
    QString comments     = ui->commentField->toPlainText();
    QAbstractButton * glasses = ui->lensGlassesRBGroup->checkedButton();
    QAbstractButton * sickness     = ui->sicknessRBGroup->checkedButton();

    if(glasses && sickness && dataFamiliar != "") {
        ppData.postDataFamiliarity = dataFamiliar;
        ppData.postComments = comments;
        ppData.postGlasses = glasses->accessibleName();
        ppData.postSickness = sickness->accessibleName();
        ppData.postquestionEndTimestamp = QDateTime::currentMSecsSinceEpoch();

        // ---------important---------
        H.saveParticipantsData();
        // ---------------------------

        this->setEnabled(false);

        emit callNextWindow();

        this->close();
        this->hide();

        this->destroy();
    } else {
        QMessageBox msgBox;
        msgBox.setText("Please answer the questions");
        msgBox.exec();
    }
    //------


}
