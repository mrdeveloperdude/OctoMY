#ifndef AUDIOOUTPUTPREVIEW_HPP
#define AUDIOOUTPUTPREVIEW_HPP

#include <QWidget>


namespace Ui {
class AudioOutputPreview;
}

class AudioOutputPreview : public QWidget
{
	Q_OBJECT
private:
	Ui::AudioOutputPreview *ui;
	QMetaObject::Connection mClickCon;
	QString mAudioDeviceName;
	
public:
	explicit AudioOutputPreview(QWidget *parent = nullptr);
	virtual ~AudioOutputPreview();

public:
	 void setAudioOutput(QString audioOutputName);
	 
private slots:
	 void onTestClicked();
};

#endif // AUDIOOUTPUTPREVIEW_HPP
