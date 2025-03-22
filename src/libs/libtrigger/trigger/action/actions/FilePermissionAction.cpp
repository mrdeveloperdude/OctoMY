#include "FilePermissionAction.hpp"
#include "trigger/context/ActionContext.hpp"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QString>

#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

FilePermissionAction::FilePermissionAction()
	: ActionBase("Change permissions on file [file] to mode [mode], user [user] and group [group].")
{
}

bool FilePermissionAction::execute(ActionContext &)
{
	QString filePath = phrase()->parameterValue("file").toString();
	QString modeStr  = phrase()->parameterValue("mode").toString();
	QString userStr  = phrase()->parameterValue("user").toString();
	QString groupStr = phrase()->parameterValue("group").toString();
	
	if (filePath.isEmpty())
	{
		// Mandatory file parameter missing; no-op.
		return false;
	}
	
	QFileInfo fileInfo(filePath);
	if (!fileInfo.exists())
	{
		// File does not exist; no-op.
		return false;
	}
	
	// If mode is provided, update file permissions using Qt.
	if (!modeStr.isEmpty())
	{
		bool ok = false;
		uint mode = modeStr.toUInt(&ok, 8);
		if (!ok)
		{
			qWarning() << "FilePermissionAction: Invalid mode parameter:" << modeStr;
			return false;
		}
		
		QFile::Permissions perms;
		// Extract owner, group, and other permissions from the octal mode.
		uint owner = (mode >> 6) & 7;
		uint group = (mode >> 3) & 7;
		uint other = mode & 7;
		
		if (owner & 4)
			perms |= QFileDevice::ReadOwner;
		if (owner & 2)
			perms |= QFileDevice::WriteOwner;
		if (owner & 1)
			perms |= QFileDevice::ExeOwner;
		
		if (group & 4)
			perms |= QFileDevice::ReadGroup;
		if (group & 2)
			perms |= QFileDevice::WriteGroup;
		if (group & 1)
			perms |= QFileDevice::ExeGroup;
		
		if (other & 4)
			perms |= QFileDevice::ReadOther;
		if (other & 2)
			perms |= QFileDevice::WriteOther;
		if (other & 1)
			perms |= QFileDevice::ExeOther;
		
		if (!QFile::setPermissions(filePath, perms))
		{
			qWarning() << "FilePermissionAction: Failed to change permissions for" << filePath;
			return false;
		}
	}
	
	// For user and group, if parameters are non-empty then change them.
	// If empty, keep the current owner/group.
	uid_t uid = fileInfo.ownerId();
	gid_t gid = fileInfo.groupId();
	bool needChown = false;
	
	if (!userStr.isEmpty())
	{
		bool userIsNumeric;
		uid = static_cast<uid_t>(userStr.toUInt(&userIsNumeric, 10));
		if (!userIsNumeric)
		{
			struct passwd *pw = getpwnam(userStr.toLocal8Bit().constData());
			if (!pw)
			{
				qWarning() << "FilePermissionAction: User not found:" << userStr;
				return false;
			}
			uid = pw->pw_uid;
		}
		needChown = true;
	}
	
	if (!groupStr.isEmpty())
	{
		bool groupIsNumeric;
		gid = static_cast<gid_t>(groupStr.toUInt(&groupIsNumeric, 10));
		if (!groupIsNumeric)
		{
			struct group *gr = getgrnam(groupStr.toLocal8Bit().constData());
			if (!gr)
			{
				qWarning() << "FilePermissionAction: Group not found:" << groupStr;
				return false;
			}
			gid = gr->gr_gid;
		}
		needChown = true;
	}
	
	// If either user or group should be changed, call chown.
	if (needChown)
	{
		if (chown(filePath.toLocal8Bit().constData(), uid, gid) != 0)
		{
			qWarning() << "FilePermissionAction: Failed to change owner/group for" << filePath;
			return false;
		}
	}
	return true;
}

