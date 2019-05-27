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
	void setFilenameBackend(QString filename) Q_DECL_OVERRIDE;
	QString filenameBackend() Q_DECL_OVERRIDE;
	bool existsBackend() Q_DECL_OVERRIDE;
	bool clearBackend() Q_DECL_OVERRIDE;
	bool saveBackend(QVariantMap data) Q_DECL_OVERRIDE;
	QVariantMap loadBackend(bool &ok) Q_DECL_OVERRIDE;

};


#endif
// JSONASYNCBACKEND_HPP
