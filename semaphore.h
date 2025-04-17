#pragma once
#include "./zaberwindow.h"
#include "qobject.h"
#include <QtWidgets>

class XYMotorSemaphore : public QObject {
  Q_OBJECT

private:
  bool xStatus = false, yStatus = false;
  ZaberWindow *xWin, *yWin;
  double xPrev, yPrev;

public:
  XYMotorSemaphore(ZaberWindow *xWin, ZaberWindow *yWin);

public slots:
  void xStop();
  void yStop();
  void xStart();
  void yStart();
  void requestNextStep(double xPos, double yPos);

signals:
  void allReady();
};
