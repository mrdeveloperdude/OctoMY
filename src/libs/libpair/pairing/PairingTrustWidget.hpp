#ifndef PAIRINGTRUSTWIDGET_HPP
#define PAIRINGTRUSTWIDGET_HPP

#include "uptime/ConfigureHelper.hpp"
#include "TrustLevel.hpp"

#include <QWidget>
#include <QTimer>

class Node;
class QAbstractButton;
class Associate;

namespace Ui
{
class PairingTrustWidget;
}


class PairingTrustWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::PairingTrustWidget *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;
	QString mCurrentID;

public:
	explicit PairingTrustWidget(QWidget *parent = nullptr);
	virtual ~PairingTrustWidget() override;


public:
	void configure(QSharedPointer<Node> node);

public:
	void startEdit(QSharedPointer<Associate> peer);
	TrustLevel selectedTrustLevel() const;
	void selectTrustLevel(TrustLevel level);

private slots:
	void onTrustButtonClicked(QAbstractButton *);

private slots:
	void on_pushButtonSaveEdits_clicked();
	void on_pushButtonRemove_clicked();

signals:
	void editComplete(QString id, TrustLevel level, bool save);
	void remove(QString id);

};

#endif
// PAIRINGTRUSTWIDGET_HPP
