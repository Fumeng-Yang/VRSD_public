#include "endwindow.h"
#include "ui_endwindow.h"
#include "settings.h"

EndWindow::EndWindow(QWidget * parent, AbstractBehavior * next) :
        AbstractBehavior(parent, next),
        ui(new Ui::EndWindow) {
        this->setWindowState(Qt::WindowMaximized);
        ui->setupUi(this);
//    connect(this, SIGNAL(callNextWindow()), next, SLOT(receiveCall()));
}


EndWindow::~EndWindow() {
        delete ui;
}


void EndWindow::prepareWindow()     {
        this->setWindowTitle(S.fixed_windowName + "End" + "~" + ppData.participantID);
        this->setEnabled(true);
        this->show();

        int w = this->width(), h = this->height();
        ui->instructionBox->setReadOnly(true);
        ui->instructionBox->setFixedSize(S.fixed_windowWidth - S.fixed_windowLeftRightMargin * 2,
                                         h - S.fixed_windowTopBottomMargin * 2 - S.fixed_componentMargin);
}


void EndWindow::keyPressEvent(QKeyEvent * event) {
}


void EndWindow::setComponents() {
}

void EndWindow::on_nextButton_clicked() {
        // clean up everything
        this->close();
        this->destroy();
}
