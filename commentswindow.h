#ifndef COMMENTSWINDOW_H
#define COMMENTSWINDOW_H

#include <QStandardItem>
#include <QStandardItemModel>
#include <QStringList>
#include <QWidget>

namespace Ui {
class CommentsWindow;
}

class CommentsWindow : public QWidget {
  Q_OBJECT

 public:
  explicit CommentsWindow(QStringList comments, QWidget *parent = nullptr);
  ~CommentsWindow();

 private:
  Ui::CommentsWindow *ui;
  QStandardItemModel *model = new QStandardItemModel(this);
};

#endif  // COMMENTSWINDOW_H
