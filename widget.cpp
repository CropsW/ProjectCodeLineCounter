#include "widget.h"

#include "ui_widget.h"

Widget::Widget(QWidget* parent) : QWidget(parent), ui(new Ui::Widget) {
  ui->setupUi(this);
  ui->scrollArea->setFrameShape(QFrame::NoFrame);

  timer->setInterval(500);
  timer->start();

  connect(timer, SIGNAL(timeout()), this, SLOT(timeOut()));
  connect(&cnter, SIGNAL(sendFileResult(QVariantMap)), this,
          SLOT(getFileResult(QVariantMap)));
  connect(&cnter, SIGNAL(sendTotalResult(QVariantMap)), this,
          SLOT(getTotalResult(QVariantMap)));
  ui->tableView->setModel(resultTabModel);
  ui->tableView->horizontalHeader()->setSectionResizeMode(
      QHeaderView::ResizeToContents);
  ui->label_loading->setVisible(0);
  resultTabModel->setColumnCount(4);
  resultTabModel->setHeaderData(0, Qt::Horizontal, tr("File Name"));
  resultTabModel->setHeaderData(1, Qt::Horizontal,
                                tr("File Lines without Comments"));
  resultTabModel->setHeaderData(2, Qt::Horizontal, tr("File Chars"));
  resultTabModel->setHeaderData(3, Qt::Horizontal, tr("File Lines"));
}

Widget::~Widget() { delete ui; }

void Widget::timeOut() {
  updateLabel();
  updateExtStatusLabel();
  ui->toolButton->setEnabled((ui->blacklist->count() != 0) &&
                             (!ui->blacklist->selectedItems().isEmpty()));
}

void Widget::updateLabel() {
  if (ui->lineEdit->text().simplified().isEmpty()) {
    ui->label_6->setText(
        QString("<font color=orange>%1</font>")
            .arg(tr(
                "Project dir is not filled, so it is set to current path.")));
    return;
  }
  QDir dir = QDir(ui->lineEdit->text());
  if (dir.exists()) {
    ui->label_6->setText(
        QString("<font color=green>%1</font>").arg(tr("All Right")));
    ui->lineEdit->setStyleSheet("color:green");
    return;
  }
  ui->label_6->setText(QString("<font color=red>%1</font>")
                           .arg(tr("Project dir does not exist!")));
  ui->lineEdit->setStyleSheet("color:red");
}

void Widget::openFile() {
  ui->lineEdit->setText(QFileDialog::getExistingDirectory(
      this, tr("Open a Project Dir"), ui->lineEdit->text()));
}

void Widget::updateExtStatusLabel() {
  if (ui->lineEdit_ext->text().simplified().isEmpty()) {
    ui->label_22->setText(
        QString("<font color=green>%1</font>").arg(tr("Empty")));
    return;
  }
  if (ui->lineEdit_ext->text().contains(QRegExp("^\\w+(,\\w+)*$"))) {
    ui->label_22->setText(
        QString("<font color=green>%1</font>").arg(tr("All Right")));
    ui->lineEdit_ext->setStyleSheet("color:green");
  } else {
    ui->label_22->setText(
        QString("<font color=red>%1</font>").arg(tr("Format Mismatch!")));
    ui->lineEdit_ext->setStyleSheet("color:red");
  }
}

void Widget::blacklistAdd() {
  QDir proDir = QDir(ui->lineEdit->text());
  if ((proDir.isEmpty()) || (!proDir.exists())) {
    QMessageBox::critical(this, tr("Dir Blacklist Add Item"),
                          tr("Before operation, please make sure that Project "
                             "Dir has been filled in and exists"));
  }
  QDir dir = QDir(QFileDialog::getExistingDirectory(this, tr("Open a Dir"),
                                                    ui->lineEdit->text()));
  if (dir.path() == ".") {
    return;
  }
  dir.makeAbsolute();
  proDir.makeAbsolute();
  if ((!dir.path().contains(proDir.path(), Qt::CaseInsensitive)) ||
      dir.path() == proDir.path()) {
    QMessageBox::warning(
        this, tr("Add Blacklist Item"),
        tr("Items in the blacklist can only be subdirs of the Project Dir."));
    return;
  }
  dir.setPath(
      dir.path().mid(proDir.path().size() + (!proDir.path().endsWith("/"))));
  if (!dir.isEmpty() && !isInBlacklist(dir.path())) {
    ui->blacklist->addItem(dir.path());
  }
}

