#pragma once

#include <QSerialPort>
#include <QtWidgets>
#include <iostream>
#include <string>

#define VIRTUAL

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
  void motorIDed(int ID);

public:
#ifdef VIRTUAL
  qint64 write(const char *data);
  QByteArray readAll();

private:
  std::string *humbukBuffer;
  QTimer *delay;

private slots:
  void humbukStop();

signals:
  void sentToPosition(int pos);
#endif
};
