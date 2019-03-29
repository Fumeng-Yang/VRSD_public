#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H
#include "glm/glm.hpp"
#include <memory>
#include <vector>
#include <QVector>

/**
 * @brief The BoundingBox class
 * Every scene object has a bounding box.
 * It has two types of encodings : the 8 vertices of the bounding box and the initial non-transformed planes.s
 * The bounding box can be transformed with a model via the model matrix as well as the 8 vertices,
 * but not the initial planes
 */
class BoundingBox
{
public:
   BoundingBox();
   // copy constructor
   BoundingBox(const BoundingBox& box);
   // initialize a bounding box for a set of other bounding boxes
   BoundingBox(const std::vector<std::shared_ptr<BoundingBox> >&);
   // initialize a bounding box from upper and lower bounds
   BoundingBox(const glm::vec3& upper, const glm::vec3& lower);

   // get the model matrix of this bounding box
   // should be the same as the object it is associated with
   glm::mat4x4 getModelMatrix();

   // get all the upper planes
   glm::vec3 getUpper();

   // get all the lower planes
   glm::vec3 getLower();

   // test if another bounding box is overlap with this box
   bool overlap(BoundingBox& box);

   // if a point is inside the bounding box
   bool inside(const glm::vec4& x);

   // extend the bounding box from another bounding box
   void updateBox(const BoundingBox& box);

   // set the model matrix
   void setModelMatrix(glm::mat4x4& mat);

   // extend the bounding box when adding a vertex
   void updateBox(const glm::vec3&);

   // extend the bounding box when adding a vertex
   void updateBox(const glm::vec4&);

   // extend the bounding box when adding a vertex
   void updateUpper(const glm::vec3&);

   // extend the bounding box when adding a vertex
   void updateLower(const glm::vec3&);

   // update 8 vertices according to upper and lower and model matrix
   void updateTranformedVertices();

   // print the upper and lower bound
   void print();

   // prin the eight vertices
   void printVertices();

   // check if this bounding box satifies its definition
   void check();

protected:
   float hit(const glm::vec4& p, const glm::vec4& d);

   glm::vec3 m_upperPlanes;
   glm::vec3 m_lowerPlanes;
   QVector<glm::vec4> m_vertices;
   glm::mat4x4 m_modelMat;
};

#endif // BOUNDINGBOX_H
