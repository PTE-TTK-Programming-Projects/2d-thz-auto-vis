#pragma once
#include "./zabermotor.h"
#include <QErrorMessage>
#include <QSerialPortInfo>
#include <QtWidgets>
#include <iostream>
#include <string>

class ZaberWindow : public QFrame {
  Q_OBJECT;

public:
  ZaberWindow(QWidget *parent = nullptr);
#ifdef VIRTUAL
  ZaberDevice *motor;
#endif

private:
#ifndef VIRTUAL
  ZaberDevice *motor;
#endif
  QPushButton *connectButton, *homeButton, *moveButton;
  QComboBox *selectBox, *unitbox;
  QLineEdit *manualMsg, *movePos;
  QListWidget *messageScrollback;
  QPushButton *stepForward, *stepBackward;
  QLineEdit *stepByValue;
  void refreshComboBox();
  double *microstepSize, *maxDistance, *unitMultiplier;
  void connectSignals();
private slots:
  void selectPort();
  void motorMsg(std::string *message);
  void motorID(int ID);
  void prepManMsg();
  void buttonHome();
  void moveToPos();
  void unitSelChd(QString unit);
  void unitSend(int index);
  void stpFWD();
  void stpBWD();
  void relayMotorReady();
public slots:
  void externalUnitChange(int index);
  void moveToUnitPos(double position);

signals:
  void connectToPort(std::string portName);
  void sendManualMsg(std::string manMsg);
  void sendUnitIndex(int index);
  void motorReady();
};
