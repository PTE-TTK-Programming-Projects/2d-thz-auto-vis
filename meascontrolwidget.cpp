#include "./meascontrolwidget.h"
#include <cstring>
#include <qcustomplot.h>

MeasureControlWindow::MeasureControlWindow(QWidget *parent) : QFrame(parent) {
  initDefaultValues();
  QVBoxLayout *layout = new QVBoxLayout();
  QVBoxLayout *xlayout = new QVBoxLayout();
  QVBoxLayout *ylayout = new QVBoxLayout();
  QHBoxLayout *merged = new QHBoxLayout();
  layout->addWidget(resetZoom);
  layout->addWidget(interpButton);
  layout->addWidget(showInstrumentControls);
  xlayout->addWidget(xStart);
  xlayout->addWidget(xStartpos);
  xlayout->addWidget(xStep);
  xlayout->addWidget(xStepsize);
  xlayout->addWidget(xStop);
  xlayout->addWidget(xEndpos);
  ylayout->addWidget(yStart);
  ylayout->addWidget(yStartpos);
  ylayout->addWidget(yStep);
  ylayout->addWidget(yStepsize);
  ylayout->addWidget(yStop);
  ylayout->addWidget(yEndpos);
  merged->addLayout(xlayout);
  merged->addLayout(ylayout);
  layout->addLayout(merged);
  layout->addWidget(unitSelector);
  layout->addWidget(startMeasure);
  layout->addWidget(stopButton);
  layout->addWidget(saveName);
  layout->addWidget(saveButton);
  layout->setAlignment(Qt::AlignmentFlag::AlignTop);
  setupConnections();
  parameterFrame->setLayout(layout);
  QGridLayout *visLayout = new QGridLayout();
  visLayout->addWidget(customPlot);
  visualizationFrame->setLayout(visLayout);
  QHBoxLayout *measureLayout = new QHBoxLayout();
  measureLayout->addWidget(visualizationFrame);
  measureLayout->addWidget(parameterFrame);
  setLayout(measureLayout);
}

void MeasureControlWindow::initDefaultValues() {
  xStartpos = new QLineEdit();
  xEndpos = new QLineEdit();
  xStepsize = new QLineEdit();
  yStartpos = new QLineEdit();
  yEndpos = new QLineEdit();
  yStepsize = new QLineEdit();
  unitSelector = new QComboBox();
  unitSelector->addItems(QList<QString>({QString("mm"), QString("um")}));
  showInstrumentControls = new QPushButton("Show instrument controls");
  showInstrumentControls->setCheckable(true);
  xStart = new QLabel("Start Position");
  xStep = new QLabel("Step size");
  xStop = new QLabel("End postition");
  yStart = new QLabel("Start Position");
  yStep = new QLabel("Step size");
  yStop = new QLabel("End postition");
  parameterFrame = new QFrame();
  visualizationFrame = new QFrame();
  // chart = new QChart();
  // chartView = new QChartView(chart);
  // chart->layout()->setContentsMargins(0, 0, 0, 0);
  // chart->resize(640, 480);
  // chartView->setRubberBand(QChartView::RectangleRubberBand);
  // chartView->setRenderHint(QPainter::Antialiasing, true);
  customPlot = new QCustomPlot();
  customPlot->setMinimumWidth(500);
  customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
  customPlot->axisRect()->setupFullAxesBox(true);
  customPlot->xAxis->setLabel("X Motor Position");
  customPlot->yAxis->setLabel("Y Motor Position");
  colorMap = new QCPColorMap(customPlot->xAxis, customPlot->yAxis);
  gradient = new QCPColorGradient();
  gradient->setColorStopAt(0.0, QColor(0, 0, 0));
  gradient->setColorStopAt(0.25, QColor(0, 64, 0));
  gradient->setColorStopAt(0.50, QColor(0, 128, 0));
  gradient->setColorStopAt(0.75, QColor(0, 192, 0));
  gradient->setColorStopAt(1, QColor(0, 255, 0));
  colorMap->setGradient(*gradient);
  colorMap->setInterpolate(false);
  parameterFrame->setFrameShape(QFrame::StyledPanel);
  parameterFrame->setFrameShadow(QFrame::Raised);
  parameterFrame->setLineWidth(3);
  parameterFrame->setMidLineWidth(3);
  parameterFrame->setFixedWidth(300);
  visualizationFrame->setFrameShape(QFrame::StyledPanel);
  visualizationFrame->setFrameShadow(QFrame::Raised);
  visualizationFrame->setLineWidth(3);
  visualizationFrame->setMidLineWidth(3);
  startMeasure = new QPushButton("Start measurement");
  xCoords = new std::vector<double>();
  yCoords = new std::vector<double>();
  stopButton = new QPushButton("Stop and Reset");
  resetZoom = new QPushButton("Reset Zoom");
  interpButton = new QPushButton("Interpolate");
  interpButton->setCheckable(true);
  saveButton = new QPushButton("Save data");
  saveName = new QLineEdit();
}

void MeasureControlWindow::setupConnections() {
  connect(unitSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &MeasureControlWindow::sendCurrentIndex);
  connect(showInstrumentControls, &QPushButton::clicked, this,
          &MeasureControlWindow::showClicked);
  connect(interpButton, &QPushButton::toggled, this,
          &MeasureControlWindow::interpData);
  connect(startMeasure, &QPushButton::clicked, this,
          &MeasureControlWindow::startMeasProc);
  connect(stopButton, &QPushButton::clicked, this,
          &MeasureControlWindow::stopMeasProc);
  connect(resetZoom, &QPushButton::clicked, this,
          &MeasureControlWindow::resetZoomSlot);
  connect(saveButton, &QPushButton::clicked, this,
          &MeasureControlWindow::saveDataSlot);
}

