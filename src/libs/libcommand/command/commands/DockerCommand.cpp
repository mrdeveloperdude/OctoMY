#include "DockerCommand.hpp"


QString DockerCommand::name() const {
	return "docker";
}

QString DockerCommand::execute(const QString &args) {
	return QString("Executed 'docker %1'").arg(args);
}

QStringList DockerCommand::complete(const QString &args) const {
	static const QStringList dockerVerbs = {"run", "build", "pull", "push", "stop", "rm", "logs"};
	QStringList suggestions;
	for (const auto &verb : dockerVerbs) {
		if (verb.startsWith(args)) {
			suggestions.append(verb);
		}
	}
	return suggestions;
}
