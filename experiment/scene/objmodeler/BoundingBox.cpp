#include "BoundingBox.h"
#include <iostream>
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "Helpers.h"

/**
 * @brief BoundingBox::BoundingBox
 * initialize the bounding box to a infinite box
 */
BoundingBox::BoundingBox() :
   m_upperPlanes(glm::vec3(-FLT_MAX)), m_lowerPlanes(glm::vec3(FLT_MAX)),
   m_modelMat(glm::mat4x4()) {
   updateTranformedVertices();
}


/**
 * @brief BoundingBox::BoundingBox, copy constructor
 * @param box
 */
BoundingBox::BoundingBox(const BoundingBox& box) : m_modelMat(glm::mat4x4()) {
   m_upperPlanes = box.m_upperPlanes;
   m_lowerPlanes = box.m_lowerPlanes;
   updateTranformedVertices();
}


BoundingBox::BoundingBox(const glm::vec3& upper, const glm::vec3& lower) :
   m_upperPlanes(upper), m_lowerPlanes(lower), m_modelMat(glm::mat4x4()) {
   updateTranformedVertices();
}


/**
 * @brief BoundingBox::BoundingBox
 * compute the bounding box of a set of bounding boxes
 * @param boxes
 */
BoundingBox::BoundingBox(const std::vector<std::shared_ptr<BoundingBox> >& boxes) :
   m_upperPlanes(glm::vec4(-FLT_MAX)), m_lowerPlanes(glm::vec4(FLT_MAX)), m_modelMat(glm::mat4x4()) {
   for (auto box : boxes) {
      updateBox(box->m_lowerPlanes);
      updateBox(box->m_lowerPlanes);
   }
}


void BoundingBox::setModelMatrix(glm::mat4x4& mat) {
   m_modelMat = mat;
   updateTranformedVertices();
}


void BoundingBox::updateTranformedVertices() {
   m_vertices.clear();
   m_vertices.push_back(m_modelMat * glm::vec4(m_upperPlanes.x, m_upperPlanes.y, m_upperPlanes.z, 1.f));
   m_vertices.push_back(m_modelMat * glm::vec4(m_lowerPlanes.x, m_upperPlanes.y, m_upperPlanes.z, 1.f));
   m_vertices.push_back(m_modelMat * glm::vec4(m_lowerPlanes.x, m_lowerPlanes.y, m_upperPlanes.z, 1.f));
   m_vertices.push_back(m_modelMat * glm::vec4(m_upperPlanes.x, m_lowerPlanes.y, m_upperPlanes.z, 1.f));
   m_vertices.push_back(m_modelMat * glm::vec4(m_upperPlanes.x, m_upperPlanes.y, m_lowerPlanes.z, 1.f));
   m_vertices.push_back(m_modelMat * glm::vec4(m_lowerPlanes.x, m_upperPlanes.y, m_lowerPlanes.z, 1.f));
   m_vertices.push_back(m_modelMat * glm::vec4(m_lowerPlanes.x, m_lowerPlanes.y, m_lowerPlanes.z, 1.f));
   m_vertices.push_back(m_modelMat * glm::vec4(m_upperPlanes.x, m_lowerPlanes.y, m_lowerPlanes.z, 1.f));
}


glm::mat4x4 BoundingBox::getModelMatrix() {
   return m_modelMat;
}


void BoundingBox::check() {
   assert(m_upperPlanes.x >= m_lowerPlanes.x);
   assert(m_upperPlanes.y >= m_lowerPlanes.y);
   assert(m_upperPlanes.z >= m_lowerPlanes.z);
}


/**
 * @brief BoundingBox::print
 * print the upper and the lower bound for this
 */
void BoundingBox::print() {
   std::cout << "upper is " << glm::to_string(m_upperPlanes) << std::endl;
   std::cout << "lower is " << glm::to_string(m_lowerPlanes) << std::endl;
}


void BoundingBox::printVertices() {
   for (auto corner : m_vertices) {
      std::cout << "corners are " << glm::to_string(corner) << std::endl;
   }
}


/**
 * @brief BoundingBox::updateBox
 * udpate the bounding box according the input
 * @param p
 */
