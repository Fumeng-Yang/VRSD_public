#include "setupwindow.h"
#include "ui_setupwindow.h"
#include "settings.h"
#include "abstractbehavior.h"
#include <iostream>
#include <cmath>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QJsonArray>
#include <QDir>

SetupWindow::SetupWindow(QWidget * parent, AbstractBehavior * next) :
    AbstractBehavior(parent, next),
    ui(new Ui::SetupWindow) {
    this->setWindowState(Qt::WindowMaximized);
    ui->setupUi(this);

    connect(this, SIGNAL(callNextWindow()), next, SLOT(receiveCall()) );
    ui->heightField->setValidator(new QDoubleValidator(1.0, 2.0, 2, static_cast<QMainWindow *>( this ) ) );
}


SetupWindow::~SetupWindow() {
    delete ui;
}


void SetupWindow::keyPressEvent(QKeyEvent * event) {
}


void SetupWindow::prepareWindow() {
    this->setWindowTitle(S.fixed_windowName + ppData.participantID);

    this->setEnabled(true);
    this->show();

}


void SetupWindow::setComponents() {
}


void SetupWindow::parseTrialData(QString path, TrialControl & t, int npt, int dim, float ta) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly) ) {
        qDebug() << file.errorString();
    }
    QTextStream in(&file);

    if (dim == 2) {
        t.p2d = new float *[npt];
    } else {
        t.p3d = new float *[npt];
    }

    t.labels      = new int[npt]();
    t.correctness = new bool[npt]();

    while (!in.atEnd() )
    {
        QString line = in.readLine();
        if (line == "test_accuracy") {
            QString nline = in.readLine();
            float ta1   = nline.toDouble();

            // check accuracy
            assert(std::abs(ta - ta1) < 0.0001);
        } else if ( ( dim == 2 ) && ( ( line == "p2d" ) || ( line == "tsne2d" ) ) ||
                        ( ( dim == 3 ) && ( ( line == "p3d" ) || ( line == "tsne3d" ) ) ) ) {
            QString nline  = in.readLine();
            QStringList points = nline.split(";");

            // parse a point
            int pIndex = 0;
            for (QString p : points) {
                QStringList coords = p.split(",");
                if (dim == 2) {
                    t.p2d[pIndex] = new float[2]();
                } else if (dim == 3) {
                    t.p3d[pIndex] = new float[3]();
                }
                int cIndex = 0;

                // parse each coordinate
                for (QString c :coords) {
                    float v = c.toDouble() * S.magic_scalingFactor;
                    if (dim == 2) {
                        t.p2d[pIndex][cIndex] = v;
                    } else if (dim == 3) {
                        t.p3d[pIndex][cIndex] = v;
                    }
                    assert(cIndex <= dim);
                    cIndex++;
                }
                assert(pIndex <= npt);
                pIndex++;
            }
        } else if (line == "labels") {
            // parse a label
            QString nline  = in.readLine();
            QStringList labels = nline.split(",");
            int li     = 0;
            for (QString l : labels) {
                t.labels[li] = l.toInt();

                // a hack to resolve labeling proble,
                if (path.contains("babi") ) {
                    switch (l.toInt() )
                    {
                    case 12:
                        t.labels[li] = 3;
                        break; // prints "1",

                    case 9:
                        t.labels[li] = 0;
                        break; // then prints "2"

                    case 15:
                        t.labels[li] = 5;
                        break;    // then prints "2"

                    case 11:
                        t.labels[li] = 2;
                        break;   // then prints "2"

                    case 17:
                        t.labels[li] = 7;
                        break; // then prints "2

                    case 10:
                        t.labels[li] = 1;
                        break;

                    default: // compilation error: jump to default: would enter the scope of 'x'
                        // without initializing it
                        qDebug() << "find an weird label";
                        break;
                    }
                }
                li++;
            }
        } else if (line == "pred_correctness") {
            QString nline       = in.readLine();
            QStringList correctness = nline.split(",");
            int ci          = 0;
            for (QString c : correctness) {
                t.correctness[ci] = c.toInt();
                ci++;
            }
        }
    }

    file.close();
}


void SetupWindow::parseDatasets() {
    for (int s = 0; s < ppControl.num_sessions; s++) {
        SessionControl * session = &( ppControl.sessions[s] );

        int num_practice = session->num_practice;
        int num_main     = session->num_main;
        int num_points   = session->num_points;

        for (int p = 0; p < num_practice; p++) {
            TrialControl * t   = &( ( session->practice_trials )[p] );
            QString path = S.shared_headerPath + "/datasets/" + t->referred_path;
            int dim  = 3;

            if (t->remark == "2d") {
                dim = 2;
            }
            parseTrialData(path, *t, num_points, dim, t->test_accuracy);
        }


        for (int m = 0; m < num_main; m++) {
            TrialControl * t   = &( ( session->main_trials )[m] );
            QString path = S.shared_headerPath + "/datasets/" + t->referred_path;
            int dim  = 3;

            if (t->remark == "2d") {
                dim = 2;
            }
            parseTrialData(path, *t, num_points, dim, t->test_accuracy);
        }
    }
}


