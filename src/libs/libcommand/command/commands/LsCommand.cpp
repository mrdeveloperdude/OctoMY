#include "LsCommand.hpp"

#include <QDir>
#include <QStringList>

QString LsCommand::name() const {
	return "ls";
}

QString LsCommand::execute(const QString &args) {
	return QString("Executed 'ls %1'").arg(args);
}

QStringList LsCommand::complete(const QString &args) const {
	QDir dir(QDir::currentPath());
	QStringList entries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	QStringList suggestions;
	for (const auto &entry : entries) {
		if (entry.startsWith(args)) {
			suggestions.append(entry);
		}
	}
	return suggestions;
}
