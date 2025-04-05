#include "./hostwindow.h"
#include "semaphore.h"

HostWindow::HostWindow(QWidget *parent) : QWidget(parent) {
  scopeWin = new ScopeWindow();
  xZaberWin = new ZaberWindow();
  yZaberWin = new ZaberWindow();
  conWin = new MeasureControlWindow();
  instrumentPanel = new QFrame();
  semaphore = new XYMotorSemaphore(xZaberWin, yZaberWin);
  QHBoxLayout *instLayout = new QHBoxLayout();
  instLayout->addWidget(scopeWin);
  instLayout->addWidget(xZaberWin);
  instLayout->addWidget(yZaberWin);
  instrumentPanel->setLayout(instLayout);
  QGridLayout *layout = new QGridLayout();
  layout->addWidget(conWin);
  instrumentPanel->setWindowFlags(Qt::Window | Qt::WindowMaximizeButtonHint);
  this->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
  setLayout(layout);
  connect(conWin, &MeasureControlWindow::unitSelectorIndex, xZaberWin,
          &ZaberWindow::externalUnitChange);
  connect(xZaberWin, &ZaberWindow::sendUnitIndex, conWin,
          &MeasureControlWindow::recUnitIndex);
  connect(conWin, &MeasureControlWindow::instrumentVisibility, this,
          &HostWindow::visChanged);
  connect(conWin, &MeasureControlWindow::controlsHidden, this,
          &HostWindow::controlHidden);
  connect(conWin, &MeasureControlWindow::requestStart, this,
          &HostWindow::start);
  connect(conWin, &MeasureControlWindow::requestStop, this, &HostWindow::stop);
  connect(xZaberWin, &ZaberWindow::motorReady, semaphore,
          &XYMotorSemaphore::xStop);
  connect(yZaberWin, &ZaberWindow::motorReady, semaphore,
          &XYMotorSemaphore::yStop);
  this->setWindowTitle("THz auto visualizer by Gergő Illés");
  instrumentPanel->setWindowTitle(
      "Advanced Instrument Controls by Gergő Illés");
#ifdef VIRTUAL
  connect(xZaberWin->motor, &ZaberDevice::sentToPosition, scopeWin->scope,
          &PicoScope::motorPos);
//  connect(yZaberWin->motor, &ZaberDevice::sentToPosition, scopeWin->scope,
//          &PicoScope::motorPos);
#endif
}

void HostWindow::visChanged(bool isChecked) {
  if (isChecked) {
    instrumentPanel->show();
  } else {
    instrumentPanel->hide();
  }
}

void HostWindow::controlHidden() { QCoreApplication::exit(); }

void HostWindow::start(double xPos, double yPos) {
  if (!(scopeWin->isLive())) { // do not run measurement loop when scope is in
                               // live mode

    connect(semaphore, &XYMotorSemaphore::allReady, scopeWin,
            &ScopeWindow::extMeasure);
    connect(scopeWin, &ScopeWindow::MEASUREMENT_TYPE, conWin,
            &MeasureControlWindow::recMeasPoint);
    connect(conWin, &MeasureControlWindow::requestNextStep, semaphore,
            &XYMotorSemaphore::requestNextStep);
    xZaberWin->moveToUnitPos(xPos);
    yZaberWin->moveToUnitPos(yPos);
  }
}

void HostWindow::stop() {
  disconnect(semaphore, &XYMotorSemaphore::allReady, scopeWin,
          &ScopeWindow::extMeasure);
  disconnect(scopeWin, &ScopeWindow::MEASUREMENT_TYPE, conWin,
          &MeasureControlWindow::recMeasPoint);
  disconnect(conWin, &MeasureControlWindow::requestNextStep, semaphore,
          &XYMotorSemaphore::requestNextStep);
}
