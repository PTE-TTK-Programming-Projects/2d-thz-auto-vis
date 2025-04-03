#pragma once
#include "matrix.h"
#include <QtCharts>
#include <QtWidgets>
#include <fstream>
#include <iostream>

class MeasureControlWindow : public QFrame {
  Q_OBJECT;

public:
  MeasureControlWindow(QWidget *parent = nullptr);

private:
  void initDefaultValues();
  void setupConnections();
  void hideEvent(QHideEvent *event);
  void closeEvent(QCloseEvent *event);

  int i=0, j=0;

  QLineEdit *xStartpos, *xEndpos, *xStepsize, *saveName;
  QLineEdit *yStartpos, *yEndpos, *yStepsize;

  QComboBox *unitSelector;
  QPushButton *showInstrumentControls, *startMeasure, *stopButton, *resetZoom,
      *saveButton;

  QLabel *xStart, *xStep, *xStop;
  QLabel *yStart, *yStep, *yStop;

  QFrame *parameterFrame, *visualizationFrame;
  QChart *chart;
  QChartView *chartView;
  std::vector<double> *xCoords, *yCoords;
  Matrix *measVals;
  void plotResults();
private slots:
  void sendCurrentIndex(int index);
  void showClicked();
  void startMeasProc();
  void stopMeasProc();
  void resetZoomSlot();
  void saveDataSlot();

public slots:
  void recUnitIndex(int index);
  void recMeasPoint(double value);

signals:
  void unitSelectorIndex(int index);
  void instrumentVisibility(bool isChecked);
  void controlsHidden();
  void requestStart(double xPos, double yPos);
  void requestNextStep(double xPos, double yPos);
  void requestStop();
};
