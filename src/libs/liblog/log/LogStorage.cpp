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
	appendLog("Log closed ---------------------------");
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
	appendLog("Log opened ---------------------------");
}


bool LogStorage::clearLogs()
{
	// unmap + close existing
	if (m_mapPtr) {
		m_logFile.unmap(m_mapPtr);
		m_mapPtr = nullptr;
	}
	if (m_logFile.isOpen()) {
		m_logFile.close();
	}
	
	// truncate the file
	const QString filename = m_logFile.fileName();
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		qWarning() << "Failed to clear log file:" << filename;
		return false;
	}
	file.close();
	
	// remap
	rotateLog();
	return true;
}

bool LogStorage::reloadLogs()
{
	// just unmap + rotate()
	if (m_mapPtr) {
		m_logFile.unmap(m_mapPtr);
		m_mapPtr = nullptr;
	}
	if (m_logFile.isOpen()) {
		m_logFile.close();
	}
	rotateLog();
	return (m_mapPtr != nullptr);
}


void LogStorage::rotateLog()
{
	// Unmap previous mapping if it exists
	if (m_mapPtr) {
		m_logFile.unmap(m_mapPtr);
		m_mapPtr = nullptr;
	}
	if (m_logFile.isOpen()) {
		m_logFile.close();
	}
	
	// Create (or open) today’s log file
	QString filename = QDir(m_logDir)
						   .filePath(QDate::currentDate().toString("yyyy-MM-dd") + ".log");
	m_logFile.setFileName(filename);
	if (!m_logFile.open(QIODevice::ReadWrite | QIODevice::Append)) {
		qWarning() << "Failed to open log file:" << filename;
		return;
	}
	
	// Ensure it’s at least m_maxSize bytes long
	if (m_logFile.size() < m_maxSize) {
		if (!m_logFile.resize(m_maxSize)) {
			qWarning() << "Failed to resize log file:" << filename;
			m_logFile.close();
			return;
		}
	}
	
	// Memory‐map the whole region
	m_mapPtr = m_logFile.map(0, m_maxSize);
	if (!m_mapPtr) {
		qWarning() << "Failed to map log file";
		m_logFile.close();
		return;
	}
	
	// Keep a non‐owning QByteArray view around if you like
	m_mappedData = QByteArray::fromRawData(
		reinterpret_cast<const char*>(m_mapPtr),
		m_maxSize
		);
	
	// --- NEW: rebuild offsets & currentOffset from existing content ---
	m_offsets.clear();
	qint64 prev = 0;
	const char *buf = reinterpret_cast<const char*>(m_mapPtr);
	
	for (qint64 i = 0; i < m_maxSize; ++i) {
		if (buf[i] == '\n') {
			// record the start of this line
			m_offsets.append(prev);
			prev = i + 1;
		}
		else if (buf[i] == '\0') {
			// hit padding / end of real data
			break;
		}
	}
	
	m_currentOffset = prev;
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
	emit logChanged();
}


QStringList LogStorage::readLogs(qint64 maxEntries)
{
	QStringList logs;
	qint64 startIndex = qMax<qint64>(0, m_offsets.size() - maxEntries);
	for (qint64 i = startIndex; i < m_offsets.size(); ++i) {
		qint64 offset = m_offsets[i];
		qint64 endOffset = (i + 1 < m_offsets.size() ? m_offsets[i + 1] : m_currentOffset);
		qint64 length = endOffset - offset;
		const char *entryStart = reinterpret_cast<const char*>(m_mapPtr) + offset;
		QString entry = QString::fromUtf8(entryStart, length);
		while (!entry.isEmpty() && (entry.endsWith('\n') || entry.endsWith('\r'))) {
			entry.chop(1);
		}
		logs.append(entry);
	}
	return logs;
}
