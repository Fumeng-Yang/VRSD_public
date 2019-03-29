#include "breakwindow.h"
#include "ui_breakwindow.h"
#include "settings.h"
#include <cstring>
#include <QTimer>
BreakWindow::BreakWindow(QWidget * parent, AbstractBehavior * next) :
        AbstractBehavior(parent, next),
        ui(new Ui::BreakWindow),
        m_time(10) {//sec
        this->setWindowState(Qt::WindowMaximized);
        ui->setupUi(this);
        ui->nextButton->setEnabled(false);
        timer = new QTimer(static_cast<QMainWindow *>( this ) );
        QMainWindow::connect(timer, SIGNAL(timeout()), static_cast<QMainWindow *>( this ), SLOT(checkTime()) );
}


BreakWindow::~BreakWindow() {
        delete ui;
}


void BreakWindow::setNextWindow(AbstractBehavior * next) {
        this->next = next;

        connect(this, SIGNAL(callNextWindow()), next, SLOT(receiveCall()) );
}


void BreakWindow::keyPressEvent(QKeyEvent * event) {

}


void BreakWindow::prepareWindow() {
        this->setWindowTitle(S.fixed_windowName + "Break" + "~" + ppData.participantID);

        this->setEnabled(true);

        this->show();

        ui->instructionsBox->setFixedSize(S.fixed_windowWidth - S.fixed_windowLeftRightMargin * 2,
                                          ui->instructionsBox->height() ); // no change to height
}


void BreakWindow::setComponents() {
        m_time = S.conf_breakingTime;

        ui->nextButton->setEnabled(false);

        QString session = S.conf_baseInstructions[S.shared_partIndex][0];
        QString remark  = S.conf_baseInstructions[S.shared_partIndex][1];
        QString remark2 = S.conf_baseInstructions[S.shared_partIndex][2];

        if (session.contains("break") ) {
                ui->instructionsBox->setHtml(remark);
                ui->instructionsBox->setReadOnly(true);
        }

        timer->start(1000);
}


void BreakWindow::checkTime() {
        m_time--;
        if (m_time >= 0) {
                int m        = m_time / 60;
                int s        = m_time - m * 60;
                QString timeText = "<p>";
                timeText += QString::number(m) + " m : " + QString::number(s) + " s </p>";
                ui->timeLabel->setHtml(timeText);
        } else {
                ui->nextButton->setEnabled(true);
                timer->stop();
        }
}


void BreakWindow::on_nextButton_clicked() {
        S.shared_partIndex++;
        S.shared_sessionIndex += 1;

        this->setEnabled(false);

        emit callNextWindow();

        this->hide();
        this->close();
        this->destroy();

//   this->destroy();
}
