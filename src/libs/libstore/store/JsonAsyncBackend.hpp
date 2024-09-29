#ifndef JSONASYNCBACKEND_HPP
#define JSONASYNCBACKEND_HPP

#include "AsyncStore.hpp"
#include "uptime/ConfigureHelper.hpp"

#include <QVariantMap>

class JsonAsyncBackend : public AsyncBackend<QVariantMap>
{
private:
	QString mFilename;
	ConfigureHelper mConfigureHelper;

public:
	explicit JsonAsyncBackend();
	virtual ~JsonAsyncBackend();

public:
	void configure(QString filename);

	// AsyncBackend interface
public:
	void setFilenameBackend(QString filename) override;
	QString filenameBackend() override;
	bool existsBackend() override;
	bool clearBackend() override;
	bool saveBackend(QVariantMap data) override;
	QVariantMap loadBackend(bool &ok) override;

};


#endif
// JSONASYNCBACKEND_HPP
