#include "viswindow.h"
#include "ui_viswindow.h"

#include "settings.h"
#include <QVBoxLayout>
#include <QSurfaceFormat>
#include <QSettings>
#include <QFileDialog>
#include <QStandardPaths>
#include <QOffscreenSurface>
#include <iostream>
#include <QDateTime>
#include <QWindow>
VisWindow::VisWindow(QWidget * parent, AbstractBehavior * next) :
    AbstractBehavior(parent, next),
    ui(new Ui::VisWindow) {
    this->setWindowState(Qt::WindowMaximized);

    viswidget = new VisWidget(this);

    ui->setupUi(this);
    ui->layout->addWidget(viswidget);

    connect(this, SIGNAL(callNextWindow()), next, SLOT(receiveCall()) );
}


VisWindow::~VisWindow() {
    delete ui;
}


void VisWindow::keyPressEvent(QKeyEvent * event) {
    //   qDebug() << "keyPressEvent captured in VisWindow";
}


void VisWindow::prepareWindow() {
    this->setWindowTitle(S.fixed_windowName + "Main" + "~" + ppData.participantID);
    this->setEnabled(true);
    this->show();

    viswidget->setFixedSize(S.fixed_windowWidth, S.fixed_windowHeight);

    viswidget->setSessionControl();

    viswidget->initDevices();

    viswidget->startRendering();
}


void VisWindow::setComponents() {
}


void VisWindow::nextWindow() {
}


QString VisWindow::nextPart() {
    int partIndex           = S.shared_partIndex;
    int sessionIndex = S.shared_sessionIndex;

    QString sessionName         = S.conf_baseInstructions[partIndex][0];
    if (sessionName.contains("practice") ) {
        S.shared_partIndex += 1;
    } else if (sessionName.contains("interval") ) {
        S.shared_partIndex += 1;
    } else if (sessionName.contains("main") ) {
        S.shared_partIndex += 1;
    } else if (sessionName.contains("endtrials") ) {
        S.shared_partIndex += 1;

        viswidget->stopRendering();
        // will force to clean scene
        viswidget->cleanupSession();

        viswidget->setEnabled(false);

        this->setEnabled(false);

        emit callNextWindow();

        this->hide();

        this->close();

        this->destroy();

    } else if (sessionName.contains("openquestions") ) {
        qDebug() << "openquestions: should not reach here from VisWindow::nextPart()";
    }
    return "nothing";
}
