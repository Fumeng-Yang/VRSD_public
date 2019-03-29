#include "modelloader.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>

ModelLoader::ModelLoader()
{

}
void ModelLoader::loadVerticesNormalsUVs(const QString & path, QVector<GLfloat> & results){
    QFile objFile(path);
    if (!objFile.exists() ) {
        qDebug() << "can't read the file " << path;
        exit(1);
    } else if (objFile.open(QIODevice::ReadOnly) ) {
        QTextStream in(&objFile);
        QVector<GLfloat> verts, uvs, ns;
        results.clear();
        while (!in.atEnd() )
        {
            QString line = in.readLine();
            QStringList data = line.simplified().split(" ");
            while (!in.atEnd() )
            {
                line = in.readLine();
                data = line.simplified().split(" ");

                if ( ( data.length() > 0 ) && ( data[0] == "v" ) ) {
                    float v1 = data[1].toFloat(), v2 = data[2].toFloat(), v3 = data[3].toFloat();
                    // add vertices
                    verts.append(v1);
                    verts.append(v2);
                    verts.append(v3);
                } else if ( ( data.length() > 0 ) && ( data[0] == "vt" ) ) {
                    // add uv coordinates
                    uvs.append(data[1].toFloat() );
                    uvs.append(data[2].toFloat() );
                } else if ( ( data.length() > 0 ) && ( data[0] == "vn" ) ) {
                    // add normals
                    ns.append(data[1].toFloat() );
                    ns.append(data[2].toFloat() );
                    ns.append(data[3].toFloat() );
                } else if ( ( data.length() > 0 ) && ( data[0] == "f" ) ) {  // this is triangle
                    QStringList row;
                    for (int i = 1; i < 4; i++) {
                        row = data[i].split("/");
                        int v  = row[0].toInt() - 1;
                        int vt = row[1].toInt() - 1;
                        int n  = row[2].toInt() - 1;
                        for (int j = 0; j < 3; j++) {
                            results.append(verts.at(v * 3 + j) );
                        }
                        for (int j = 0; j < 2; j++) {
                            if (vt == -1) {
                                results.append(0);
                            } else {
                                results.append(uvs.at(vt * 2 + j) );
                            }
                        }
                        for (int j = 0; j < 3; j++) {
                            if (n == -1) {
                                results.append(0);
                            } else {
                                results.append(ns.at(n * 3 + j) );
                            }
                        }
                    }
                }
            }
        }

    }
}


void ModelLoader::loadVerticesNormals(const QString & path, QVector<GLfloat> & results){
    QFile objFile(path);
    if (!objFile.exists() ) {
        qDebug() << "can't read the file " << path;
        exit(1);
    } else if (objFile.open(QIODevice::ReadOnly) ) {
        QTextStream in(&objFile);
        QVector<GLfloat> verts, uvs, ns;
        results.clear();
        while (!in.atEnd() )
        {
            QString line = in.readLine();
            QStringList data = line.simplified().split(" ");
            while (!in.atEnd() )
            {
                line = in.readLine();
                data = line.simplified().split(" ");

                if ( ( data.length() > 0 ) && ( data[0] == "v" ) ) {
                    float v1 = data[1].toFloat(), v2 = data[2].toFloat(), v3 = data[3].toFloat();
                    // add vertices
                    verts.append(v1);
                    verts.append(v2);
                    verts.append(v3);
                } else if ( ( data.length() > 0 ) && ( data[0] == "vt" ) ) {
                    // add uv coordinates
                    uvs.append(data[1].toFloat() );
                    uvs.append(data[2].toFloat() );
                } else if ( ( data.length() > 0 ) && ( data[0] == "vn" ) ) {
                    // add normals
                    ns.append(data[1].toFloat() );
                    ns.append(data[2].toFloat() );
                    ns.append(data[3].toFloat() );
                } else if ( ( data.length() > 0 ) && ( data[0] == "f" ) ) {  // this is triangle
                    QStringList row;
                    for (int i = 1; i < 4; i++) {
                        row = data[i].split("/");
                        int v  = row[0].toInt() - 1;
                        int vt = row[1].toInt() - 1;
                        int n  = row[2].toInt() - 1;
                        for (int j = 0; j < 3; j++) {
                            results.append(verts.at(v * 3 + j) );
                        }
                        for (int j = 0; j < 3; j++) {
                            if (n == -1) {
                                results.append(0);
                            } else {
                                results.append(ns.at(n * 3 + j) );
                            }
                        }
                    }
                }
            }
        }

    }
}


void ModelLoader::loadVertices(const QString & path, QVector<GLfloat> & results){
    QFile objFile(path);
    if (!objFile.exists() ) {
        qDebug() << "can't read the file " << path;
        exit(1);
    } else if (objFile.open(QIODevice::ReadOnly) ) {
        QTextStream in(&objFile);
        QVector<GLfloat> verts, uvs, ns;
        results.clear();
        while (!in.atEnd() )
        {
            QString line = in.readLine();
            QStringList data = line.simplified().split(" ");
            while (!in.atEnd() )
            {
                line = in.readLine();
                data = line.simplified().split(" ");

                if ( ( data.length() > 0 ) && ( data[0] == "v" ) ) {
                    float v1 = data[1].toFloat(), v2 = data[2].toFloat(), v3 = data[3].toFloat();
                    // add vertices
                    verts.append(v1);
                    verts.append(v2);
                    verts.append(v3);
                } else if ( ( data.length() > 0 ) && ( data[0] == "vt" ) ) {
                    // add uv coordinates
                    uvs.append(data[1].toFloat() );
                    uvs.append(data[2].toFloat() );
                } else if ( ( data.length() > 0 ) && ( data[0] == "vn" ) ) {
                    // add normals
                    ns.append(data[1].toFloat() );
                    ns.append(data[2].toFloat() );
                    ns.append(data[3].toFloat() );
                } else if ( ( data.length() > 0 ) && ( data[0] == "f" ) ) {  // this is triangle
                    QStringList row;
                    for (int i = 1; i < 4; i++) {
                        row = data[i].split("/");
                        int v  = row[0].toInt() - 1;
                        int vt = row[1].toInt() - 1;
                        int n  = row[2].toInt() - 1;
                        for (int j = 0; j < 3; j++) {
                            results.append(verts.at(v * 3 + j) );
                        }

                    }
                }
            }
        }

    }
}
