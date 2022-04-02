#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

public:
  void setDefaultDisposition();
  void setDefaultComponent();
  void readSourceFileIntoTextEdit(QString filename);
private slots:
  void on_actionOpen_triggered();
  void qPushButtonRunSlot();

private:
  Ui::MainWindow *ui;

  QString mQStringFilename;

  QLabel *mQLabelExternalInclude;
  QTextEdit *mQTextEditExternalInclude;

  QLabel *mQLabelSourceFile;
  QTextEdit *mQTextEditSourceFile;
  QPushButton *mQPushButtonRun;

  QLabel *mQLabelLog;
  QTextEdit *mQTextEditLog;
};
#endif // MAINWINDOW_H
