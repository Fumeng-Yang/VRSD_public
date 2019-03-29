#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <QString>
#include <QGL>

class ModelLoader
    {
   public:
    ModelLoader();
    void loadVerticesNormalsUVs(const QString & path, QVector<GLfloat> & results);
    void loadVerticesNormals(const QString & path, QVector<GLfloat> & results);
    void loadVertices(const QString & path, QVector<GLfloat> & results);
    };

#endif // MODELLOADER_H
