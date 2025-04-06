#pragma once
#include <QtWidgets>
#include <iostream>
#include <ps5000aApi.h>
#include <string>

#define VIRTUAL

class PicoScope : public QObject {
  Q_OBJECT;

public:
  PicoScope();

private:
  int16_t *handle;
  PICO_STATUS *status;
  int16_t *maxValue, *ready, *divisor;
  uint32_t *timeBase;
  int32_t *timeTookMS;
  int16_t *bufferArray, *sendArray;
  int32_t *avgArray;
  int32_t *bufferLength;
  int16_t *avgCounter, *avgRqst;
  uint32_t *noOfSamples;
  static void readReady(int16_t handle, PICO_STATUS status, void *pParameter);
  QTimer *statusTimer;
private slots:
  void retrieveData();

public slots:
  void getStatus();
  void measure();
  void setScopeChannel(int couplingIDX, int sensIDX);
  void setTimeWindow(uint32_t timeBase);
  void setTriggerRatio(int16_t divisor);
  void setAvgRqst(const QString &lineEditString);
signals:
  void sendStatus(std::string status);
  void finishSignal();
  void sendMeasurement(int32_t *bufferLength, int16_t *buffer);

#ifdef VIRTUAL
private:
  int *xmotorPosValue = new int(0), *ymotorPosValue = new int(0);
private slots:
  void virtualRequest();
public slots:
  void xmotorPos(int pos);
  void ymotorPos(int pos);
#endif
};