void BoundingBox::updateBox(const glm::vec3& p) {
   m_upperPlanes.x = std::fmax(p.x, m_upperPlanes.x);
   m_upperPlanes.y = std::fmax(p.y, m_upperPlanes.y);
   m_upperPlanes.z = std::fmax(p.z, m_upperPlanes.z);
   m_lowerPlanes.x = std::fmin(p.x, m_lowerPlanes.x);
   m_lowerPlanes.y = std::fmin(p.y, m_lowerPlanes.y);
   m_lowerPlanes.z = std::fmin(p.z, m_lowerPlanes.z);
   updateTranformedVertices();
   check();
}


/**
 * @brief BoundingBox::updateBox
 * udpate the bounding box according the input
 * @param p
 */
void BoundingBox::updateBox(const BoundingBox& box) {
   updateBox(box.m_lowerPlanes);
   updateBox(box.m_upperPlanes);
}


/**
 * @brief BoundingBox::updateBox
 * udpate the bounding box according the input
 * @param p
 */
void BoundingBox::updateBox(const glm::vec4& p) {
   m_upperPlanes.x = std::fmax(p.x, m_upperPlanes.x);
   m_upperPlanes.y = std::fmax(p.y, m_upperPlanes.y);
   m_upperPlanes.z = std::fmax(p.z, m_upperPlanes.z);
   m_lowerPlanes.x = std::fmin(p.x, m_lowerPlanes.x);
   m_lowerPlanes.y = std::fmin(p.y, m_lowerPlanes.y);
   m_lowerPlanes.z = std::fmin(p.z, m_lowerPlanes.z);
   updateTranformedVertices();
   check();
}


/**
 * @brief BoundingBox::updateLower
 * udpate the lower bound of the the bounding box according the input
 * @param p
 */
void BoundingBox::updateLower(const glm::vec3& p) {
   m_lowerPlanes.x = std::fmin(p.x, m_lowerPlanes.x);
   m_lowerPlanes.y = std::fmin(p.y, m_lowerPlanes.y);
   m_lowerPlanes.z = std::fmin(p.z, m_lowerPlanes.z);
   updateTranformedVertices();
   check();
}


/**
 * @brief BoundingBox::updateUpper
 * udpate the upper bound of the the bounding box according the input
 * @param p
 */
void BoundingBox::updateUpper(const glm::vec3& p) {
   m_upperPlanes.x = std::fmax(p.x, m_upperPlanes.x);
   m_upperPlanes.y = std::fmax(p.y, m_upperPlanes.y);
   m_upperPlanes.z = std::fmax(p.z, m_upperPlanes.z);
   updateTranformedVertices();
   check();
}


/**
 * @brief BoundingBox::getUpper
 * @return
 */
glm::vec3 BoundingBox::getUpper() {
   return m_upperPlanes;
}


/**
 * @brief BoundingBox::getLower
 * @return
 */
glm::vec3 BoundingBox::getLower() {
   return m_lowerPlanes;
}


/**
 * @brief BoundingBox::hit
 * Check if two bounding boxes overlap with each other
 * @param p
 * @param d
 */
bool BoundingBox::overlap(BoundingBox& box) {
   for (glm::vec4 x : box.m_vertices) {
      if (inside(x)) {
         return true;
      }
   }
   for (glm::vec4 x : m_vertices) {
      if (box.inside(x)) {
         return true;
      }
   }
   return false;
}


/**
 * @brief BoundingBox::inside : check if a vertex x is inside the current bounding box
 * even after transform
 * @param x
 * @return
 * http://math.stackexchange.com/questions/1472049/check-if-a-point-is-inside-a-rectangular-shaped-area-3d
 */
