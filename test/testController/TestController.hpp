#ifndef TESTCONTROLLER_HPP
#define TESTCONTROLLER_HPP

#include "test/Common.hpp"


class IController;
class QComboBox;
class QFormLayout;
class QLabel;
class QPlainTextEdit;
class QScrollArea;
class QSplitter;
class QStackedWidget;
class QToolBar;
class QVBoxLayout;



class ControllerTester : public QWidget {
	Q_OBJECT
private:
	QVBoxLayout *mMainLayout;
	QToolBar *mToolBar;
	QComboBox *mDropdown;
	QScrollArea *mScrollAreaUI;
	QScrollArea *mScrollAreaForm;
	QLabel *mNoControllerLabel;
	QLabel *mNoConfigLabel;
	QPlainTextEdit *mConfigEdit;
	QFormLayout *mFormLayout;
	QWidget *mFormWidget;
	QStackedWidget *mStackedConfigLabel;
	QSplitter *mSplitter;
	IController *mController{nullptr};
	bool mAllocated{false};

public:
	ControllerTester(QWidget *parent = nullptr);

private slots:
	void toggleDebug(bool wantDebug);
	void toggleConnection(bool wantConnection);
	void toggleAllocation(bool wantAllocate);
	void readConfig();
	void writeConfig();
	void configChanged();


public:
	void updateWidget();
	void updateConfigInfo();
	void updateActuatorInfo();

};



class TestController:public QObject
{
	Q_OBJECT
private slots:
	void test();

};


#endif
// TESTCONTROLLER_HPP
