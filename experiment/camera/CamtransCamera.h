#ifndef CAMTRANSCAMERA_H
#define CAMTRANSCAMERA_H

#include "Camera.h"


/**
 * @class CamtransCamera
 *
 * The perspective camera to be implemented in the Camtrans lab.
 */
class CamtransCamera : public Camera {
public:
   // Initialize your camera.
   CamtransCamera();

   CamtransCamera(int);

   // Sets the aspect ratio of this camera. Automatically called by the GUI when the window is
   // resized.
   virtual void setAspectRatio(float aspectRatio);

   // Returns the projection matrix given the current camera settings.
   virtual glm::mat4x4 getProjectionMatrix() const;

   // Returns the view matrix given the current camera settings.
   virtual glm::mat4x4 getViewMatrix() const;

   // Returns the matrix that scales down the perspective view volume into the canonical
   // perspective view volume, given the current camera settings.
   virtual glm::mat4x4 getScaleMatrix() const;

   // Returns the matrix the unhinges the perspective view volume, given the current camera
   // settings.
   virtual glm::mat4x4 getPerspectiveMatrix() const;

   // Returns the current position of the camera.
   glm::vec4 getPosition() const;

   // Returns the current 'look' vector for this camera.
   glm::vec4 getLook() const;

   // Returns the current 'up' vector for this camera (the 'V' vector).
   glm::vec4 getUp() const;

   // Returns the currently set aspect ratio.
   float getAspectRatio() const;

   // Returns the currently set height angle.
   float getHeightAngle() const;

   // Move this camera to a new eye position, and orient the camera's axes given look and up
   // vectors.
   void orientLook(const glm::vec4& eye, const glm::vec4& look, const glm::vec4& up);

   // Sets the height angle of this camera.
   void setHeightAngle(float h);

   // Translates the camera along a given vector.
   void translate(const glm::vec4& v) override;

   // Rotates the camera about the U axis by a specified number of degrees.
   void rotateU(float degrees);

   // Rotates the camera about the V axis by a specified number of degrees.
   void rotateV(float degrees);

   // Rotates the camera about the W axis by a specified number of degrees.
   void rotateW(float degrees);

   // Sets the near and far clip planes for this camera.
   void setClip(float nearPlane, float farPlane);

   void updateMatrices() override;

   virtual void mouseDown(int x, int y) override;
   virtual void mouseDragged(int x, int y) override;
   virtual void mouseScrolled(int delta) override;
   virtual void mouseUp(int, int) override;
   virtual void resetCamera() override;

private:
   void updateProjectionMatrix();
   void updateViewMatrix();
   void updateTransMatrix();
   void updateRotMatrix();
   void updateScaleMatrix();
   void updatePerspectiveMatrix();
   void makeCamera();
   void print();

   glm::mat4x4 m_viewMatrix, m_projectionMatrix;
   glm::mat4x4 m_transMatrix, m_rotMatrix, m_scaleMatrix, m_perspectiveMatrix;
   glm::vec4   m_position;
   glm::vec4   m_look, m_up, m_u, m_v, m_w;
   float       m_aspectRatio, m_angleW, m_angleH, m_near, m_far;
   float       m_lastX, m_lastY, m_currX, m_currY;
   bool        arcball_on;
   int m_which;
};

#endif // CAMTRANSCAMERA_H
