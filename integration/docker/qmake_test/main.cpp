#include <QApplication>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QMediaPlaylist>



int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QMediaPlayer *player = new QMediaPlayer;
	QMediaPlaylist *playlist = new QMediaPlaylist(player);
	playlist->addMedia(QMediaContent(QUrl::fromLocalFile("/home/lennart/Desktop/vid.mp4")));
	playlist->addMedia(QMediaContent(QUrl::fromLocalFile("/home/lennart/Desktop/vid.mp4")));
	QVideoWidget *videoWidget = new QVideoWidget();
	player->setVideoOutput(videoWidget);
	videoWidget->show();
	playlist->setCurrentIndex(1);
	player->play();
	return a.exec();
}
