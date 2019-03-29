#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <QString>
#include <string.h>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>
#include <QOpenGLTexture>
#include <QElapsedTimer>
#include "BoundingBox.h"
enum ObjectType {
   SCENE_OBJECT,
   PRIMITIVE_OBJECT,
   GROUP_OBJECT,
   PRIMITIVE_IN_GROUP
};

/**
 * @brief The SceneObject class
 */
class SceneObject
{
public:
   SceneObject();
   // the name of the object
   // for group object, it is the name of the group
   QString getName();
   glm::mat4x4 getModelMatrix();
   ObjectType getObjectType();

   // return the bounding box of this object in box
   void getBox(BoundingBox& box);

   // update the bounding box of this object when adding an extra vertice
   void updateBox(const glm::vec3& p);

   // set the name of the object, for group object, it is the name of the group
   void setName(const QString& str);
   void setModelMatrix(glm::mat4x4& mat);

   // update model matrix by first transforming it back to origin (xreference matrix)
   // the times the pose matrix (where the controller is)
   void updateModelMatrixFromReference(const glm::mat4x4& poseMat);
   void resetModelMatrix();
   void resetReferenceMatrx();
   void setReferenceMatrx(glm::mat4x4& mat);
   void setObjectType(ObjectType t);
   void setPickable(bool b);
   void setActive(bool b);
   void setIsPicked(bool b);
   bool isPickable();
   bool isActive();
   bool isPicked();
   bool isSetted();

   virtual void draw(QOpenGLShaderProgram& shader,
                     QMap<QString, QOpenGLTexture *>& txtMap) {}

   virtual int getNumVertices() {
      return 0;
   }

protected:
   QString m_name;
   BoundingBox m_box;
   glm::mat4x4 m_modelMat;
   // the reference matrix represents the matrix that transforms the object back to the origin
   // it is set when collision happens
   glm::mat4x4 m_referenceMat;
   ObjectType m_type;
   bool m_pickable;
   bool m_isPicked;
   // if this object is active, active object will be drawn on the screeen
   bool m_isActive;
   int m_setted;
   // the inner timer for this object, can be used in animation
   QElapsedTimer m_timer;
};

#endif // SCENEOBJECT_H
