#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <sqlfetch.h>
#include <QMainWindow>
namespace Ui {
class MainWindow;
}
class QMenu;
class QTimer;
class QAction;
class QCloseEvent;

class prosesbar;

namespace form {
class MainMenu : public QMainWindow {
  Q_OBJECT
 public:
  bool isloginok = false;
  explicit MainMenu();
  virtual ~MainMenu();

 Q_SIGNALS:
  void askquit();

 public Q_SLOTS:
  void recieveloginInfo(const QByteArray &cnf);

 protected:
  void closeEvent(QCloseEvent *evt) Q_DECL_OVERRIDE;

 private Q_SLOTS:
  void setuphalamanPage();

  void on_actionKeluar_triggered();

  void spawnlogindialog();

  void on_toolButton_data_master_clicked();

  void on_toolButton_data_provider_clicked();

  void importdataNomorAndCluster();

  void on_toolButton_pengaturan_clicked();
  
  void buatdatabasebaru();
  
  void opendatabase();
  
  void hidealltooldb();
  
  void showalltooldb();
  
  void on_toolButton_non_duplicate_clicked();
  
  void importdataNomorSaja();
  
  void resetdatamastersaja();
  
  void resetdataNonduplicatenomor();
  
  void importHapusDuplicateNomorDataMaster();
  
private:
  bool testkolomdb();
  
  dbbase::configdb mdb;
  QAction *act_buatdb;
  QAction *act_opendb;
  
  Ui::MainWindow *ui;
  prosesbar *bar_for_reset;
  QMenu *menudb;
  QMenu *menuimportdata;
  QMenu *resetdatamenu;
};
}

#endif  // MAINWINDOW_H
