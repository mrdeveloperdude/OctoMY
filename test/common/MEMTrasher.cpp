#include "MEMTrasher.hpp"


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
		out = memory.toLong()*1024L;
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
