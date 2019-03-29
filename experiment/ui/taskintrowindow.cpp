#include "taskintrowindow.h"
#include "ui_taskintrowindow.h"
#include "settings.h"
#include <iostream>
#include <QImage>
#include <QDir>
#include <QPicture>
#include <QPainter>
#include <QDateTime>

TaskIntroductionWindow::TaskIntroductionWindow(QWidget * parent, AbstractBehavior * next) :
    AbstractBehavior(parent, next),
    ui(new Ui::TaskIntroWindow) {
    this->setWindowState(Qt::WindowMaximized);
    ui->setupUi(this);

    connect(this, SIGNAL(callNextWindow()), next, SLOT(receiveCall()) );
}


TaskIntroductionWindow::~TaskIntroductionWindow() {
    delete ui;
}


void TaskIntroductionWindow::keyPressEvent(QKeyEvent * event) {
    //   qDebug() << "keyPressEvent captured in TaskIntroductionWindow";
}


void TaskIntroductionWindow::setComponents() {
    QString session = S.conf_baseInstructions[S.shared_partIndex][0];
    QString remark  = S.conf_baseInstructions[S.shared_partIndex][1];
    QString remark2 = S.conf_baseInstructions[S.shared_partIndex][2];

    if (session.contains("taskIntroduction") ) {
        ui->instructionsBox->setHtml(remark);
        ui->instructionsBox->setReadOnly(true);
        ui->instructions2Box->setHtml(remark2);
        ui->instructions2Box->setReadOnly(true);

        if(session.contains("1") ) {
            QString path;
            if(ppControl.firstDataName.contains("babi") ) {
                path = S.conf_taskIntrodcutionImagePath1Babi;
            } else {
                path = S.conf_taskIntrodcutionImagePath1Cifar10;
            }
            QPixmap img(path);
            ui->imageBox->setPixmap(img);
        } else {
            QPixmap img(S.conf_taskIntrodcutionImagePath2);
            ui->imageBox->setPixmap(img);
        }
    }
}


void TaskIntroductionWindow::prepareWindow() {
    this->setWindowTitle(S.fixed_windowName + "Task" + "~" + ppData.participantID);
    this->setEnabled(true);
    this->show();

    QString session = S.conf_baseInstructions[S.shared_partIndex][0];
    if(session.contains("1") ) {
        ui->instructionsBox->setFixedSize(S.fixed_windowWidth - S.fixed_windowLeftRightMargin * 2, S.fixed_windowHeight / 5);
        ui->imageBox->setFixedSize(S.fixed_windowWidth - S.fixed_windowLeftRightMargin * 2, S.fixed_windowHeight / 2);
        ui->instructions2Box->setFixedSize(S.fixed_windowWidth - S.fixed_windowLeftRightMargin * 2, S.fixed_windowHeight / 7.5);
        //-----
        ppData.taskIntro1StartTimestamp = QDateTime::currentMSecsSinceEpoch();
        //-----
    } else {
        ui->instructionsBox->setFixedSize(S.fixed_windowWidth - S.fixed_windowLeftRightMargin * 2, S.fixed_windowHeight / 4);
        ui->imageBox->setFixedSize(S.fixed_windowWidth - S.fixed_windowLeftRightMargin * 2, S.fixed_windowHeight / 3.5);
        ui->instructions2Box->setFixedSize(S.fixed_windowWidth - S.fixed_windowLeftRightMargin * 2, S.fixed_windowHeight / 5);
    }
    ui->verticalLayout->update();
}


void TaskIntroductionWindow::on_nextButton_clicked() {
    QString session = S.conf_baseInstructions[S.shared_partIndex][0];
    if(session.contains("1") ) {
        ppData.taskIntro1FinishTimestamp = QDateTime::currentMSecsSinceEpoch();
    } else {
        ppData.taskIntro2FinishTimestamp = QDateTime::currentMSecsSinceEpoch();
    }

    S.shared_partIndex++;
    S.shared_sessionIndex = 0;

    this->setEnabled(false);

    emit callNextWindow();

    this->close();
    this->hide();
    this->destroy();

}
