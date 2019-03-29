#ifndef PRIMITIVEOBJECT_H
#define PRIMITIVEOBJECT_H

#include <QVector>
#include <QGL>
#include <QString>
#include <QOpenGLTexture>
#include <memory>
#include "material.h"
#include "SceneObject.h"
#include "glm/glm.hpp"

/**
 * @brief The PrimitiveObject class
 * The primitive object means that this object only has one material
 */
class PrimitiveObject : public SceneObject
{
   friend class GroupObject;
public:
   PrimitiveObject();
   ~PrimitiveObject();
   // this is not used in this program because we don't have vertices for each object
   void setVertices(const QVector<GLfloat>& verts);
   void setMaterial(const Material& mtl);

   // the offset from the first in the big vertices array
   void setOffset(int offset);

   // set how many tuples this object have (actually one vertice, 3 coordinates + 2 uv + 3 normals + ...)
   void setNumVertices(int num);

   // how many vertices for this primitive object
   int getNumVertices() override;

   // draw the object using the provide shader, textmap
   void draw(QOpenGLShaderProgram& shader,
             QMap<QString, QOpenGLTexture *>& txtMap) override;

protected:
   // all the vertices for this object
   // this is not used in RoomInSpace, but leave for the future usage
   QVector<GLfloat> m_vertices;
   // the material of this object
   // currently, one object could only have one material
   Material m_material;
   // where it starts in the grant vertices array
   int m_offset;
   // how many vertices for this object
   int m_numVertices;
};

#endif // PRIMITIVEOBJECT_H
