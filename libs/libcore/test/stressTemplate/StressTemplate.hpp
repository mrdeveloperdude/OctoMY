#ifndef STRESSTEMPLATE_HPP
#define STRESSTEMPLATE_HPP

#include <QTest>


class StressTemplate:public QObject{
		Q_OBJECT

	private slots:
		void stress();

};


#endif // STRESSTEMPLATE_HPP