void SetupWindow::parseParticipantProfile() {
    QFile file;
    QString path = QDir::currentPath() + "/datasets/participants/" + ppControl.participantID + ".json";
    file.setFileName(path);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString val = file.readAll();
    file.close();

    QJsonDocument d     = QJsonDocument::fromJson(val.toUtf8() );
    QJsonObject obj   = d.object();
    QJsonArray array = obj["sessions"].toArray();
    ppControl.sessions        = new SessionControl[4];
    S.shared_projectionMethod = obj["projectionMethod"].toString();

    int si = 0;
    for (const QJsonValue & value : array) {
        QJsonObject content = value.toObject();

        //**********************************************
        // set up session headers
        // **********************************************
        QJsonObject factor_prior = content["factors_prior"].toObject();
        int num_practice = content["num_practice"].toInt(),
            num_main     = content["num_main"].toInt(),
            num_repeats  = content["num_repeats"].toInt(),
            num_points   = content["num_points"].toInt(),
            num_classes  = content["num_classes"].toInt();

        int session_num = content["session_num"].toInt();
        QString data_name   = content["data_name"].toString();

        SessionControl * session = new SessionControl(session_num,
                                                      factor_prior["stereoscopy"].toString(),
                                                      factor_prior["motion"].toString(),
                                                      num_practice,
                                                      num_main,
                                                      num_repeats,
                                                      num_points,
                                                      num_classes,
                                                      data_name);
        qDebug() << session->factor_sterescopy << ":" << session->factor_motion \
                 << ":" << session->num_practice << ":" << session->num_main    \
                 << ":" << data_name << ":" << num_points << ":" << num_classes;


        //**********************************************
        // declare trials
        // **********************************************
        session->main_trials     = new TrialControl[num_main];
        session->practice_trials = new TrialControl[num_practice];

        QJsonArray factors_practice = content["factors_practice_trials"].toArray();

        //**********************************************
        // init practice trials
        // **********************************************
        int practiceIndex = 0;
        for (const QJsonValue & tvalue : factors_practice) {
            QJsonObject trial = tvalue["trial"].toObject();
            TrialControl * pt   = session->practice_trials;
            TrialControl * t    = new TrialControl(
            factor_prior["stereoscopy"].toString(),
            factor_prior["motion"].toString(),
            tvalue["shading"].toString(),
            tvalue["perspective"].toString(),
            tvalue["remark"].toString(),
            float(trial["test_accuracy"].toDouble() ),
            trial["int_accuracy"].toInt(),
            trial["referred_file"].toString(),
            trial["referred_path"].toString(),
            float(trial["bin_lower"].toDouble() ),
            float(trial["bin_upper"].toDouble() ),
            float(trial["step"].toDouble() ),
            trial["bin_index"].toInt(),
            num_points, num_classes, "practice", data_name
            );

            pt[practiceIndex] = *( t );

            practiceIndex++;
        }

        //**********************************************
        // init main trials
        // **********************************************
        QJsonArray factors_main = content["factors_main_trials"].toArray();

        int mainIndex = 0;
        for (const QJsonValue & mvalue : factors_main) {
            QJsonObject trial = mvalue["trial"].toObject();
            TrialControl * mt   = session->main_trials;
            TrialControl * t    = new TrialControl(
            factor_prior["stereoscopy"].toString(),
            factor_prior["motion"].toString(),
            mvalue["shading"].toString(),
            mvalue["perspective"].toString(),
            mvalue["remark"].toString(),
            float(trial["test_accuracy"].toDouble() ),
            trial["int_accuracy"].toInt(),
            trial["referred_file"].toString(),
            trial["referred_path"].toString(),
            float(trial["bin_lower"].toDouble() ),
            float(trial["bin_upper"].toDouble() ),
            float(trial["step"].toDouble() ),
            trial["bin_index"].toInt(),
            num_points, num_classes, "main", data_name
            );
            mt[mainIndex] = *( t );

            //         qDebug() << trial["bin_lower"].toDouble() << ":" << \
            //            trial["bin_upper"].toDouble() << ":" <<          \
            //            trial["step"].toDouble() << ":" <<               \
            //            trial["bin_index"].toInt();

            mainIndex++;
        }
        if(si == 0){
            ppControl.firstDataName = data_name;
        }
        ppControl.sessions[si] = *session;
        si++;
    }

    ppData.sessionData = new SessionData[si];

    //****************************************************************
    // for each trial data, adding a pointer back to the trial data
    // ***************************************************************

    for (int i = 0; i < si; i++) {
        SessionData    * sd   = new SessionData();
        SessionControl * back = &( ppControl.sessions[i] );

        TrialData * tp = new TrialData[back->num_practice]();
        sd->practiceTrials = tp;
        sd->sessionBack       = back;

        for (int np = 0; np < back->num_practice; np++) {
            ( ( sd->practiceTrials )[np] ).trialBack = &( ( ( ppControl.sessions[i] ).practice_trials )[np] );
        }

        TrialData * tm = new TrialData[back->num_main]();
        sd->mainTrials = tm;

        for (int nm = 0; nm < back->num_main; nm++) {
            ( ( sd->mainTrials )[nm] ).trialBack = &( ( ( ppControl.sessions[i] ).main_trials )[nm] );
        }

        ppData.sessionData[i] = *sd;
    }
}


void SetupWindow::on_nextButton_clicked() {
    QString id = ui->participantIDBox->text();
    float height = ( ui->heightField->text() ).toFloat();
    ppControl.participantID    = id;
    ppControl.participantIndex = id.mid(0).toInt();

    ppData.participantID    = ppControl.participantID;
    ppData.participantIndex = ppControl.participantIndex;
    ppData.participantHeight = height;

    parseParticipantProfile();
    parseDatasets();

    S.shared_partIndex += 1;
    S.shared_participantHeight += height;

    this->setEnabled(false);

    emit callNextWindow();

    this->close();
    this->hide();
    this->destroy();

    qDebug() << "set up participant ID: " << ppControl.participantIndex << ";" <<  S.shared_participantHeight;
}
