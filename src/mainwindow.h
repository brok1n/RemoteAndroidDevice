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

private slots:
    void on_startBtn_clicked();
    void readyRead();

private:
    Ui::MainWindow *ui;
    uchar mVersion;
    uchar mHeadSize;
    quint32 mReadBannerBytes;
    quint32 mMiniCapPid;
    quint32 mRealDisplayWidth;
    quint32 mRealDisplayHeight;
    quint32 mVirtualDisplayWidth;
    quint32 mVirtualDisplayHeight;
    uchar mDisplayOrientation;
    uchar mQuirkBitFlags;
    QByteArray *mHeadData;
    quint32 mReadFrameBytes;
    quint32 mFrameSize;
    quint32 mFrameIndex;
    QByteArray *mFrameData;
    QTcpSocket *mSocket;
    quint32 mPicIndex;

};
#endif // MAINWINDOW_H
