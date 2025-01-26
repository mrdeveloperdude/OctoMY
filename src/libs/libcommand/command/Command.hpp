#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <QString>
#include <QStringList>

class Command {
public:
	virtual ~Command() = default;
	
	// Returns the name of the command (e.g., "docker", "ls")
	virtual QString name() const = 0;
	
	// Execute the command with parameters
	virtual QString execute(const QString &args) = 0;
	
	// Provide completion suggestions for the arguments
	virtual QStringList complete(const QString &args) const = 0;
};


#endif // COMMAND_HPP
