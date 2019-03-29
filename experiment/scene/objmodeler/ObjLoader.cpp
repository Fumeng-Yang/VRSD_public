#include "ObjLoader.h"
#include <QString>
#include <QMap>
#include <QTextStream>
#include <QFile>
#include <QDir>
#include <QOpenGLTexture>
#include <iostream>
#include "SceneObject.h"
#include "PrimitiveObject.h"
#include "GroupObject.h"
#include "Settings.h"

/**
 * @brief ObjLoader::ObjLoader
 */
ObjLoader::ObjLoader() : m_materialMap(QMap<QString, Material>()),
   m_allObjs(QVector<PrimitiveObject *>())
{}

/**
 * @brief ObjLoader::~ObjLoader
 * We free all the objects in the scene class. The objLoader has all the primitive objects,
 * even they will be a part of GroupObject from the perspective of the scene
 */
ObjLoader::~ObjLoader() {}


/**
 * @brief ObjLoader::loadObj
 * @param target, path to the obj file, with out the .obj suffix, because we also have to read material file!
 * @param results, where we put our results
 * @param points, where we put all the vertices
 */
void ObjLoader::loadObj(const QString&        target,
                        QVector<SceneObject *>& results,
                        QVector<GLfloat>&     points) {
   // which one is pickable
   setUpPickableList();
   // load the material file
   loadMaterials(target);
   // parse all vertices, uvs, and normals
   // build all primitive objects
   parseVertices(target, points);
   // group different primitive objects into groups
   buildGroups(results);
}


/**
 * @brief ObjLoader::setUpPickableList
 */
void ObjLoader::setUpPickableList() {
   settings.pickableList.push_back("Mug");
   settings.pickableList.push_back("OpenBook");
   settings.pickableList.push_back("Clock");
   settings.pickableList.push_back("Globe");
   settings.pickableList.push_back("RedBook");
   settings.pickableList.push_back("Boy");
}


/**
 * @brief ObjLoader::getTextureMap
 * @param textMap
 * Read in all the textures appeared in this mtl file
 */
void ObjLoader::getTextureMap(QVector<QString>& textMap) {
   for (Material m : m_materialMap) {
      if ((m.map_Ka != "") && !textMap.contains(m.map_Ka)) {
         textMap.push_back(m.map_Ka);
      }
      if ((m.map_Kd != "") && !textMap.contains(m.map_Kd)) {
         textMap.push_back(m.map_Kd);
      }
      if ((m.map_bump != "") && !textMap.contains(m.map_bump)) {
         textMap.push_back(m.map_bump);
      }
      if ((m.map_normal != "") && !textMap.contains(m.map_normal)) {
         textMap.push_back(m.map_normal);
      }
   }
}


/**
 * @brief ObjLoader::loadMaterials
 * @param target
 * load and parse the material file
 */
