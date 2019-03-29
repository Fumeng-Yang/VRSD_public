#include "prequeswindow.h"
#include "ui_prequeswindow.h"
#include "settings.h"
#include <iostream>
#include <QIntValidator>
#include <QButtonGroup>
#include <QMessageBox>
#include <QDateTime>
#include <QKeyEvent>
PreQuesWindow::PreQuesWindow(QWidget * parent, AbstractBehavior * next) :
    AbstractBehavior(parent, next),
    ui(new Ui::PreQuesWindow) {
    this->setWindowState(Qt::WindowMaximized);

    ui->setupUi(this);

    ui->ageField->setValidator(new QIntValidator(18, 65, static_cast<QMainWindow *>( this ) ) );

    connect(this, SIGNAL(callNextWindow()), next, SLOT(receiveCall()) );
}


PreQuesWindow::~PreQuesWindow() {
    delete ui;
}


void PreQuesWindow::prepareWindow() {
    this->setWindowTitle(S.fixed_windowName + "Pre-questions" + "~" + ppData.participantID);

    this->setEnabled(true);

    this->show();

    int w = this->width(), h = this->height();
    ui->instructionsBox->setFixedSize(S.fixed_windowWidth - S.fixed_windowLeftRightMargin * 2,
                                      ui->instructionsBox->height() );     // no change to height
    //-----
    ppData.prequestionStartTimestamp = QDateTime::currentMSecsSinceEpoch();
    //-----
}


void PreQuesWindow::setComponents() {
    QString session = S.conf_baseInstructions[S.shared_partIndex][0];
    QString remark  = S.conf_baseInstructions[S.shared_partIndex][1];

    this->setWindowTitle(S.fixed_windowName + "Pre-questions" + "~" + ppData.participantID);

    if (session.contains("prequestions") ) {
        ui->instructionsBox->setHtml(remark);
        ui->instructionsBox->setReadOnly(true);
        ui->centralwidget->setEnabled(true);
        ui->ageField->setEnabled(true);
    }
}


void PreQuesWindow::keyPressEvent(QKeyEvent * event) {
    //   qDebug() << "keyPressEvent captured in PreQuesWindow";
}


void PreQuesWindow::on_nextButton_clicked() {
    QString age     = ui->ageField->text();
    int ageInt  = age.toInt();
    QAbstractButton * gender = ui->genderButtonGroup->checkedButton();
    QAbstractButton * ml     = ui->MLButtonGroup->checkedButton();
    QAbstractButton * vis    = ui->VISButtonGroup->checkedButton();
    QAbstractButton * dr     = ui->DRButtonGroup->checkedButton();
    QAbstractButton * vr     = ui->VRButtonGroup->checkedButton();

    qDebug() << "prequestion button pressed";

    if (S.ifDebug || ( ( age != "" ) && ( ageInt >= 18 ) && ( ageInt <= 65 ) && gender && ml && vis && vr ) ) {
        if (!S.ifDebug) {
            ppData.prequestionFinishTimestamp = QDateTime::currentMSecsSinceEpoch();
            ppData.age         = age.toInt();
            ppData.gender      = gender->text().trimmed();
            ppData.familiarML  = ml->accessibleName().toInt();
            ppData.familiarVIS = vis->accessibleName().toInt();
            ppData.familiarDR  = dr->accessibleName().toInt();
            ppData.familiarVR  = vr->accessibleName().toInt();
        }

        qDebug() << "age: " << ppData.age << ", gender: " << ppData.gender
                 << ", ml: " << ppData.familiarML << ", vis: " << ppData.familiarVIS
                 << ", dr: " << ppData.familiarDR << ", vr: " << ppData.familiarVR;

        S.shared_partIndex += 1;

        this->setEnabled(false);

        emit callNextWindow();

        this->close();
        this->hide();
        this->destroy();
    } else {
        QMessageBox msgBox;
        msgBox.setText("Please answer all the questions");
        msgBox.exec();
    }
}
