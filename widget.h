#ifndef WIDGET_H
#define WIDGET_H

#include <QCloseEvent>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QMovie>
#include <QRegExp>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStringList>
#include <QTableWidgetItem>
#include <QTimer>
#include <QVariantMap>
#include <QWidget>
#include <tuple>

#include "commentswindow.h"
#include "counter.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget {
  Q_OBJECT

 public:
  Widget(QWidget *parent = nullptr);
  ~Widget();

 private slots:
  void timeOut();
  void openFile();
  void blacklistAdd();
  void blacklistDel();
  void extBlackListChanged(int, int);
  void startCount();
  void getFileResult(QVariantMap);
  void getTotalResult(QVariantMap);
  void clearResult();
  void showComments();

 protected:
  void closeEvent(QCloseEvent *event);

 private:
  Ui::Widget *ui;
  QTimer *timer = new QTimer();
  void updateLabel();
  void updateExtStatusLabel();
  bool isInBlacklist(QString);
  QStandardItemModel *resultTabModel = new QStandardItemModel(this);
  Counter cnter;
  QStringList comments;
};
#endif  // WIDGET_H
