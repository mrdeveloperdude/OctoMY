#include "CopyFileAction.hpp"

#include "trigger/context/ActionContext.hpp"

#include <QDebug>
#include <QFile>
#include <QString>



CopyFileAction::CopyFileAction() 
	: ActionBase("Copy file from [source] to [destination].")
{
}

bool CopyFileAction::execute(ActionContext &)
{
	// Retrieve parameters from the stored phrase.
	QString source = phrase()->parameterValue("source").toString();
	QString destination = phrase()->parameterValue("destination").toString();
	
	if (source.isEmpty() )
	{
		qWarning() << "CopyFileAction: Missing source parameter.";
		return false;
	}
	
	if (destination.isEmpty())
	{
		qWarning() << "CopyFileAction: Missing destination parameter.";
		return false;
	}
	
	if (!QFile::exists(source))
		
	{
		qWarning() << "CopyFileAction: Source file does not exist:" << source;
		return false;
	}
	
	if (!QFile::copy(source, destination))
	{
		qWarning() << "CopyFileAction: Failed to copy file from" << source << "to" << destination;
	}
	
	return true;
}

