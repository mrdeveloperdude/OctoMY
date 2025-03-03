#include "LogStorage.hpp"


#include <QDir>
#include <QDateTime>
#include <QDebug>
#include <QTextStream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>


LogStorage::LogStorage(QObject *parent)
	: QObject(parent), m_fd(-1), m_mapPtr(nullptr)
{
}

LogStorage::~LogStorage()
{
	if (m_mapPtr) {
		munmap(m_mapPtr, m_maxSize);
	}
	if (m_fd != -1) {
		close(m_fd);
	}
}

void LogStorage::configure(const QString &logDir, qint64 maxSize){
	m_logDir = logDir;
	m_maxSize = maxSize;
	QDir().mkpath(m_logDir); // Ensure directory exists
	rotateLog(); // Open current day's log file
}

void LogStorage::rotateLog()
{
	// Close existing file
	if (m_fd != -1) {
		munmap(m_mapPtr, m_maxSize);
		close(m_fd);
	}
	
	// Create new log file for today
	QString filename = m_logDir + "/" + QDate::currentDate().toString("yyyy-MM-dd") + ".log";
	m_logFile.setFileName(filename);
	if (!m_logFile.open(QIODevice::ReadWrite | QIODevice::Append)) {
		qWarning() << "Failed to open log file:" << filename;
		return;
	}
	
	// Memory map the file
	m_fd = open(qPrintable(filename), O_RDWR);
	if (m_fd == -1) {
		qWarning() << "Failed to open file descriptor for mmap";
		return;
	}
	
	if (ftruncate(m_fd, m_maxSize) == -1) {
		qWarning() << "Failed to resize log file";
		close(m_fd);
		m_fd = -1;
		return;
	}
	
	m_mapPtr = mmap(nullptr, m_maxSize, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
	if (m_mapPtr == MAP_FAILED) {
		qWarning() << "mmap failed";
		close(m_fd);
		m_fd = -1;
		return;
	}
	
	m_mappedData = QByteArray::fromRawData(static_cast<char *>(m_mapPtr), m_maxSize);
}

void LogStorage::appendLog(const QString &text)
{
	if (m_fd == -1) return;
	
	QString logEntry = QDateTime::currentDateTime().toString("HH:mm:ss.zzz ") + text + "\n";
	qint64 offset = m_mappedData.size();
	m_offsets.append(offset);
	
	if (offset + logEntry.size() >= m_maxSize) {
		rotateLog();
		offset = 0;
	}
	
	memcpy(static_cast<char *>(m_mapPtr) + offset, logEntry.toUtf8().constData(), logEntry.size());
	msync(m_mapPtr, m_maxSize, MS_SYNC);
}

QStringList LogStorage::readLogs(qint64 maxEntries)
{
	QStringList logs;
	qint64 start = qMax<qint64>(0, m_offsets.size() - maxEntries);
	
	for (qint64 i = start; i < m_offsets.size(); ++i) {
		qint64 offset = m_offsets[i];
		QString entry = QString::fromUtf8(static_cast<char *>(m_mapPtr) + offset);
		logs.append(entry);
	}
	
	return logs;
}
