#include "commentswindow.h"

#include "ui_commentswindow.h"

CommentsWindow::CommentsWindow(QStringList comments, QWidget *parent)
    : QWidget(parent), ui(new Ui::CommentsWindow) {
  ui->setupUi(this);
  ui->listView->setModel(model);
  for (int i = 0; i < comments.size(); i++) {
    model->appendRow(QList<QStandardItem *>()
                     << new QStandardItem(comments.at(i)));
  }
}

CommentsWindow::~CommentsWindow() { delete ui; }