void MeasureControlWindow::interpData(bool interp) {
  colorMap->setInterpolate(interp);
  customPlot->replot();
}
void MeasureControlWindow::sendCurrentIndex(int index) {
  emit unitSelectorIndex(index);
}

void MeasureControlWindow::recUnitIndex(int index) {
  unitSelector->setCurrentIndex(index);
}
void MeasureControlWindow::showClicked() {
  emit instrumentVisibility(showInstrumentControls->isChecked());
}

void MeasureControlWindow::hideEvent(QHideEvent *event) { event->ignore(); }
void MeasureControlWindow::closeEvent(QCloseEvent *event) { event->ignore(); }

void MeasureControlWindow::startMeasProc() {
  double xStart, xStep, xEnd, xCurrentCoord;
  double yStart, yStep, yEnd, yCurrentCoord;

  xStart = xStartpos->text().toDouble();
  xStep = xStepsize->text().toDouble();
  xEnd = xEndpos->text().toDouble();
  xCoords->clear();
  xCurrentCoord = xStart;
  while (xCurrentCoord < xEnd) {
    xCoords->push_back(xCurrentCoord);
    xCurrentCoord += xStep;
  }
  xCoords->push_back(xEnd);

  yStart = yStartpos->text().toDouble();
  yStep = yStepsize->text().toDouble();
  yEnd = yEndpos->text().toDouble();
  yCoords->clear();
  yCurrentCoord = yStart;
  while (yCurrentCoord < yEnd) {
    yCoords->push_back(yCurrentCoord);
    yCurrentCoord += yStep;
  }
  yCoords->push_back(yEnd);
  measVals = new Matrix(xCoords->size(), yCoords->size());
  i = 0, j = 0;
  colorMap->data()->setSize(xCoords->size(), yCoords->size());
  colorMap->data()->setRange(QCPRange(xCoords->front(), xCoords->back()),
                             QCPRange(yCoords->front(), yCoords->back()));
  colorMap->rescaleAxes();
  colorMap->rescaleDataRange();
  emit requestStart(xCoords->at(0), yCoords->at(0));
}

void MeasureControlWindow::recMeasPoint(double value) {
  measVals->at(i, j) = value;
  plotResults();
  switch (j % 2) {
  case 0:
    if (i == measVals->n() - 1) {
      if (j + 1 < measVals->m()) {
        j++;
      } else {
        emit requestStop();
        return;
      }
    } else {
      i++;
    }
    break;

  case 1:
    if (i == 0) {
      if (j + 1 < measVals->m()) {
        j++;
      } else {
        emit requestStop();
        return;
      }
    } else {
      i--;
    }
    break;
  }
  emit this->requestNextStep(xCoords->at(i), yCoords->at(j));
  // std::cout << "Next step, i:" << i << " j:" << j << std::endl;
}

// if (!(i == measVals->n() - 1 && j == measVals->m() - 1)) {
//   if (i == measVals->n() - 1) {
//     i = 0;
//     j++;
//   } else {
//     i++;
//   }
//   emit requestNextStep(xCoords->at(i), yCoords->at(j));
// } else {
//   emit requestStop();
// }
// //  if (xCoords->size() > size_t(measVals->n())) {
// //    emit requestNextStep(xCoords->at(measVals->size()));
// //  } else {
// //    emit requestStop();
// //  }
void MeasureControlWindow::plotResults() {
  colorMap->data()->setCell(i, j, measVals->at(i, j));
  colorMap->rescaleDataRange();
  customPlot->replot();
  // chart->removeAllSeries();
  // QLineSeries *line = new QLineSeries();
  // for (size_t i = 0; i < measVals->size(); i++) {
  //   line->append(xCoords->at(i), measVals->at(i));
  // }
  // chart->addSeries(line);
  // chart->createDefaultAxes();
  // chart->axisX()->setTitleText("Position (m)");
  // chart->axisY()->setTitleText("Voltage (V)");
  // chart->legend()->hide();
  // chart->update();
}

void MeasureControlWindow::stopMeasProc() { emit requestStop(); }

void MeasureControlWindow::resetZoomSlot() {
  customPlot->rescaleAxes();
  customPlot->replot();
}

void MeasureControlWindow::saveDataSlot() {
  std::ofstream dataFile;
  dataFile.open(saveName->text().toStdString().c_str());
  for (size_t ii = 0; ii < xCoords->size(); ii++) {
    for (size_t jj = 0; jj < yCoords->size(); jj++) {
      dataFile << xCoords->at(ii) << ",\t" << yCoords->at(jj) << ",\t"
               << measVals->at(ii, jj) << "\n";
    }
  }
  // for (int i = 0; i < points.size(); i++) {
  //   dataFile << points.at(i).x() << ",\t" << points.at(i).y() << "\n";
  // }
  dataFile.close();
  // if (chart->series().isEmpty()) {
  //   return;
  // } else {
  //   std::ofstream dataFile;
  //   dataFile.open(saveName->text().toStdString().c_str());
  //   QLineSeries *series = qobject_cast<QLineSeries
  //   *>(chart->series().last()); QVector<QPointF> points =
  //   series->pointsVector(); for (int i = 0; i < points.size(); i++) {
  //     dataFile << points.at(i).x() << ",\t" << points.at(i).y() << "\n";
  //   }
  //   dataFile.close();
  // }
}