void ObjLoader::loadMaterials(const QString& target) {
   QString mtlPath = QDir::currentPath() + "/" + target + ".mtl";
   QFile   mtlFile(mtlPath);
   if (!mtlFile.exists()) {
      std::cout << "Can't read the texture file (" << mtlPath.toStdString() << ")" << std::endl;
      exit(1);
   }else if (mtlFile.open(QIODevice::ReadOnly)) {
      QTextStream in(&mtlFile);
      while (!in.atEnd())
      {
         QString line = in.readLine();
         // removed whitespace from the start and the end
         // and that has each sequence of internal whitespace replaced with a single space.
         QStringList data = line.simplified().split(" ");
         if ((data.length() > 0) && (data[0] == "newmtl")) {
            QString  mtlName = data[1];
            Material mtl;
            mtl.name = mtlName;
            bool nextMtl = false;
            while (!nextMtl)
            {
               line = in.readLine();
               data = line.simplified().split(" ");

               if ((data.length() <= 0) || (data[0] == "")) { nextMtl = true; continue; }
               if (data[0] == "Ns") { mtl.Ns = data[1].toFloat(); }
               if (data[0] == "Ni") { mtl.Ni = data[1].toFloat(); }
               if (data[0] == "d") { mtl.d = data[1].toFloat(); }
               if (data[0] == "Tr") { mtl.Tr = data[1].toFloat(); }
               if (data[0] == "Tf") {
                  mtl.Tf = glm::vec3(data[1].toFloat(),
                                     data[2].toFloat(),
                                     data[3].toFloat());
               }
               if (data[0] == "illum") { mtl.illum = data[1].toInt(); continue; }
               if (data[0] == "Ka") {
                  mtl.Ka = glm::vec3(data[1].toFloat(),
                                     data[2].toFloat(),
                                     data[3].toFloat());
               }
               if (data[0] == "Kd") {
                  mtl.Kd = glm::vec3(data[1].toFloat(),
                                     data[2].toFloat(),
                                     data[3].toFloat());
               }
               if (data[0] == "Ks") {
                  mtl.Ks = glm::vec3(data[1].toFloat(),
                                     data[2].toFloat(),
                                     data[3].toFloat());
               }
               if (data[0] == "Ke") {
                  mtl.Ke = glm::vec3(data[1].toFloat(),
                                     data[2].toFloat(),
                                     data[3].toFloat());
               }
               if (data[0] == "map_Ka") { mtl.map_Ka = data[1]; }
               if (data[0] == "map_Kd") { mtl.map_Kd = data[1]; }
               if (data[0] == "map_bump") { mtl.map_bump = data[1]; }
               if (data[0] == "map_normal") { mtl.map_normal = data[1]; }
            }
            m_materialMap[mtlName] = mtl;
//          std::cout << mtlName.toStdString() << std::endl;
         }
      }
   }
}


/**
 * @brief ObjLoader::parseVertices
 * @param target
 * @param cVerts
 */
