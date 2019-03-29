#include "GroupObject.h"

GroupObject::GroupObject() : m_primObjects(QVector<PrimitiveObject *>())
{}

/**
 * @brief GroupObject::~GroupObject
 * Clean up
 */
GroupObject::~GroupObject() {
   for (PrimitiveObject *obj : m_primObjects) {
      delete obj;
   }
}


/**
 * @brief GroupObject::addPrimitiveObject
 * @param obj
 */
void GroupObject::addPrimitiveObject(PrimitiveObject *obj) {
   m_setted++;
   m_box.updateBox(obj->m_box);
   m_primObjects.push_back(obj);
}


/**
 * @brief GroupObject::draw
 * @param shader
 * @param txtMap
 */
void GroupObject::draw(QOpenGLShaderProgram& shader,
                       QMap<QString, QOpenGLTexture *>& txtMap) {
   for (PrimitiveObject *primObj : m_primObjects) {
      primObj->setModelMatrix(m_modelMat);
      primObj->draw(shader, txtMap);
   }
}
