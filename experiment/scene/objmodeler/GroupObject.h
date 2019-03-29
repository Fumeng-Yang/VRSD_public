#ifndef GROUPOBJECT_H
#define GROUPOBJECT_H

#include <QVector>
#include "PrimitiveObject.h"

/**
 * @brief The GroupObject class
 * The groupobject contains a set of primitive objects
 *
 */
class GroupObject : public SceneObject
{
public:
   GroupObject();
   ~GroupObject();
   // add a primitive object into this group
   void addPrimitiveObject(PrimitiveObject *obj);

   // draw all the objects in this group
   void draw(QOpenGLShaderProgram& shader,
             QMap<QString, QOpenGLTexture *>& txtMap) override;

private:
   QVector<PrimitiveObject *> m_primObjects;
};

#endif // GROUPOBJECT_H
