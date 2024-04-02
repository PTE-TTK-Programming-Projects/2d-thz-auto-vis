#pragma once

#include <QSerialPort>
#include <QtWidgets>
#include <string>

class ZaberDevice : public QSerialPort {
  Q_OBJECT;

public:
  ZaberDevice();

private:
  std::string *buffer;
  QTimer *readTimer;

private slots:
  void startSerialTimer();
  void readSerial();
public slots:
  void connectName(std::string portName);
  void sendToMotor(std::string message);
signals:
  void motorReady();
  void motorBusy();
  void motorSent(std::string *msg);
};
