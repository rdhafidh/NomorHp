#ifndef CUSTOMSLOTTABLE_H
#define CUSTOMSLOTTABLE_H
#include <QWidget>
#include <functional>
struct customslottable{
  QWidget*wgt;
  QIcon icon;
    QString name;
  std::function<void()> func;
};

#endif // CUSTOMSLOTTABLE_H
