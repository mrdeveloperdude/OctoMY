#ifndef AGENTPAIRINGWIZARD_HPP
#define AGENTPAIRINGWIZARD_HPP

#include "comms/PairingManager.hpp"

#include <QWidget>



namespace Ui {
	class AgentPairingWizard;
}

class AgentPairingWizard : public QWidget
{
		Q_OBJECT

	private:
		Ui::AgentPairingWizard *ui;

		PairingManager pm;


	public:
		explicit AgentPairingWizard(QWidget *parent = 0);
		~AgentPairingWizard();

	private:
		void save();
	public:
		void reset();

	protected:

		virtual void showEvent(QShowEvent *);
		virtual void hideEvent(QHideEvent *);

	private slots:
		void on_pushButtonMaybeOnward_clicked();
		void on_pushButtonTryAgain_clicked();
		void on_pushButtonDone_clicked();
		void on_pushButtonYoutubeTutorial_clicked();
		void on_pushButtonCameraPair_clicked();
		void on_pushButtonSkip_clicked();

	signals:
		void done();
};

#endif // AGENTPAIRINGWIZARD_HPP
