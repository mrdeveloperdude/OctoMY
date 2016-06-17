#ifndef BLUETOOTHLIST_HPP
#define BLUETOOTHLIST_HPP

#include <QObject>
#include <QString>
class BluetoothList : public QObject
{
		Q_OBJECT
	public:
		explicit BluetoothList(QObject *parent = nullptr);

	public:
		QString toSpecStanzas(QString space="");
	signals:

	public slots:
};

#endif // BLUETOOTHLIST_HPP
