#include "./scope.h"
#include "PicoStatus.h"
#include <cmath>
PicoScope::PicoScope() : QObject() {
  statusTimer = new QTimer;
  statusTimer->setInterval(100);
  status = new PICO_STATUS;
  bufferLength = new int32_t(2001);
  noOfSamples = new uint32_t(2001);
  bufferArray = new int16_t[2001];
  sendArray = new int16_t[2001];
  avgArray = new int32_t[2001];
  avgCounter = new int16_t(0);
  avgRqst = new int16_t(0);

  *status = PICO_OK;
  connect(statusTimer, &QTimer::timeout, this, &PicoScope::virtualRequest);
  connect(this, &PicoScope::finishSignal, this, &PicoScope::retrieveData);
}

void PicoScope::motorPos(int pos) { *motorPosValue = pos; }
void PicoScope::getStatus() { emit sendStatus("VIRTUAL_OK"); }

void PicoScope::measure() {
  std::cout << "Measure request arrived" << std::endl;
  statusTimer->start();
}

void PicoScope::virtualRequest() {
  std::cout << "Virtual finish signal emitted" << std::endl;
  statusTimer->stop();
  emit this->finishSignal();
}

double gaussian(double center, double width, int pos) {
  double Pos = double(pos) / 1e4;
  return exp(-pow(Pos - center, 2) / width);
}

void PicoScope::retrieveData() {
  std::cout << "Virtual data retrieval started" << std::endl;
  for (int i = 0; i < *bufferLength; i++) {
    bufferArray[i] = std::floor(gaussian(250, 10, *motorPosValue) * 25000);
  }
  emit sendMeasurement(bufferLength, bufferArray);
}
void PicoScope::setTriggerRatio(short) {}
void PicoScope::setTimeWindow(unsigned int) {}
void PicoScope::setAvgRqst(QString const &) {}
void PicoScope::setScopeChannel(int, int) {}
