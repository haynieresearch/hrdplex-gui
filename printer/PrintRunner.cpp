/*
#**********************************************************
#* CATEGORY    APPLICATIONS
#* GROUP       HRDPLEX-GUI
#* AUTHOR      LANCE HAYNIE <LHAYNIE@HAYNIEMAIL.COM>
#* DATE        2017-10-17
#**********************************************************
#* MODIFICATIONS
#* 2017-10-17 - LHAYNIE - INITIAL VERSION
#**********************************************************

#HRDPLEX-GUI - A GUI Front End to the Hercules Mainframe Emulator
#Copyright (C) 2017  Haynie Research & Development

#This program is free software; you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation; either version 2 of the License, or
#(at your option) any later version.

#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.

#You should have received a copy of the GNU General Public License along
#with this program; if not, write to the Free Software Foundation, Inc.,
#51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include "PrintRunner.h"
#include "PrinterItem.h"

#include <QHostAddress>
#include <QElapsedTimer>
#include <QtGlobal>
#include <algorithm>

PrinterSocket::PrinterSocket() :
    QTcpSocket()
{
    mBuffPos = mBuff;
}

QByteArray PrinterSocket::returnLine(const char *end)
{
    end++;
    QByteArray line(mBuff, end-mBuff);

    memmove(mBuff, end, mBuffPos-end);

    mBuffPos -= (end - mBuff);
    return line;
}

char *PrinterSocket::nextPos()
{
    char * maybeEndF = std::find(mBuff, mBuffPos, '\f');
    char * maybeEndN = std::find(mBuff, mBuffPos, '\n');
    char * maybeEndR = std::find(mBuff, mBuffPos, '\r');

    char * ret = std::min(std::min(maybeEndF, maybeEndN), maybeEndR);
    if (ret < mBuffPos) return ret;

    return NULL;
}

QByteArray PrinterSocket::readLine()
{
    char *pos = nextPos();
    if (pos != NULL)
    {
        return returnLine(pos);
    }

    while (true)
    {
        waitForReadyRead(500);
        if (bytesAvailable() > 0)
        {
            qint64 len = read(mBuffPos, 255);
            if (len < 0) return "";
            mBuffPos += len;
            pos = nextPos();
            if (pos != NULL) return returnLine(pos);
        }
    }
}

qint64 PrinterSocket::bytesAvailable() const
{
    return QTcpSocket::bytesAvailable() + (mBuffPos - mBuff);
}

PrintRunner::PrintRunner(SynchronizedQueue& queue, PrinterItemConstPtr& printerItem, int maxQueueSize) :
    Runner(queue), mQueue(queue), mPrinterItem(printerItem), mMaxQueueSize(maxQueueSize)
{
}

void PrintRunner::readFromSocket()
{
    while((mSocket->state() == QAbstractSocket::ConnectedState && mRunning) || mSocket->bytesAvailable() > 0)
    {
        mSocket->waitForReadyRead(500);
        if (mSocket->bytesAvailable() > 0)
        {
            bool ff = false;
            QByteArray line = mSocket->readLine();
            int len = line.length();
            if ((line[len-1] == '\f') && (len > 1)) // in practice this is not likely to happen as all \f are preceded by \r or \n
            {
                line.data()[len-1] = '\n';
                ff = true;
            }

            if ( (len>0) &&  ((line[len-1] == '\n') || (line[len-1] == '\r')) )
                line.data()[len-1] = '\0';

            if (line.length() != 0)
            {
                hOutDebug(5,":" << line.data());
                if (ff) mQueue.push_back("\f"); // generate eject
#ifdef Q_OS_DARWIN
                else mQueue.push_back((line));
#else
                else mQueue.push_back(std::move(line));
#endif
                emit newData();
                while (mQueue.size() > mMaxQueueSize) // do not flood the queue
                    QThread::msleep(100);
            }
        }
    }
}

void PrintRunner::waitForConnected()
{
    while(mSocket->state() != QAbstractSocket::ConnectedState && mRunning)
    {
        hOutDebug(3,"not connected:" << mSocket->state());
        if (mSocket->state() == QAbstractSocket::UnconnectedState)
        {
            mSocket->connectToHost(QHostAddress(mPrinterItem->mIp), mPrinterItem->mPort);
            mSocket->waitForConnected(1000);
            if(mRunning && mSocket->state() != QAbstractSocket::ConnectedState)
                QThread::msleep(200);
            hOutDebug(3, "error:" << mSocket->errorString().toStdString());
        }
        else
        {
            QThread::sleep(1);
        }
    }
}

void PrintRunner::run()
{
    mSocket = QSharedPointer<PrinterSocket>(new PrinterSocket());
    mRunning = true;

    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
    mSocket->moveToThread(this);
    connect(mSocket.data(), SIGNAL(stateChanged(QAbstractSocket::SocketState)),this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));

    while(mRunning)
    {
        QElapsedTimer timer;
        timer.start();
        emit waiting();
        waitForConnected();

        if (!mRunning) break;

        emit connected();
        hOutDebug(3,"appl connected");

        readFromSocket();
        hOutDebug(3, "state:" << mSocket->state() << " running: " << (mRunning? "y" : "n") );
        emit disconnected();
        if (timer.elapsed() < 1000)
        {
            QThread::msleep(1000);
        }
    }

    emit stoppedWaiting();
    mSocket->close();
}

void PrintRunner::socketStateChanged(QAbstractSocket::SocketState state)
{
    static QAbstractSocket::SocketState prevState = QAbstractSocket::UnconnectedState;
    if (state != prevState)
    {
        if (state == QAbstractSocket::ConnectedState && prevState != QAbstractSocket::ConnectedState)
        {
            hOutDebug(3,"Connected");
            emit connected();
        }
        if (state != QAbstractSocket::ConnectedState && prevState == QAbstractSocket::ConnectedState)
        {
            hOutDebug(3,"Disconnected");
            emit disconnected();
        }
        prevState = state;
    }
    hOutDebug(3, "Socket state: " << state);
}

void PrintRunner::stop()
{
    mRunning = false;
}
