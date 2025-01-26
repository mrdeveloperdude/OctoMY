#ifndef LSCOMMAND_HPP
#define LSCOMMAND_HPP

#include <QDir>
#include <QStringList>

#include "command/Command.hpp"

class LsCommand : public Command {
	// Command interface
public:
	QString name() const override;
	QString execute(const QString &args) override;
	QStringList complete(const QString &args) const override;
};


#endif // LSCOMMAND_HPP
