#include "MEMTrasher.hpp"

#include <QProcess>


//Adaptively find available memory to trash, and return 90% of that to leave a margin.
qint64 MEMTrasher::findAvailableMem(quint64 MINREQ, quint64 required)
{
	qDebug()<<"LOOKING FOR AVAILABLE MEMORY WITH " <<utility::time::humanReadableSize(MINREQ)<<utility::time::humanReadableSize(required);
	void *mem = nullptr;
	while (nullptr == mem) {
		if (required < MINREQ) {
			return 0;
		}
		qDebug()<<"Trying " <<utility::time::humanReadableSize(required);
		mem = malloc (required);
		required=(required*900)/1000;
	}
	free (mem);

	mem = malloc (required);
	if (nullptr == mem) {
		qWarning()<<"Cannot enough allocate memory";
		return 0;
	}
	free (mem);
	qDebug()<<"FOUND AVAILABLE MEMORY TO BE " <<utility::time::humanReadableSize(required);
	return static_cast<qint64>(required);
}


quint64 totalAvailableMemory()
{
	quint64 out=0;
	// Linux (/proc/meminfo)
	#if (defined (Q_OS_LINUX))
	{
		QProcess p;
		p.start("awk", QStringList() << "/MemTotal/ { print $2 }" << "/proc/meminfo");
		p.waitForFinished();
		QString memory = p.readAllStandardOutput();
		out = static_cast<quint64>(memory.toLong()) * static_cast<quint64>(1024);


		p.close();
	}
	// Mac (sysctl)
	#elif (defined (Q_OS_MAC))
	{
		QProcess p;
		p.start("sysctl", QStringList() << "kern.version" << "hw.physmem");
		p.waitForFinished();
		QString system_info = p.readAllStandardOutput();
		out<<system_info;
		p.close();
	}
	// Windows (GlobalMemoryStatusEx)
	#elif (defined (Q_OS_WIN) || defined (Q_OS_WIN32) || defined (Q_OS_WIN64))
	{
		MEMORYSTATUSEX memory_status;
		ZeroMemory(&memory_status, sizeof(MEMORYSTATUSEX));
		memory_status.dwLength = sizeof(MEMORYSTATUSEX);
		if (GlobalMemoryStatusEx(&memory_status)) {
			out=memory_status.ullTotalPhys;
		}
	}
	#endif
	return out;
}