bool Widget::isInBlacklist(QString s) {
  bool flag = false;
  for (int i = 0; i < ui->blacklist->count(); i++) {
    if (ui->blacklist->item(i)->text() == s) {
      flag = true;
      break;
    }
  }
  return flag;
}

void Widget::blacklistDel() { delete ui->blacklist->currentItem(); }

void Widget::extBlackListChanged(int, int) { timeOut(); }

void Widget::startCount() {
  QList<QString> dirBlacklist = QList<QString>();
  QStringList fileExtBlacklist = QStringList();
  QDir dir = QDir(ui->lineEdit->text());
  if (!dir.exists()) {
    QMessageBox::critical(this, tr("Start Count"),
                          tr("Error:\n  Project dir does not exist!"));
    return;
  }
  if (ui->checkBox->isChecked()) {
    for (int i = 0; i < ui->blacklist->count(); i++) {
      dirBlacklist << ui->blacklist->item(i)->text();
    }
  }
  if (ui->checkBox_2->isChecked() &&
      !ui->lineEdit_ext->text().simplified().isEmpty()) {
    if (!ui->lineEdit_ext->text().contains(QRegExp("^\\w+(,\\w+)*$"))) {
      QMessageBox::critical(
          this, tr("Start Count"),
          tr("Error:\n  FileExt Blacklist: Format Mismatch!"));
      return;
    }
    fileExtBlacklist = ui->lineEdit_ext->text().split(',');
  }
  //  qDebug() << dir << dirBlacklist << fileExtBlacklist;

  cnter.config(
      dir, dirBlacklist, fileExtBlacklist, ui->checkBox_3->isChecked(),
      std::make_tuple(ui->checkBox_6->isChecked(), ui->checkBox_7->isChecked(),
                      ui->spinBox->value()));
  clearResult();
  cnter.start();
  ui->label_loading->setVisible(1);
}
void Widget::getFileResult(QVariantMap result) {
  resultTabModel->appendRow(
      QList<QStandardItem*>()
      << new QStandardItem(result["fileName"].toString())
      << new QStandardItem(
             QString::number(result["fileLinesWithoutComments"].toInt()))
      << new QStandardItem(QString::number(result["fileChars"].toInt()))
      << new QStandardItem(QString::number(result["fileLines"].toInt())));
  ui->tableView->resizeColumnsToContents();
}

void Widget::getTotalResult(QVariantMap result) {
  ui->label_loading->setVisible(0);
  ui->label_resultA->setText(result["totCnt"].toString());
  ui->label_resultB->setText(result["totLinesWithoutComments"].toString());
  ui->label_resultC->setText(result["totChars"].toString());
  ui->label_resultD->setText(result["totLines"].toString());
  comments = result["comments"].toStringList();
}

void Widget::clearResult() {
  resultTabModel->clear();
  resultTabModel->setColumnCount(4);
  resultTabModel->setHeaderData(0, Qt::Horizontal, tr("File Name"));
  resultTabModel->setHeaderData(1, Qt::Horizontal,
                                tr("File Lines without Comments"));
  resultTabModel->setHeaderData(2, Qt::Horizontal, tr("File Chars"));
  resultTabModel->setHeaderData(3, Qt::Horizontal, tr("File Lines"));
  ui->label_resultA->clear();
  ui->label_resultB->clear();
  ui->label_resultC->clear();
  ui->label_resultD->clear();
  comments.clear();
}

void Widget::showComments() {
  CommentsWindow* cW = new CommentsWindow(comments);
  cW->show();
}

void Widget::closeEvent(QCloseEvent* event) {
  if (cnter.isRunning()) {
    QMessageBox::warning(
        this, tr("Quit Application"),
        tr("The program cannot be closed because the task is executing."));
    event->ignore();
  } else {
    event->accept();
  }
}
