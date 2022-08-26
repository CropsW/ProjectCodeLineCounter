#ifndef COUNTER_H
#define COUNTER_H

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QList>
#include <QMap>
#include <QMessageBox>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QThread>
#include <QVariantMap>
#include <tuple>

class Counter : public QThread {
  Q_OBJECT
 public:
  Counter(QObject *parent = nullptr);
  ~Counter();
  void config(QDir dir, QList<QString> dirBlacklist,
              QStringList fileExtBlacklist, bool charOptions,
              std::tuple<bool, bool, int> lineOptions);

 protected:
  void run();

 private:
  int relaMidSize();
  bool m_isCharCountBlank, m_isLineCountBlankLine,
      m_isLineDoNotCountCharsLessThanN;
  int m_N;
  QDir m_dir;
  QList<QString> m_dirBlacklist, m_fileExtBlacklist;
  QVariantMap m_total = QVariantMap();
  void readFiles();
  QVariantMap fileCount(QString);
 signals:
  void sendFileResult(QVariantMap);
  void sendTotalResult(QVariantMap);
  void fileOpenErr();
};

#endif  // COUNTER_H
