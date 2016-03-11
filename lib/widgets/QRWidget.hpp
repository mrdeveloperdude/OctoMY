#ifndef QRWIDGET_HPP
#define QRWIDGET_HPP

#include <QWidget>

class QRWidget : public QWidget{
    Q_OBJECT
private:
    QString data;
public:
    explicit QRWidget(QWidget *parent = 0);
    void setQRData(QString data);

protected:
    void paintEvent(QPaintEvent *);


signals:

public slots:

};

#endif // QRWIDGET_HPP
