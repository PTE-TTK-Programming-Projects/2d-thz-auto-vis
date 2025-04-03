#pragma once

#include "./meascontrolwidget.h"
#include "./scopewindow.h"
#include "./zaberwindow.h"

#ifndef MEASUREMENT_TYPE
#define MEASUREMENT_TYPE sendAvg // sendAvg -> average; sendPtp -> peak-to-peak
#endif

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

private slots:
  void visChanged(bool isChecked);
  void controlHidden();
  void start(double pos);
  void stop();
};
