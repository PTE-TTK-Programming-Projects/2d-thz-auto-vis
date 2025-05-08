#pragma once

#include "semaphore.h"

#ifndef MEASUREMENT_TYPE
#define MEASUREMENT_TYPE sendAvg // sendAvg -> average; sendPtp -> peak-to-peak
#endif

class ScopeWindow;
class ZaberWindow;
class MeasureControlWindow;


class HostWindow : public QWidget {
  Q_OBJECT

public:
  HostWindow(QWidget *parent = nullptr);

private:
  ScopeWindow *scopeWin;
  ZaberWindow *xZaberWin;
  ZaberWindow *yZaberWin;
  MeasureControlWindow *conWin;
  QFrame *instrumentPanel;
  XYMotorSemaphore *semaphore;

private slots:
  void visChanged(bool isChecked);
  void controlHidden();
  void start(double xPos, double yPos);
  void stop();
};
