#ifndef TABWIDGETUTAMA_H
#define TABWIDGETUTAMA_H

#include <QTabWidget>

class TabWidgetUtama : public QTabWidget {
  Q_OBJECT
 public:
  explicit TabWidgetUtama(QWidget *parent);
  void manualremovetab(int idx);

  ~TabWidgetUtama();
  void asktorefreshtabDataMaster();

  void remembercloseandOpenTab(int forceopen_window_id=1);

 Q_SIGNALS:
  void emitOpentabDataMaster();
  void emitOpentabDataNonDuplicate();

 private Q_SLOTS:

  void emitclosetab(int idx);
};

#endif  // TABWIDGETUTAMA_H
