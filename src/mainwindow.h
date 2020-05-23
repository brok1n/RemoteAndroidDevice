#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void on_frame(QPixmap*);

private slots:
    void on_startBtn_clicked();
    void readyRead();
    void onFrame(QPixmap*);

private:
    Ui::MainWindow *ui;
    uchar mVersion;
    uchar mHeadSize;
    quint32 mMiniCapPid;
    quint32 mRealDisplayWidth;
    quint32 mRealDisplayHeight;
    quint32 mVirtualDisplayWidth;
    quint32 mVirtualDisplayHeight;
    uchar mDisplayOrientation;
    uchar mQuirkBitFlags;
    quint32 mReadFrameBytes;
    quint32 mFrameSize;
    QByteArray *mFrameData;
    QTcpSocket *mSocket;
    quint32 mPicIndex;

};
#endif // MAINWINDOW_H
