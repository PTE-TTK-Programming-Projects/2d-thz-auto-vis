#include "./hostwindow.h"

HostWindow::HostWindow(QWidget *parent) : QWidget(parent) {
  scopeWin = new ScopeWindow();
  xZaberWin = new ZaberWindow();
  yZaberWin = new ZaberWindow();
  conWin = new MeasureControlWindow();
  instrumentPanel = new QFrame();
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

void HostWindow::start(double pos) {
  if (!(scopeWin->isLive())) { // do not run measurement loop when scope is in
                               // live mode
    connect(xZaberWin, &ZaberWindow::motorReady, scopeWin,
            &ScopeWindow::extMeasure);
    connect(scopeWin, &ScopeWindow::MEASUREMENT_TYPE, conWin,
            &MeasureControlWindow::recMeasPoint);
    connect(conWin, &MeasureControlWindow::requestNextStep, xZaberWin,
            &ZaberWindow::moveToUnitPos);
    xZaberWin->moveToUnitPos(pos);
  }
}

void HostWindow::stop() {
  disconnect(xZaberWin, &ZaberWindow::motorReady, scopeWin,
             &ScopeWindow::extMeasure);
  disconnect(scopeWin, &ScopeWindow::MEASUREMENT_TYPE, conWin,
             &MeasureControlWindow::recMeasPoint);
  disconnect(conWin, &MeasureControlWindow::requestNextStep, xZaberWin,
             &ZaberWindow::moveToUnitPos);
}
