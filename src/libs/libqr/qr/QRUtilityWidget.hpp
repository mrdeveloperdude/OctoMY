#ifndef QRUTILITYWIDGET_HPP
#define QRUTILITYWIDGET_HPP

#include <QWidget>

class Node;
class ZBarScanner;
namespace Ui {
class QRUtilityWidget;
}

class QRUtilityWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::QRUtilityWidget *ui;
	QSharedPointer<Node> mNode;
	ZBarScanner *mScanner{nullptr};
	
public:
	explicit QRUtilityWidget(QWidget *parent = nullptr);
	~QRUtilityWidget();
	
public:
	void configure(QSharedPointer<Node> node=nullptr);
	
private slots:
	void onQRRedraw();
	void qrTextChanged(const QString &text);
	
};

#endif // QRUTILITYWIDGET_HPP