bool BoundingBox::inside(const glm::vec4& x) {
//   return hit(x, glm::vec4(1.0, 0, 0, 0)) < FLT_MAX &&
//          hit(x, glm::vec4(-1.0, 0, 0, 0)) < FLT_MAX;

   glm::vec4 u = m_vertices[0] - m_vertices[1];
   glm::vec4 v = m_vertices[0] - m_vertices[3];
   glm::vec4 w = m_vertices[0] - m_vertices[4];
   float     ux = glm::dot(u, x), up1 = glm::dot(u, m_vertices[0]), up2 = glm::dot(u, m_vertices[1]);
   float     vx = glm::dot(v, x), vp1 = glm::dot(v, m_vertices[0]), vp2 = glm::dot(v, m_vertices[3]);
   float     wx = glm::dot(w, x), wp1 = glm::dot(w, m_vertices[0]), wp2 = glm::dot(w, m_vertices[4]);

   return helper.between(ux, up1, up2) &&
          helper.between(vx, vp1, vp2) &&
          helper.between(wx, wp1, wp2);
}


/**
 * @brief BoundingBox::hit
 * @param p
 * @param d
 * @return
 */
float BoundingBox::hit(const glm::vec4& p, const glm::vec4& d) {
   float t = FLT_MAX;


   // top
   float t1 = (m_upperPlanes.y - p.y) / d.y;
   float x  = p.x + t1 * d.x;
   float z  = p.z + t1 * d.z;

   // check if is within the range and update the nearest t
   if ((x <= m_upperPlanes.x) && (x >= m_lowerPlanes.x) && (z <= m_upperPlanes.z) && (z >= m_lowerPlanes.z)) {
      if ((t1 >= 0) && (t1 < t)) {
         t = t1;
      }
   }

   // bottom
   t1 = (m_lowerPlanes.y - p.y) / d.y;
   x  = p.x + t1 * d.x;
   z  = p.z + t1 * d.z;
   // check if is within the range and update the nearest t
   if ((x <= m_upperPlanes.x) && (x >= m_lowerPlanes.x) && (z <= m_upperPlanes.z) && (z >= m_lowerPlanes.z)) {
      if ((t1 >= 0) && (t1 < t)) {
         t = t1;
      }
   }


   // left
   // x = m_lower, m_lower <= y <= m_upper, m_lower <= z <= m_upper, n = (-1, 0, 0)
   t1 = (m_lowerPlanes.x - p.x) / d.x;
   float y = p.y + t1 * d.y;
   z = p.z + t1 * d.z;
   // check if is within the range and update the nearest t
   if ((y <= m_upperPlanes.y) && (y >= m_lowerPlanes.y) && (z <= m_upperPlanes.z) && (z >= m_lowerPlanes.z)) {
      if ((t1 >= 0) && (t1 < t)) {
         t = t1;
      }
   }


   // right
   // x = m_upper, m_lower <= y <= m_upper, m_lower <= z <= m_upper, n = (1, 0, 0)
   t1 = (m_upperPlanes.x - p.x) / d.x;
   y  = p.y + t1 * d.y;
   z  = p.z + t1 * d.z;
   if ((y <= m_upperPlanes.y) && (y >= m_lowerPlanes.y) && (z <= m_upperPlanes.z) && (z >= m_lowerPlanes.z)) {
      if ((t1 >= 0) && (t1 < t)) {
         t = t1;
      }
   }


   // front
   // z = m_upper, m_lower <= y <= m_upper, m_lower <= x <= m_upper, n = (0, 0, 1)
   t1 = (m_upperPlanes.z - p.z) / d.z;
   y  = p.y + t1 * d.y;
   x  = p.x + t1 * d.x;
   // check if is within the range and update the nearest t
   if ((y <= m_upperPlanes.y) && (y >= m_lowerPlanes.y) && (x <= m_upperPlanes.x) && (x >= m_lowerPlanes.x)) {
      if ((t1 >= 0) && (t1 < t)) {
         t = t1;
      }
   }


   // back
   // z = m_lower, m_lower <= y <= m_upper, m_lower <= x <= m_upper, n = (0, 0, -1)
   t1 = (m_lowerPlanes.z - p.z) / d.z;
   y  = p.y + t1 * d.y;
   x  = p.x + t1 * d.x;
   // check if is within the range and update the nearest t
   if ((y <= m_upperPlanes.y) && (y >= m_lowerPlanes.y) && (x <= m_upperPlanes.x) && (x >= m_lowerPlanes.x)) {
      if ((t1 >= 0) && (t1 < t)) {
         t = t1;
      }
   }

   return t;
}
