#include "overviewwindow.h"
#include "ui_overviewwindow.h"
#include "settings.h"

#include <QString>
#include <QDateTime>

#include <iostream>


OverviewWindow::OverviewWindow(QWidget * parent, AbstractBehavior * next) :
        AbstractBehavior(parent, next),
        ui(new Ui::InstructionWindow) {
        this->setWindowState(Qt::WindowMaximized);

        ui->setupUi(this);

        connect(this, SIGNAL(callNextWindow()), next, SLOT(receiveCall()) );
}


OverviewWindow::~OverviewWindow() {
        delete ui;
}


void OverviewWindow::keyPressEvent(QKeyEvent * event) {
}


void OverviewWindow::setComponents() {
        this->setWindowTitle(S.fixed_windowName + "Overview" + "~" + ppData.participantID);

        QString session = S.conf_baseInstructions[S.shared_partIndex][0];
        QString remark  = S.conf_baseInstructions[S.shared_partIndex][1];

        if (session.contains("overview") ) {
                ui->instructionBox->setHtml(remark);
                ui->instructionBox->setReadOnly(true);
        }
}


void OverviewWindow::prepareWindow() {
        this->setEnabled(true);

        this->show();

//   grabKeyboard();

        int w = this->width(), h = this->height();
        ui->instructionBox->setFixedSize(S.fixed_windowWidth - S.fixed_windowLeftRightMargin * 2,
                                         h / 2);
        ui->layout->update();

}


void OverviewWindow::updateInstructions(std::string instr, std::string fig_path) {
        QString qsinstr = QString::fromStdString(instr);
        ui->instructionBox->setHtml("qsinstr");

        if (fig_path != "") {
                // TODO
                // load an image and
                // display the image
                // adjust the size
        }
}


void OverviewWindow::on_nextButton_clicked() {
        ppData.overviewFinishTimestamp = QDateTime::currentMSecsSinceEpoch();

        S.shared_partIndex += 1;

        this->setEnabled(false);

        emit callNextWindow();

        this->close();
        this->hide();
        this->destroy();
}
