#ifndef AUDIOINPUTPREVIEW_H
#define AUDIOINPUTPREVIEW_H

#include <QWidget>

namespace Ui {
class AudioInputPreview;
}

class AudioInputPreview : public QWidget
{
	Q_OBJECT
private:
	Ui::AudioInputPreview *ui;

public:
	explicit AudioInputPreview(QWidget *parent = nullptr);
	virtual ~AudioInputPreview();
	
public:
	void setAudioInput(QString inputName);
	
};

#endif // AUDIOINPUTPREVIEW_H
