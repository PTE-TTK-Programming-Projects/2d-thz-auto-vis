#pragma once
#include "./scope.h"
#include "./scope_data_line.h"
#include <QtCharts>
#include <QtWidgets>
#include <string>

class ScopeWindow : public QFrame {
  Q_OBJECT;

public:
  ScopeWindow(QWidget *parent = nullptr);
  bool isLive();
#ifdef VIRTUAL
  PicoScope *scope;
#endif
private:
#ifndef VIRTUAL
  PicoScope *scope;
#endif

private slots:
  void showStatus(std::string status);
  void showMeasurementData(int32_t *bufferSize, int16_t *buffer);
  void resetZoom();
  void liveRequest(bool isLive);
  void sendSetup();
  void sendTime();
  void sendRatio();

signals:
  void chartingFinished();
  void setScopeChannel(int couplingIDX, int sensIDX);
  void setTriggerThreshold(int16_t divisor);
  void setTimeBase(uint32_t timeBase);
  void sendAvg(double avg);
  void sendPtp(double ptp);

private:
  QPushButton *button, *measurebutton, *liveButton, *homeButton;
  QLineEdit *status, *avgRequest;
  QLabel *avgRequestLabel;
  QChartView *chartView;
  QChart *chart;
  ScopeDataLine *avgLine, *ptpLine;
  QComboBox *coupling, *sens;
  QComboBox *windowLength, *triggerRatio;
  double *timeWindow, *sensitivity;

public slots:
  void extMeasure();
};
