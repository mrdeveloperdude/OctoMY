/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#ifndef FACEWIDGET_HPP
#define FACEWIDGET_HPP

#include "widgets/TryToggle.hpp"
#include "security/PortableID.hpp"

#include <QWidget>



class Agent;
class ISyncParameter;

namespace Ui
{
class FaceWidget;
}

class FaceWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::FaceWidget *ui;
	PortableID mLastPID;
	QSharedPointer<Agent> mAgent;

public:
	explicit FaceWidget(QWidget *parent = nullptr);
	~FaceWidget();

protected:

	QSharedPointer<Agent> agent();



public:
	void updateEyeColor();
	void updateVisibility();
	void appendLog(const QString&);
	void setAgent(QSharedPointer<Agent>);

	void setConnectionState(const TryToggleState s, const bool doEmit=true);
	TryToggleState  connectionState() const ;

	void setHookSignals(QObject &ob, bool hook);
	void setPanic(bool panic);

signals:
	void connectionStateChanged(const TryToggleState last, const TryToggleState current);
	void colorChanged(QColor);
	void panic();

public slots:
	void onSyncParameterChanged(ISyncParameter *);

private slots:
	void on_pushButtonNewColor_clicked();
	void on_pushButtonPanic_toggled(bool checked);
	void on_splitterTop_splitterMoved(int pos, int index);
	void on_splitterBottom_splitterMoved(int pos, int index);
	void on_splitterMiddle_splitterMoved(int pos, int index);

};

#endif // FACEWIDGET_HPP
