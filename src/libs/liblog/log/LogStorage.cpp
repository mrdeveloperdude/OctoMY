#include "LogStorage.hpp"

#include <QDir>
#include <QDateTime>
#include <QDebug>
#include <QTextStream>

LogStorage::LogStorage(QObject *parent)
	: QObject(parent), m_logFile(), m_mapPtr(nullptr), m_currentOffset(0)
{
}

LogStorage::~LogStorage()
{
	if (m_mapPtr) {
		m_logFile.unmap(m_mapPtr);
		m_mapPtr = nullptr;
	}
	if (m_logFile.isOpen()) {
		m_logFile.close();
	}
}

void LogStorage::configure(const QString &logDir, qint64 maxSize)
{
	m_logDir = logDir;
	m_maxSize = maxSize;
	QDir().mkpath(m_logDir); // Ensure directory exists
	rotateLog(); // Open current day's log file
}

void LogStorage::rotateLog()
{
	// Unmap previous mapping if it exists
	if (m_mapPtr) {
		m_logFile.unmap(m_mapPtr);
		m_mapPtr = nullptr;
	}
	if (m_logFile.isOpen()){
		m_logFile.close();
	}
	
	// Create new log file for today
//	QString filename = m_logDir + "/" + QDate::currentDate().toString("yyyy-MM-dd") + ".log";
	QString filename = QDir(m_logDir).filePath(QDate::currentDate().toString("yyyy-MM-dd") + ".log");
	m_logFile.setFileName(filename);
	if (!m_logFile.open(QIODevice::ReadWrite | QIODevice::Append)) {
		qWarning() << "Failed to open log file:" << filename;
		return;
	}
	
	// Ensure file is at least m_maxSize bytes long
	if (m_logFile.size() < m_maxSize) {
		if (!m_logFile.resize(m_maxSize)) {
			qWarning() << "Failed to resize log file:" << filename;
			m_logFile.close();
			return;
		}
	}
	
	// Memory map the file using QFile::map
	m_mapPtr = m_logFile.map(0, m_maxSize);
	if (!m_mapPtr) {
		qWarning() << "Failed to map log file";
		m_logFile.close();
		return;
	}
	
	// Create a QByteArray view (non-owning) of the mapped memory
	m_mappedData = QByteArray::fromRawData(reinterpret_cast<const char*>(m_mapPtr), m_maxSize);
	
	// Reset offsets and current write position
	m_offsets.clear();
	m_currentOffset = 0;
}

void LogStorage::appendLog(const QString &text)
{
	if (!m_mapPtr){
		return;
	}
	
	QString logEntry = QDateTime::currentDateTime().toString("HH:mm:ss.zzz ") + text + "\n";
	QByteArray logData = logEntry.toUtf8();
	qint64 entrySize = logData.size();
	
	// Check if appending the entry would exceed the mapped area
	if (m_currentOffset + entrySize >= m_maxSize) {
		rotateLog();
	}
	
	// Copy the log entry into the mapped memory
	memcpy(m_mapPtr + m_currentOffset, logData.constData(), entrySize);
	m_offsets.append(m_currentOffset);
	m_currentOffset += entrySize;
	
	// Flush changes to disk
	m_logFile.flush();
}

QStringList LogStorage::readLogs(qint64 maxEntries)
{
	QStringList logs;
	qint64 startIndex = qMax<qint64>(0, m_offsets.size() - maxEntries);
	
	for (qint64 i = startIndex; i < m_offsets.size(); ++i) {
		qint64 offset = m_offsets[i];
		// Interpret the entry starting at the given offset
		const char *entryStart = reinterpret_cast<const char*>(m_mapPtr) + offset;
		QString entry = QString::fromUtf8(entryStart);
		logs.append(entry);
	}
	return logs;
}
