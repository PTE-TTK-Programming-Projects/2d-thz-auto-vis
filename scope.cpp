#include "./scope.h"

PicoScope::PicoScope() : QObject() {
  timeBase = new uint32_t(34);
  statusTimer = new QTimer;
  statusTimer->setInterval(50);
  ready = new int16_t;
  divisor = new int16_t(100);
  handle = new int16_t;
  status = new PICO_STATUS;
  timeTookMS = new int32_t;
  bufferLength = new int32_t(2001);
  noOfSamples = new uint32_t(2001);
  bufferArray = new int16_t[2001];
  sendArray = new int16_t[2001];
  avgArray = new int32_t[2001];
  avgCounter = new int16_t(0);
  avgRqst = new int16_t(0);
  *status = ps5000aOpenUnit(handle, nullptr, PS5000A_DR_12BIT);
  if (*status == PICO_POWER_SUPPLY_NOT_CONNECTED) {
    ps5000aChangePowerSource(*handle, *status);
  }
  maxValue = new int16_t;
  ps5000aMaximumValue(*handle, maxValue);
  ps5000aSetChannel(*handle, PS5000A_CHANNEL_A, 1, PS5000A_AC, PS5000A_5V, 0);
  ps5000aSetSimpleTrigger(*handle, 1, PS5000A_CHANNEL::PS5000A_CHANNEL_A,
                          *maxValue / *divisor,
                          PS5000A_THRESHOLD_DIRECTION::PS5000A_RISING, 0, 1000);
  ps5000aSetDataBuffer(*handle, PS5000A_CHANNEL_A, bufferArray, *bufferLength,
                       0, PS5000A_RATIO_MODE_NONE);
  connect(this, &PicoScope::finishSignal, this, &PicoScope::retrieveData);
  connect(statusTimer, &QTimer::timeout, this, &PicoScope::getStatus);
  statusTimer->start();
}

void PicoScope::measure() {
  ps5000aRunBlock(*handle, 1000, 1001, *timeBase, timeTookMS, 0,
                  PicoScope::readReady, static_cast<void *>(this));
}

void PicoScope::getStatus() {
  // std::cout << "Status fetching reached" << std::endl;
  *status = ps5000aPingUnit(*handle);
  switch (*status) {
  case PICO_OK:
    emit sendStatus("PICO_OK");
    break;
  case PICO_BUSY:
    emit sendStatus("PICO_BUSY");
    break;
  case PICO_MEMORY:
    emit sendStatus("PICO_MEMORY");
    break;

  default:
    emit sendStatus("PICO_ERROR");
    break;
  }
}

void PicoScope::retrieveData() {
  // std::cout << "Retrieving data" << std::endl;
  ps5000aGetValues(*handle, 0, noOfSamples, 1, PS5000A_RATIO_MODE_NONE, 0,
                   nullptr);
  *noOfSamples = 2001;
  if (*avgRqst == 0 || *avgRqst == 1) {
    emit sendMeasurement(bufferLength, bufferArray);
  } else if (*avgCounter < *avgRqst) {
    if (*avgCounter == 0) {
      for (uint32_t i = 0; i < *noOfSamples; i++) {
        avgArray[i] = static_cast<int32_t>(bufferArray[i]);
      }
      *avgCounter += 1;
      measure();
    } else {
      for (uint32_t i = 0; i < *noOfSamples; i++) {
        avgArray[i] += static_cast<int32_t>(bufferArray[i]);
      }
      *avgCounter += 1;
      measure();
    }
  } else if (*avgCounter >= *avgRqst) {
    for (uint32_t i = 0; i < *noOfSamples; i++) {
      sendArray[i] = static_cast<int16_t>(avgArray[i] / *avgRqst);
    }
    *avgCounter = 0;
    emit sendMeasurement(bufferLength, sendArray);
  }
}
void PicoScope::setAvgRqst(const QString &lineEditString) {
  *this->avgRqst = static_cast<int16_t>(lineEditString.toInt());
}

void PicoScope::readReady(int16_t handle, PICO_STATUS status,
                          void *pParameter) {
  static_cast<PicoScope *>(pParameter)->finishSignal();
}

void PicoScope::setScopeChannel(int couplingIDX, int sensIDX) {
  int couplingEnum, sensEnum;

  switch (couplingIDX) {
  case 0:
    couplingEnum = PS5000A_AC;
    break;
  case 1:
    couplingEnum = PS5000A_DC;
    break;
  default:
    couplingEnum = PS5000A_AC;
    break;
  }

  switch (sensIDX) {
  case 0:
    sensEnum = PS5000A_10MV;
    break;
  case 1:
    sensEnum = PS5000A_50MV;
    break;
  case 2:
    sensEnum = PS5000A_100MV;
    break;
  case 3:
    sensEnum = PS5000A_500MV;
    break;
  case 4:
    sensEnum = PS5000A_1V;
    break;
  case 5:
    sensEnum = PS5000A_2V;
    break;
  case 6:
    sensEnum = PS5000A_5V;
    break;
  default:
    sensEnum = PS5000A_20V;
    break;
  }
  ps5000aSetChannel(*handle, PS5000A_CHANNEL_A, 1,
                    (PS5000A_COUPLING)couplingEnum, (PS5000A_RANGE)sensEnum, 0);
}

void PicoScope::setTimeWindow(uint32_t timeBase) { *this->timeBase = timeBase; }
void PicoScope::setTriggerRatio(int16_t divisor) {
  if (divisor != -1) {
    *this->divisor = divisor;
    ps5000aSetSimpleTrigger(
        *handle, 0, PS5000A_CHANNEL::PS5000A_EXTERNAL, *maxValue / 10,
        PS5000A_THRESHOLD_DIRECTION::PS5000A_RISING, 0, 1000);
    ps5000aSetSimpleTrigger(*handle, 1, PS5000A_CHANNEL::PS5000A_CHANNEL_A,
                            *maxValue / *this->divisor,
                            PS5000A_THRESHOLD_DIRECTION::PS5000A_RISING, 0,
                            1000);
  } else {
    ps5000aSetSimpleTrigger(*handle, 0, PS5000A_CHANNEL::PS5000A_CHANNEL_A,
                            *maxValue / *this->divisor,
                            PS5000A_THRESHOLD_DIRECTION::PS5000A_RISING, 0,
                            1000);
    ps5000aSetSimpleTrigger(
        *handle, 1, PS5000A_CHANNEL::PS5000A_EXTERNAL, *maxValue / 2,
        PS5000A_THRESHOLD_DIRECTION::PS5000A_RISING, 0, 1000);
  }
}
