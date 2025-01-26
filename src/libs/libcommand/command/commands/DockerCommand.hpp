#ifndef DOCKERCOMMAND_HPP
#define DOCKERCOMMAND_HPP

#include <QStringList>

#include "command/Command.hpp"

class DockerCommand : public Command {
	// Command interface
public:
	QString name() const override;
	QString execute(const QString &args) override;
	QStringList complete(const QString &args) const override;
};




#endif // DOCKERCOMMAND_HPP
