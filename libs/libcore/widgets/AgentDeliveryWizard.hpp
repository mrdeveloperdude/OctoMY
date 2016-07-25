#ifndef AGENTDELIVERYWIZARD_HPP
#define AGENTDELIVERYWIZARD_HPP

#include "security/KeyStore.hpp"
#include "security/PortableID.hpp"
#include "basic/AgentNameGenerator.hpp"


#include <QWidget>
#include <QTimer>


class WaitingSpinnerWidget;

namespace Ui {
	class AgentDeliveryWizard;
}

class Settings;
class Node;
class AgentDeliveryWizard : public QWidget
{
		Q_OBJECT

	protected:
		Ui::AgentDeliveryWizard *ui;
		QTimer birthTimer;
		WaitingSpinnerWidget *spinner;
		AgentNameGenerator ng;
		Settings *settings;
		Node *node;
		PortableID mID;

	public:

		static const quint64 MINIMUM_BIRTH_TIME;

	public:
		explicit AgentDeliveryWizard(QWidget *parent = 0);
		~AgentDeliveryWizard();

	private:
		void save();
	public:
		void reset();

		void configure(Node *node);

	signals:
		void done(bool);

	private slots:
		void onBirthComplete();

	private slots:
		void on_pushButtonDone_clicked();
		void on_pushButtonPairNow_clicked();
		void on_pushButtonRandomName_clicked();
		void on_pushButtonOnward_clicked();
};

#endif // AGENTDELIVERYWIZARD_HPP
