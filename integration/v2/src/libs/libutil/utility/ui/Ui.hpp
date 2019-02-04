#ifndef UTILITY_UI_HPP
#define UTILITY_UI_HPP

class QLayout;
class QWidget;
class QStackedLayout;
class QStackedWidget;
class QButtonGroup;
class QAbstractButton;
class QString;
class QTableView;
class QComboBox;

class QSplitter;

#include <qnamespace.h>

#include <QSizePolicy>
#include <QWidget>

namespace utility
{

namespace ui
{

QLayout *createStraigtLayout(QWidget *w);
QStackedLayout *createStackedLayout(QWidget *w);
void makeFiller(QWidget *w);
void addIconToLayout(QString name,QLayout &l,int w=-1,int h=-1);
void clearStackedWidget(QStackedWidget *stackedWidget, bool deleteWidgets=false);
void clearWidget(QWidget *par);
void clearLayout(QLayout* layout, bool deleteWidgets = true);
bool checkWidgetInsideLayout(const QWidget* _someWidget);
bool widgetIsInLayout(const QLayout* layout, const QWidget* _someWidget );
void setWidgetSize(QWidget *wg, int x, int y, int w, int h);
void setWidgetSize(QWindow *wg, int x, int y, int w, int h);
void showToolTip(QWidget *widget,QString str);
void clearButtonGroupSelection(QButtonGroup* group);
bool hasSelectionButtonGroup(QButtonGroup* group);
bool isEnabledButtonGroup(QButtonGroup* group);
void toggleButtonGroup(QButtonGroup* group,bool on,bool clearOnOff=true);
void toggleButtonVisible(QButtonGroup* group,bool visible);
QString selectedButtonName(QButtonGroup* group,QString def="");
qint32 selectedButtonIndex(QButtonGroup* group,qint32 def);
void setSelectedButtonIndex(QButtonGroup* group, const qint32 index);
void toggleButtonSelection(QAbstractButton* button, bool on, bool clearOnOff=true);
void fitContent(QTableView &tv);
void placeInScreen(QWidget &w,QPointF gravity=QPointF(0.5,0.5), int pref=-1);
int nonPrimaryScreen();
void sendFakeKeyEvent(QWidget *target, Qt::Key k, QString ks);
void populateComboboxWithLocalAdresses(QComboBox &cb);

qreal moveSplitter(QSplitter &splitter, qreal pos);

template <typename T>
void pack(T *stacked);





template <typename T>
void pack(T *stacked)
{
	stacked->updatesEnabled();
	stacked->setUpdatesEnabled(false);
	for(int i=0; i<stacked->count(); i++) {
		stacked->widget(i)->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	}
	QWidget *widget=stacked->currentWidget();
	widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	widget->resize(widget->minimumSizeHint());
	widget->adjustSize();
	stacked->resize(stacked->minimumSizeHint());
	stacked->adjustSize();
	stacked->setUpdatesEnabled(true);
}




}
}

#endif
// UTILITY_UI_HPP
