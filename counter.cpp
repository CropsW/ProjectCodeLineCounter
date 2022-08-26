#include "counter.h"

Counter::Counter(QObject *parent) : QThread(parent) {}

Counter::~Counter() {}

void Counter::config(QDir dir, QList<QString> dirBlacklist,
                     QStringList fileExtBlacklist, bool isCharCountBlank,
                     std::tuple<bool, bool, int> lineOptions) {
  m_isCharCountBlank = isCharCountBlank;
  std::tie(m_isLineCountBlankLine, m_isLineDoNotCountCharsLessThanN, m_N) =
      lineOptions;
  dir.makeAbsolute();
  m_dir = dir;
  m_dirBlacklist = dirBlacklist;
  m_fileExtBlacklist = fileExtBlacklist;
  m_total.insert("totCnt", 0);
  m_total.insert("totLinesWithoutComments", 0);
  m_total.insert("totChars", 0);
  m_total.insert("totLines", 0);
  m_total.insert("comments", QStringList());
}

int Counter::relaMidSize() {
  return m_dir.path().size() + (!m_dir.path().endsWith("/"));
}

void Counter::run() {
  QDir::setCurrent(m_dir.path());
  m_dir.makeAbsolute();
  m_total["totCnt"].clear();
  m_total["totLinesWithoutComments"].clear();
  m_total["totChars"].clear();
  m_total["totLines"].clear();
  m_total["comments"].clear();
  readFiles();
  emit sendTotalResult(m_total);
}

void Counter::readFiles() {
  QDirIterator dir_iterator(m_dir.path(), QStringList() << "*",
                            QDir::Files | QDir::NoSymLinks,
                            QDirIterator::Subdirectories);
  while (dir_iterator.hasNext()) {
    dir_iterator.next();
    QFileInfo file_info = dir_iterator.fileInfo();
    QString file_path = file_info.absoluteFilePath().mid(relaMidSize());
    bool flag = true;
    foreach (QString dirBL, m_dirBlacklist) {
      if (file_info.absoluteDir().path().mid(relaMidSize()) == dirBL) {
        flag = false;
        break;
      }
    }
    if (flag) {
      foreach (QString extBL, m_fileExtBlacklist) {
        if (file_info.suffix() == extBL) {
          flag = false;
          break;
        }
      }
      if (flag) {
        QVariantMap map = fileCount(file_path);
        emit sendFileResult(map);
      }
    }
  }
}

QVariantMap Counter::fileCount(QString fp) {
  QFile file(fp);
  QVariantMap map = QVariantMap();
  int fileLinesWithoutComments = 0, fileLines = 0;
  long long fileChars = 0;
  file.open(QIODevice::ReadOnly | QIODevice::Text);
  QTextStream stream(&file);
  stream.setCodec("utf-8");
  while (!stream.atEnd()) {
    QString line = stream.readLine();
    // count chars
    if (m_isCharCountBlank) {
      fileChars += line.size();
    } else {
      fileChars += line.simplified().size();
    }
    // count line
    if (m_isLineDoNotCountCharsLessThanN && (line.simplified().size() < m_N)) {
      continue;
    }
    if ((m_isLineCountBlankLine || (!line.simplified().isEmpty())) &&
        (!line.simplified().startsWith("//"))) {
      fileLines++;
      fileLinesWithoutComments++;
    }
    if ((line.simplified().startsWith("//")) ||
        ((line.simplified().startsWith("/*") &&
          (line.simplified().endsWith("*/"))))) {
      fileLines++;
      m_total["comments"].setValue(m_total["comments"].toStringList()
                                   << line.simplified());
    }
  }
  file.close();
  map.insert("fileName", fp);
  map.insert("fileLinesWithoutComments", fileLinesWithoutComments);
  map.insert("fileChars", fileChars);
  map.insert("fileLines", fileLines);
  m_total["totCnt"].setValue(m_total["totCnt"].toInt() + 1);
  m_total["totLinesWithoutComments"].setValue(
      m_total["totLinesWithoutComments"].toInt() + fileLinesWithoutComments);
  m_total["totChars"].setValue(m_total["totChars"].toLongLong() + fileChars);
  m_total["totLines"].setValue(m_total["totLines"].toInt() + fileLines);
  return map;
}
