#include "./zabermotor.h"
#include "qtimer.h"
#include <cstddef>
#include <future>
#include <ostream>
#include <sstream>
bool contains(std::string base, std::string sub);

ZaberDevice::ZaberDevice() : QSerialPort() {

  humbukBuffer = new std::string();
  delay = new QTimer();
  delay->setInterval(5);

  buffer = new std::string("");
  readTimer = new QTimer();
  readTimer->setInterval(5);

  connect(readTimer, &QTimer::timeout, this, &ZaberDevice::readSerial);
  connect(delay, &QTimer::timeout, this, &ZaberDevice::humbukStop);
  connect(this, &ZaberDevice::readyRead, this, &ZaberDevice::startSerialTimer);
}

void ZaberDevice::connectName(std::string portName) {
  this->sendToMotor("/get device.id");
}

void ZaberDevice::startSerialTimer() {
  *buffer = "";
  readTimer->start();
}

void ZaberDevice::readSerial() {
  *buffer += this->readAll().toStdString();
  if (*buffer->end() == 10 || *buffer->end() == 0) {
    readTimer->stop();
    emit motorSent(buffer);
    if (buffer->find("IDLE") != std::string::npos) {
      emit motorReady();
    } else if (buffer->find("BUSY") != std::string::npos) {
      emit motorBusy();
    }

    if (buffer->find("50105") != std::string::npos) {
      emit motorIDed(50105);
    }
  }
}

void ZaberDevice::sendToMotor(std::string message) {
  this->write((message + "\r\n").c_str());
  if (contains(message, "/move abs")) {
    int pos;
    pos = stoi(message.substr(10));
    emit sentToPosition(pos);
  }
}

qint64 ZaberDevice::write(const char *data) {
  std::string buffer = std::string(data);
  if (buffer.find("/get device.id") != std::string::npos) {
    *humbukBuffer = "50105++";
    delay->start();
  } else if (contains(buffer, "/move abs")) {
    *humbukBuffer = "IDLE++";
    delay->start();
  }
  return 0;
}

QByteArray ZaberDevice::readAll() {
  QByteArray retval = QByteArray(humbukBuffer->c_str(), humbukBuffer->size());
  // *humbukBuffer = "";
  return retval;
}

void ZaberDevice::humbukStop() {
  delay->stop();
  emit readyRead();
}

bool contains(std::string base, std::string sub) {
  return base.find(sub) != std::string::npos;
} // buffer.find("/get device.id") != std::string::npos
