#ifndef LOBESELECTOR_HPP
#define LOBESELECTOR_HPP

#include <QWidget>

class Agent;


struct MtlobeStanza {
	QString nickName;
	QString fullName;
	QString iconURL;
	
	MtlobeStanza(
		QString nickName
		, QString fullName
		, QString iconURL
		)
		: nickName (nickName)
		, fullName (fullName)
		, iconURL (iconURL)
	{
		
	}
};

struct MtlobeStanzaList: public QList<MtlobeStanza> {
	
};


namespace Ui {
class LobeSelector;
}

class LobeSelector : public QWidget
{
	Q_OBJECT
private:
	Ui::LobeSelector *ui;
	MtlobeStanzaList mMtlobeStanzas;
	QSharedPointer<Agent> mAgent;
	

#ifdef OC_USE_FEATURE_MOTORICS
	MtlobeController mMtlobeController;
	MtlobeManagerWidget *mWidgetMotorLobeManager;
#endif
	
	
public:
	explicit LobeSelector(QWidget *parent = nullptr);
	~LobeSelector();

private:
	void setupMtLobeUi();
	void initMtlobeList();
	void save();
	void select();

public:
	void configure(QSharedPointer<Agent> agent);
	void reset();

signals:
	void done();

private slots:
	void on_comboBoxAddMtlobe_currentIndexChanged(int index);

};

#endif // LOBESELECTOR_HPP
