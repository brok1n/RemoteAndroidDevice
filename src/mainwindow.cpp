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
    , mHeadData(new QByteArray)
    , mFrameSize(0)
    , mFrameIndex(0)
    , mFrameData(new QByteArray)
    , mPicIndex(0)
{
    ui->setupUi(this);
    mSocket = new QTcpSocket(this);
    mSocket->setReadBufferSize(6220800);

    connect(mSocket, &QTcpSocket::readyRead, this, &MainWindow::readyRead);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_startBtn_clicked()
{
    mSocket->connectToHost("127.0.0.1", 1313);
}

void MainWindow::readyRead()
{
    QByteArray bt  = mSocket->readAll();
    qDebug("recv data:%d", bt.size());
    for (int cursor = 0, len = bt.size(); cursor < len;) {
            if (mReadBannerBytes < mHeadSize) {
              switch (mReadBannerBytes) {
              case 0:
                    // version
                    mVersion = bt.at(cursor);
                  break;
              case 1:
                // length
                mHeadSize = mHeadSize = bt.at(cursor);
                break;
              case 2:
              case 3:
              case 4:
              case 5:
                // pid
                mMiniCapPid += (bt.at(cursor) << ((mReadBannerBytes - 2) * 8)) & 0x80;
                  break;
              case 6:
              case 7:
              case 8:
              case 9:
                // real width
                mRealDisplayWidth +=
                  (bt.at(cursor) << ((mReadBannerBytes - 6) * 8)) & 0x80;
                  break;
              case 10:
              case 11:
              case 12:
              case 13:
                // real height
                mRealDisplayHeight +=
                  (bt.at(cursor) << ((mReadBannerBytes - 10) * 8)) & 0x80;
                break;
              case 14:
              case 15:
              case 16:
              case 17:
                // virtual width
                mVirtualDisplayWidth +=
                  (bt.at(cursor) << ((mReadBannerBytes - 14) * 8)) & 0x80;
                  break;
              case 18:
              case 19:
              case 20:
              case 21:
                // virtual height
                mVirtualDisplayHeight +=
                  (bt.at(cursor) << ((mReadBannerBytes - 18) * 8)) & 0x80;
                  break;
              case 22:
                // orientation
                mDisplayOrientation += bt.at(cursor) * 90;
                  break;
              case 23:
                // quirks
                mQuirkBitFlags = bt.at(cursor);
                  break;
              }

              cursor += 1;
              mReadBannerBytes += 1;

              if (mReadBannerBytes == mHeadSize) {
                  qDebug("数据头读取完毕");
                  qDebug("version:%d, headSize:%d, minicapPid:%d, realWidth:%d, realHeight:%d, virtualWidth:%d, virtualHeight:%d, displayOrientation:%d, quirkBitFlags:%d", mVersion, mHeadSize, mMiniCapPid, mRealDisplayWidth, mRealDisplayHeight, mVirtualDisplayWidth, mVirtualDisplayHeight, mDisplayOrientation, mQuirkBitFlags);
              }
            }
            else if (mReadFrameBytes < 4) {
              mFrameSize += (bt.at(cursor) << (mReadFrameBytes * 8));
              cursor += 1;
              mReadFrameBytes += 1;
            }
            else {
                mFrameSize &= 0x80;
                if (len - cursor >= mFrameSize) {

                    bt.remove(0, cursor);
                    mFrameData->append(bt.data(), mFrameSize);

                    QFile f(QString("img_%1.png").arg(mPicIndex));
                    f.open(QIODevice::WriteOnly);
                    f.write(mFrameData->data());
                    f.flush();
                    f.close();
                    mPicIndex ++;

                    cursor += mFrameSize;
                    mFrameSize = mReadFrameBytes = 0;
                    mFrameData->clear();
                  }
                  else {

                    bt.remove(0, cursor);
                    mFrameData->append(bt.data(), len);

                    mFrameSize -= len - cursor;
                    mReadFrameBytes += len - cursor;
                    cursor = len;
                  }
            }
          }




//    if(mHeadData->size() == 0)
//    {
//        //第一个数据包 需要先读头
//        mHeadData->append(bt);
////        if(mHeadData->size() > 1)
////        {
////            mHeadSize = mHeadData->at(1);
////            qDebug("head size:%d", mHeadSize);
////        }
//    }
//    else if(mHeadSize == 0)
//    {
//        //数据头刚刚读取完毕
//        mVersion = mHeadData->at(0);
//        mHeadSize = mHeadData->at(1);
//        mMiniCapPid = (mHeadData->at(2) << 24) + (mHeadData->at(3) << 16) + (mHeadData->at(4) << 8) + mHeadData->at(5);
//        mRealDisplayWidth = (mHeadData->at(6) << 24) + (mHeadData->at(7) << 16) + (mHeadData->at(8) << 8) + mHeadData->at(9);
//        mRealDisplayHeight = (mHeadData->at(10) << 24) + (mHeadData->at(11) << 16) + (mHeadData->at(12) << 8) + mHeadData->at(13);
//        mVirtualDisplayWidth = (mHeadData->at(14) << 24) + (mHeadData->at(15) << 16) + (mHeadData->at(16) << 8) + mHeadData->at(17);
//        mVirtualDisplayHeight = (mHeadData->at(18) << 24) + (mHeadData->at(19) << 16) + (mHeadData->at(20) << 8) + mHeadData->at(21);
//        mDisplayOrientation = mHeadData->at(22);
//        mQuirkBitFlags = mHeadData->at(23);
//        qDebug("数据头读取完毕");
//        qDebug("version:%d, headSize:%d, minicapPid:%d, realWidth:%d, realHeight:%d, virtualWidth:%d, virtualHeight:%d, displayOrientation:%d, quirkBitFlags:%d", mVersion, mHeadSize, mMiniCapPid, mRealDisplayWidth, mRealDisplayHeight, mVirtualDisplayWidth, mVirtualDisplayHeight, mDisplayOrientation, mQuirkBitFlags);
////        qDebug(mHeadData->data());
////        disconnect(mSocket, &QTcpSocket::readyRead, this, &MainWindow::readyRead);
////        mSocket->close();
//    }
//    else if(mFrameData->size() < 4)
//    {
//        //读一个新的图片
//        mFrameSize = bt.at(0) + (bt.at(1) << 8) + (bt.at(2) << 16) + (bt.at(3) << 24 & 0x80);
//        qDebug("new img:%d", mFrameSize);
//        if(bt.size() > 4) {
//            bt.remove(0, 4);
//            mFrameData->append(bt);
//        }
//    }
//    else
//    {
//        //接收一张图片数据里的内容
//        if(mFrameData->size() + bt.size() < mFrameSize) {
//            mFrameData->append(bt);
//        }
//        else if(mFrameData->size() + bt.size() == mFrameSize) {
//            qDebug("一张图片111");
//            mFrameData->append(bt.data());
//            bt.remove(0, mFrameSize - mFrameData->size());
//            QFile f(QString("img_%1.png").arg(mPicIndex));
//            f.open(QIODevice::WriteOnly);
//            f.write(mFrameData->data());
//            f.flush();
//            f.close();
//            mPicIndex ++;
//            mFrameData->clear();
//            mFrameSize = 0;
//        }
//        else
//        {
//            qDebug("一张图片222");
//            mFrameData->append(bt.data(), mFrameSize - mFrameData->size());
//            bt.remove(0, mFrameSize - mFrameData->size());
//            QFile f(QString("img_%1.png").arg(mPicIndex));
//            f.open(QIODevice::WriteOnly);
//            f.write(mFrameData->data());
//            f.flush();
//            f.close();
//            mPicIndex ++;
//            mFrameData->clear();
//            mFrameSize = 0;

////            QImage *img = new QImage((unsigned char*)mFrameData->data(), mVirtualDisplayWidth, mVirtualDisplayHeight, QImage::Format_RGB888);
////            ui->imgLabel->setPixmap(QPixmap::fromImage(&img));

//            //读一个新的图片
//            mFrameSize = bt.at(0) + (bt.at(1) << 8) + (bt.at(2) << 16) + (bt.at(3) << 24 & 0x80);
//            qDebug("new img:%d", mFrameSize);
//            if(bt.size() > 4) {
//                bt.remove(0, 4);
//                mFrameData->append(bt);
//            }
//        }
//    }
//    else if(mFrameSize == mFrameData->size())
//    {
//        //一张图片数据接收完毕
//        qDebug("一张图片");
//        QFile f(QString("img_%1.png").arg(mPicIndex));
//        f.open(QIODevice::WriteOnly);
//        f.write(mFrameData->data());
//        f.flush();
//        f.close();
//        mPicIndex ++;

////        QImage *img = new QImage((unsigned char*)mFrameData->data(), mVirtualDisplayWidth, mVirtualDisplayHeight, QImage::Format_RGB888);
////        ui->imgLabel->setPixmap(QPixmap::fromImage(&img));


//        //读一个新的图片
//        mFrameData->clear();
//        mFrameSize = bt.at(0) + (bt.at(1) << 8) + (bt.at(2) << 16) + (bt.at(3) << 24 & 0x80);
//        qDebug("new img:%d", mFrameSize);
//        if(bt.size() > 4) {
//            bt.remove(0, 4);
//            mFrameData->append(bt);
//        }
//    }

//    QFile f("a.jpg");
//    f.open(QIODevice::WriteOnly);
//    f.write(bt.constData());
//    f.close();
//    disconnect(mSocket, &QTcpSocket::readyRead, this, &MainWindow::readyRead);
//    mSocket->close();
}
