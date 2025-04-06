#include "./scope.h"
#include "PicoStatus.h"
#include <cmath>
PicoScope::PicoScope() : QObject() {
  statusTimer = new QTimer;
  statusTimer->setInterval(5);
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

void PicoScope::xmotorPos(int pos) { *xmotorPosValue = pos; }
void PicoScope::ymotorPos(int pos) { *ymotorPosValue = pos; }
void PicoScope::getStatus() { emit sendStatus("VIRTUAL_OK"); }

void PicoScope::measure() {
  // std::cout << "Measure request arrived" << std::endl;
  statusTimer->start();
}

void PicoScope::virtualRequest() {
  // std::cout << "Virtual finish signal emitted" << std::endl;
  statusTimer->stop();
  emit this->finishSignal();
}

double gaussian(double center, double width, int xpos, int ypos) {
  double Pos = double(xpos) / 1e4;
  double yPos = double(ypos) / 1e4;
  return exp(-pow(Pos - center, 2) / (pow((yPos * 0.5), 2) + 20)) *
         abs(cos((fmax(2,(-yPos) / 200 + 10)) * (Pos - 120) / 3.14));
}

void PicoScope::retrieveData() {
  // std::cout << "Virtual data retrieval started" << std::endl;
  for (int i = 0; i < *bufferLength; i++) {
    bufferArray[i] =
        std::floor(gaussian(120, 25, *xmotorPosValue, *ymotorPosValue) * 25000);
  }
  emit sendMeasurement(bufferLength, bufferArray);
}
void PicoScope::setTriggerRatio(short) {}
void PicoScope::setTimeWindow(unsigned int) {}
void PicoScope::setAvgRqst(QString const &) {}
void PicoScope::setScopeChannel(int, int) {}
