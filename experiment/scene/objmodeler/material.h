#ifndef MATERIAL_H
#define MATERIAL_H
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include <QString>
#include <iostream>
// using Qt library to avoid compatibility issues

/**
 * @brief The Material struct
 * The material struct for .mtl. This is a subset of all the possible material,
 * but should be enough for this project
 */
struct Material
{
   QString   name; // the name of the material, which is used as an identifier
   int       id;
   float     Ns;
   float     Ni;
   float     d;
   float     Tr;
   glm::vec3 Tf;
   float     illum;
   glm::vec3 Ka;         // ambient
   glm::vec3 Kd;         // diffuse
   glm::vec3 Ks;         // specualr
   glm::vec3 Ke;         // emissive
   QString   map_Ka;     // the path for the texture of ambient
   QString   map_Kd;     // the path for the texture of diffuse
   QString   map_bump;   // the path for the texture used in bump mapping
   QString   map_normal; // the path for the texture used in normal mapping
   // notice that normal mapping path is not consistent with the a real .mtl
   Material() {
      name     = "", id = -1;
      Ns       = 0, Ni = 0, d = 0, Tr = 0, Tf = glm::vec3(), illum = 0;
      Ka       = glm::vec3(), Kd = glm::vec3(), Ks = glm::vec3(), Ke = glm::vec3();
      map_Ka   = "", map_Kd = "";
      map_bump = "", map_normal = "";
   }

   Material(QString& name_, int id_) {
      name     = name_, id = id_;
      Ns       = 0, Ni = 0, d = 0, Tr = 0, Tf = glm::vec3(), illum = 0;
      Ka       = glm::vec3(), Kd = glm::vec3(), Ks = glm::vec3(), Ke = glm::vec3();
      map_Ka   = "", map_Kd = "";
      map_bump = "", map_normal = "";
   }

   Material(QString& name_, int id_, glm::vec3& a, glm::vec3& d, QString& map_a, QString& map_d) {
      name     = name_, id = id_;
      Ka       = a, Ka = d, map_Ka = map_a, map_Kd = map_d;
      map_bump = "", map_normal = "";
   }

   void      print() {
      std::cout << "name is " << name.toStdString() << ", map_Ka is " << map_Ka.toStdString() << std::endl;
   }
};
#endif // MATERIAL_H