void ObjLoader::parseVertices(const QString& target, QVector<GLfloat>& cVerts) {
   QString objPath = QDir::currentPath() + "/" + target + ".obj";
   QFile   objFile(objPath);
   if (!objFile.exists()) {
      std::cout << "Can't read the obj file (" << objPath.toStdString() << ")" << std::endl;
      exit(1);
   }else if (objFile.open(QIODevice::ReadOnly)) {
      QVector<GLfloat> verts, uvs, ns;
      QTextStream      in(&objFile);
      int              offset = 0;
      while (!in.atEnd())
      {
         QString     line = in.readLine();
         QStringList data = line.simplified().split(" ");
         if ((data.length() > 1) && (data[0] == "#") && (data[1] == "object")) {
            PrimitiveObject *obj = new PrimitiveObject();
            obj->setOffset(offset);
            obj->setName(QString(data[2]));
            bool nextObj = false;
            int  count   = 0;
            while (!nextObj && !in.atEnd())
            {
               line = in.readLine();
               data = line.simplified().split(" ");

               if ((data.length() > 0) && (data[0] == "v")) {
                  float v1 = data[1].toFloat(), v2 = data[2].toFloat(), v3 = data[3].toFloat();
                  // add vertices
                  verts.append(v1);
                  verts.append(v2);
                  verts.append(v3);
                  obj->updateBox(glm::vec3(v1, v2, v3));
               }else if ((data.length() > 0) && (data[0] == "vt")) {
                  // add uv coordinates
                  uvs.append(data[1].toFloat());
                  uvs.append(data[2].toFloat());
               }else if ((data.length() > 0) && (data[0] == "vn")) {
                  // add normals
                  ns.append(data[1].toFloat());
                  ns.append(data[2].toFloat());
                  ns.append(data[3].toFloat());
               }else if ((data.length() > 0) && (data[0] == "usemtl")) {
                  // map material
                  QString mtlName = data[1];
                  obj->setMaterial(m_materialMap[mtlName]);
               }else if ((data.length() > 0) && (data[0] == "f")) {
                  // according to the f *** line, put vertices, uvs and normals in the correct order
                  if (data.length() == 5) { // this is quad
                     int         tris[] = { 0, 1, 2, 0, 2, 3 };
                     QStringList row;

                     for (int i = 0; i < 6; i++) {
                        row = data[tris[i] + 1].split("/");
                        int v  = row[0].toInt() - 1;
                        int vt = row[1].toInt() - 1;
                        int n  = row[2].toInt() - 1;
                        for (int j = 0; j < 3; j++) {
                           cVerts.append(verts.at(v * 3 + j));
                        }
                        for (int j = 0; j < 2; j++) {
                           cVerts.append(uvs.at(vt * 2 + j));
                        }
                        for (int j = 0; j < 3; j++) {
                           cVerts.append(ns.at(n * 3 + j));
                        }
                        offset += 1;
                        count  += 1;
                     }
                  }else { // this is triangle
                     QStringList row;
                     for (int i = 1; i < 4; i++) {
                        row = data[i].split("/");
                        int v  = row[0].toInt() - 1;
                        int vt = row[1].toInt() - 1;
                        int n  = row[2].toInt() - 1;
                        for (int j = 0; j < 3; j++) {
                           cVerts.append(verts.at(v * 3 + j));
                        }
                        for (int j = 0; j < 2; j++) {
                           if (vt == -1) {
                              cVerts.append(0);
                           }else{
                              cVerts.append(uvs.at(vt * 2 + j));
                           }
                        }
                        for (int j = 0; j < 3; j++) {
                           if (n == -1) {
                              cVerts.append(0);
                           }else{
                              cVerts.append(ns.at(n * 3 + j));
                           }
                        }
                        offset += 1;
                        count  += 1;
                     }
                  }
                  // end of parsing one line of f ...
               }
               if ((data.length() > 1) && (data[0] == "#") && (data[2] == "faces")) {
                  nextObj = true;
               }
            } // end of parsing one obj
            obj->setNumVertices(count);
            obj->setObjectType(PRIMITIVE_OBJECT);
            m_allObjs.append(obj);
         }
      }
   }
}


/**
 * @brief ObjLoader::buildGroups
 * @param results
 * the trick is that we use the name as the identicier (?)
 * name is in the format of [..]_[..]_[..]
 * if a name starts with g, we know it is a primitive object that belongs to a group
 *     the name of the group is the second word
 *     if we haven't build the group, we build the group first
 *     otherwise, we just add the object into the group
 * if a name starts with o, we know it is a primitive object on its own
 *
 */
void ObjLoader::buildGroups(QVector<SceneObject *>& results) {
   QMap<QString, GroupObject *> groupDicts = QMap<QString, GroupObject *>();
   for (PrimitiveObject *ptr : m_allObjs) {
      QString     name     = ptr->getName();
      QStringList nameDict = name.simplified().split("_");
      if (nameDict[0] == "g") {    // this is an object in a group
         QString groupName = nameDict[1];
         if (!groupDicts[groupName]) {
            // build a new group if there is no such a group
            GroupObject *newGroup = new GroupObject();
            newGroup->setName(groupName);
            newGroup->setObjectType(GROUP_OBJECT);
            groupDicts[groupName] = newGroup;
            results.push_back(newGroup);
            if (settings.pickableList.contains(groupName)) {
               groupDicts[groupName]->setPickable(true);
            }
         }
         ptr->setObjectType(PRIMITIVE_IN_GROUP);
         ptr->setPickable(groupDicts[groupName]->isPickable());
         groupDicts[groupName]->addPrimitiveObject(ptr);
      }else{    // this is "o" or just an object
         QString simpleName = name;
         if (nameDict.length() > 1) {
            simpleName = nameDict[1];
         }
         ptr->setName(simpleName);
         results.push_back(ptr);
         if (settings.pickableList.contains(simpleName)) {
            ptr->setPickable(true);
         }
      }
   }
}
