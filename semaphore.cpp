#include "./semaphore.h"
#include "zaberwindow.h"

XYMotorSemaphore::XYMotorSemaphore(ZaberWindow *xWin, ZaberWindow *yWin)
    : QObject(nullptr) {
  this->xWin = xWin;
  this->yWin = yWin;
}

void XYMotorSemaphore::xStop() {
  xStatus = true;
  if (xStatus && yStatus) {
    emit allReady();
  }
}

void XYMotorSemaphore::yStop() {
  yStatus = true;
  if (xStatus && yStatus) {
    emit allReady();
  }
}
void XYMotorSemaphore::xStart() { xStatus = false; }

void XYMotorSemaphore::yStart() { yStatus = false; }

void XYMotorSemaphore::requestNextStep(double xPos, double yPos) {
  // std::cout << "X: " << xPos << ", Y: " << yPos << std::endl;
  if (xPrev != xPos) {
    xStatus = false;
    xPrev = xPos;
    xWin->moveToUnitPos(xPos);
  }
  if (yPrev != yPos) {
    xStatus = false;
    yPrev = yPos;
    yWin->moveToUnitPos(yPos);
  }
}
