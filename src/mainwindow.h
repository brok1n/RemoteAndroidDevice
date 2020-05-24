#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void runCmd(QString);

signals:
    void on_frame(QPixmap);

private slots:
    void on_startBtn_clicked();
    void readyRead();
    void onFrame(QPixmap);
    void onProcessReadyRead();
    void onProcessErrorReadyRead();

    void on_rightBtn_clicked();

    void on_homeBtn_clicked();

    void on_backBtn_clicked();

    void on_settingBtn_clicked();

    void on_upBtn_clicked();

    void on_downBtn_clicked();

    void on_leftBtn_clicked();

    void on_okBtn_clicked();

    void on_volUpBtn_clicked();

    void on_volDownBtn_clicked();

    void on_num1Btn_clicked();

    void on_num2Btn_clicked();

    void on_num3Btn_clicked();

    void on_num4Btn_clicked();

    void on_num5Btn_clicked();

    void on_num6Btn_clicked();

    void on_num7Btn_clicked();

    void on_num8Btn_clicked();

    void on_num9Btn_clicked();

    void on_num0Btn_clicked();

    void on_numDotBtn_clicked();

    void on_delBtn_clicked();

    void on_sendBtn_clicked();

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
    QProcess *mProcess;

};
#endif // MAINWINDOW_H
