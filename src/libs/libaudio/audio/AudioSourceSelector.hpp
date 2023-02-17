#ifndef AUDIOSOURCESELECTOR_HPP
#define AUDIOSOURCESELECTOR_HPP

#include <QWidget>
#include <QAudioDevice>

namespace Ui {
class AudioSourceSelector;
}

class AudioSourceSelector : public QWidget
{
	Q_OBJECT
private:
	Ui::AudioSourceSelector *ui;
	
public:
	explicit AudioSourceSelector(QWidget *parent = nullptr);
	virtual ~AudioSourceSelector();

public:
	void addSource(QAudioDevice device);
	void clearSources();
	void updateSources(bool showInputs, bool showOutputs);
	
signals:
	void deviceSelected(QAudioDevice device);

private slots:
	void on_pushButtonCancel_clicked();
};

#endif // AUDIOSOURCESELECTOR_HPP
