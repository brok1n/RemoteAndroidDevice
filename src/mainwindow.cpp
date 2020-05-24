#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mVersion(0)
    , mHeadSize(0)
    , mMiniCapPid(0)
    , mRealDisplayWidth(0)
    , mRealDisplayHeight(0)
    , mVirtualDisplayWidth(0)
    , mVirtualDisplayHeight(0)
    , mDisplayOrientation(0)
    , mQuirkBitFlags(0)
    , mReadFrameBytes(0)
    , mFrameSize(0)
    , mFrameData(new QByteArray)
    , mPicIndex(0)
{
    ui->setupUi(this);
    mSocket = new QTcpSocket(this);

    connect(mSocket, &QTcpSocket::readyRead, this, &MainWindow::readyRead);
    connect(this, SIGNAL(on_frame(QPixmap*)), this, SLOT(onFrame(QPixmap*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_startBtn_clicked()
{
    //adb shell dumpsys window | findstr init=
    mSocket->connectToHost("127.0.0.1", 1313);
}

void MainWindow::readyRead()
{
    QByteArray bt  = mSocket->readAll();
//    qDebug("recv data:%d", bt.size());

    for(int i = 0, len = bt.size(); i < len;)
    {
        if(mQuirkBitFlags==0)
        {
            switch(i) {
                case 0:
                    mVersion = bt.at(i);
                    break;
                case 1:
                    mHeadSize = bt.at(i);
                    break;
                case 2:
                case 3:
                case 4:
                case 5:
                    mMiniCapPid += (unsigned)(bt.at(i) << ((i - 2) * 8)) >> 0;
                    break;
                case 6:
                case 7:
                case 8:
                case 9:
                    mRealDisplayWidth += (unsigned)(bt.at(i) << ((i-6) * 8)) >> 0;
                    break;
                case 10:
                case 11:
                case 12:
                case 13:
                    mRealDisplayHeight += (unsigned)(bt.at(i) << ((i-10) * 8)) >> 0;
                    break;
                case 14:
                case 15:
                case 16:
                case 17:
                    mVirtualDisplayWidth += (unsigned)(bt.at(i) << ((i-14) * 8)) >> 0;
                    break;
                case 18:
                case 19:
                case 20:
                case 21:
                    mVirtualDisplayHeight += (unsigned)(bt.at(i) << ((i-18) * 8)) >> 0;
                    break;
                case 22:
                    mDisplayOrientation = bt.at(i) * 90;
                    break;
                case 23:
                    mQuirkBitFlags = bt.at(i);
                    break;
            }
            i ++;
            if(mQuirkBitFlags != 0)
            {
                qDebug("数据头读取完毕");
                qDebug("version:%d, headSize:%d, minicapPid:%d, realWidth:%d, realHeight:%d, virtualWidth:%d, virtualHeight:%d, displayOrientation:%d, quirkBitFlags:%d", mVersion, mHeadSize, mMiniCapPid, mRealDisplayWidth, mRealDisplayHeight, mVirtualDisplayWidth, mVirtualDisplayHeight, mDisplayOrientation, mQuirkBitFlags);
            }
        }
        else if(mReadFrameBytes < 4)
        {
            mFrameSize += ((unsigned)(bt.at(i) & 0xFF) << (mReadFrameBytes * 8)) >> 0;
            qDebug("bodyfin(len=%d, cursor=%d)", mFrameSize, i);
            mReadFrameBytes ++;
            i ++;
        }
        else
        {
            if(len - i >= mFrameSize)
            {
                bt.remove(0, i);
                mFrameData->append(bt.constData(), mFrameSize);
                int dataSize = mFrameData->size();
//                qDebug("dataSize:%d, frameSize:%d", dataSize, mFrameSize);
//                qDebug("-------------------------------");
//                qDebug("000000000000000");
//                qDebug() << (((unsigned)bt.at(0)) & 0xFF);
//                qDebug() << (bt.at(0) & 0xFF);
//                qDebug("11111111111111");
//                qDebug() << (((unsigned)bt.at(1)) & 0xFF);
//                qDebug() << (bt.at(1) & 0xFF);
//                qDebug("2222222222222222");
//                qDebug() << (((unsigned)bt.at(2)) & 0xFF);
//                qDebug() << (bt.at(2) & 0xFF);
//                qDebug("33333333333333333");

//                qDebug() << (((unsigned)bt.at(3)) & 0xFF);
//                qDebug() << (bt.at(3) & 0xFF);

                if((unsigned char)mFrameData->at(0) != 0xFF || (unsigned char)mFrameData->at(1) != 0xD8)
                {
                    qDebug("frame body does not start with jpg header");
                }

               QPixmap *pixMap = new QPixmap();
               pixMap->loadFromData((uchar*)mFrameData->data(), mFrameData->size());
               emit on_frame(pixMap);


                i += mFrameSize;
                mFrameSize = mReadFrameBytes = 0;
                mFrameData->clear();
            }
            else
            {
                bt.remove(0, i);
                mFrameData->append(bt);

                mFrameSize -= len - i;
                mReadFrameBytes += len - i;
                i = len;
            }
        }

    }
}

void MainWindow::onFrame(QPixmap *img)
{
    const QPixmap *apm = ui->imgLabel->pixmap();
    ui->imgLabel->setPixmap(*img);
    update();
//    delete apm;
//    apm = nullptr;
}
