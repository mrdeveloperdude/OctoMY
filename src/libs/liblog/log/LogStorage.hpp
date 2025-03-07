#ifndef LOGSTORAGE_H
#define LOGSTORAGE_H


#include <QObject>
#include <QFile>
#include <QDate>
#include <QVector>

class LogStorage : public QObject {
	Q_OBJECT

private:
	QFile m_logFile;
	QString m_logDir;
	qint64 m_maxSize {0};
	uchar *m_mapPtr {nullptr};
	QByteArray m_mappedData;
	QVector<qint64> m_offsets;
	qint64 m_currentOffset {0};

public:
	explicit LogStorage(QObject *parent = nullptr);
	~LogStorage();

private:
	void rotateLog();

public:
	void configure(const QString &logDir, qint64 maxSize = 10 * 1024 * 1024);
	void appendLog(const QString &text);
	QStringList readLogs(qint64 maxEntries = 100);

};

#endif // LOGSTORAGE_H
