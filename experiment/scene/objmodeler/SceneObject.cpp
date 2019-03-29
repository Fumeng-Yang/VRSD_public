#include "SceneObject.h"
#include <iostream>
#include <glm/ext.hpp>

SceneObject::SceneObject() : m_name(""),
   m_box(BoundingBox()),
   m_modelMat(glm::mat4x4()),
   m_referenceMat(glm::mat4x4()),
   m_type(SCENE_OBJECT),
   m_pickable(false),
   m_isPicked(false),
   m_isActive(true),
   m_setted(0),
   m_timer(QElapsedTimer()) {
   m_timer.start();
}


/**
 * @brief SceneObject::setName
 * @param str
 */
void SceneObject::setName(const QString& str) {
   m_name = str;
}


/**
 * @brief SceneObject::getName
 * @return
 */
QString SceneObject::getName() {
   return m_name;
}


/**
 * @brief SceneObject::resetModelMatrix
 */
void SceneObject::resetModelMatrix() {
   m_modelMat = glm::mat4x4();
}


/**
 * @brief SceneObject::setReferenceMatrx
 * @param mat
 */
void SceneObject::setReferenceMatrx(glm::mat4x4& mat) {
   m_referenceMat = glm::inverse(mat);
}


/**
 * @brief SceneObject::updateModelMatrixFromReference
 * update the model matrix of this object
 * reference matrix makes it back to the origin
 * pose matrix makes it at the position of the controller
 * @param poseMat
 */
void SceneObject::updateModelMatrixFromReference(const glm::mat4x4& poseMat) {
   m_modelMat = poseMat * m_referenceMat;
}


/**
 * @brief SceneObject::resetReferenceMatrx
 */
void SceneObject::resetReferenceMatrx() {
   m_referenceMat = glm::mat4x4();
}


/**
 * @brief SceneObject::updateBox
 * @param p
 */
void SceneObject::updateBox(const glm::vec3& p) {
   m_box.updateBox(p);
}


/**
 * @brief SceneObject::setObjectType
 * @param t
 */
void SceneObject::setObjectType(ObjectType t) {
   m_type = t;
}


/**
 * @brief SceneObject::getObjectType
 * @return
 */
ObjectType SceneObject::getObjectType() {
   return m_type;
}


/**
 * @brief SceneObject::getModelMatrix
 * @return
 */
glm::mat4x4 SceneObject::getModelMatrix() {
   return m_modelMat;
}


/**
 * @brief SceneObject::setModelMatrix
 * @param mat
 */
void SceneObject::setModelMatrix(glm::mat4x4& mat) {
   m_modelMat = mat;
   m_box.setModelMatrix(mat);
}


/**
 * @brief SceneObject::getBox
 * @param box
 */
void SceneObject::getBox(BoundingBox& box) {
   box = m_box;
}


/**
 * @brief SceneObject::setActive
 * @param b
 */
void SceneObject::setActive(bool b) {
   m_isActive = b;
}


/**
 * @brief SceneObject::setPickable
 * @param b
 */
void SceneObject::setPickable(bool b) {
   m_pickable = b;
}


/**
 * @brief SceneObject::setIsPicked
 * @param b
 */
void SceneObject::setIsPicked(bool b) {
   m_isPicked = b;
}


/**
 * @brief SceneObject::isActive
 * @return
 */
bool SceneObject::isActive() {
   return m_isActive;
}


bool SceneObject::isSetted() {
   return m_setted > 0;
}


/**
 * @brief SceneObject::isPicked
 * @return
 */
bool SceneObject::isPicked() {
   return m_isPicked;
}


/**
 * @brief SceneObject::isPickable
 * @return
 */
bool SceneObject::isPickable() {
   return m_pickable;
}
