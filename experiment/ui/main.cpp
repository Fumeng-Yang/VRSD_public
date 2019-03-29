#include "setupwindow.h"
#include "overviewwindow.h"
#include "prequeswindow.h"
#include "taskintrowindow.h"
#include "sessionintrowindow.h"
#include "viswindow.h" // main and practice
#include "openquestionswindow.h"
#include "breakwindow.h"
#include "postqueswindow.h"
#include "endwindow.h"
#include "settings.h"
#include <QApplication>
#include <QSurfaceFormat>
#include <QMessageBox>
#include <QSettings>
#include <QDir>
#include <iostream>
#include <chrono>
#include <windows.h>

int main(int argc, char * argv[]) {

    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

    QCoreApplication::setOrganizationName("Brown University");
    QCoreApplication::setOrganizationDomain("cs.brown.edu");
    QCoreApplication::setApplicationName("~Experiment~");
    QSurfaceFormat glFormat;

    glFormat.setVersion(4, 5);
    glFormat.setProfile(QSurfaceFormat::CoreProfile);
    glFormat.setSwapInterval(0);
    glFormat.setOption(QSurfaceFormat::DebugContext);
    glFormat.setSamples(S.fixed_samples);

    QSurfaceFormat::setDefaultFormat(glFormat);
    QApplication a(argc, argv);
    QMessageBox msgBox;

    EndWindow ew(nullptr, nullptr);

    PostQuesWindow pstqw(nullptr, &ew);

    BreakWindow bw1(nullptr, nullptr), bw2(nullptr, nullptr), bw3(nullptr, nullptr);
    OpenQuestionsWindow oqw1(nullptr, &bw1), oqw2(nullptr, &bw2), oqw3(nullptr, &bw3), oqw4(nullptr, &pstqw);
    VisWindow vis1(nullptr, &oqw1), vis2(nullptr, &oqw2), vis3(nullptr, &oqw3), vis4(nullptr, &oqw4);
    SessionIntroWindow siw1(nullptr, &vis1), siw2(nullptr, &vis2), siw3(nullptr, &vis3), siw4(nullptr, &vis4);

    TaskIntroductionWindow tiw2(nullptr, &siw1);
    TaskIntroductionWindow tiw1(nullptr, &tiw2);

    PreQuesWindow pqw(nullptr, &tiw1);

    OverviewWindow ow(nullptr, &pqw);

    bw1.setNextWindow(&siw2);
    bw2.setNextWindow(&siw3);
    bw3.setNextWindow(&siw4);

    SetupWindow sw(nullptr, &ow);

    S.shared_headerPath = QDir::currentPath();

    sw.setEnabled(true);
    sw.show();

    return a.exec();
}
