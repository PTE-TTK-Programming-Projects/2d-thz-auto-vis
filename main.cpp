#include "./hostwindow.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  HostWindow w;
  w.show();
  return app.exec();
}
