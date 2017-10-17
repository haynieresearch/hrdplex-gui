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

#ifndef PRINTRUNNER_H
#define PRINTRUNNER_H

#include "Runner.h"
#include "gui.h"

#include <QTcpSocket>

mk_shared_constptr(PrinterItem)

class PrinterSocket : public QTcpSocket
{
public:
    PrinterSocket();
    QByteArray readLine();
    qint64 bytesAvailable() const;
private:
    QByteArray returnLine(const char *end);
    char *nextPos();
    char mBuff[2048];
    char *mBuffPos;
};

class PrintRunner : public Runner
{
    Q_OBJECT
public:
    PrintRunner(SynchronizedQueue& queue, PrinterItemConstPtr& printerItem, int maxQueueSize);

    virtual void run();
    void stop();

private:
    SynchronizedQueue& mQueue;
    PrinterItemConstPtr mPrinterItem;
    static const int MAX_LINE = 2048;
    int mMaxQueueSize;
    QSharedPointer<PrinterSocket> mSocket;
    bool mRunning;
    char mBuff[MAX_LINE];
    int  mCurrBufLen;

    void waitForConnected();
    void readFromSocket();


private slots:
    void socketStateChanged(QAbstractSocket::SocketState state);

signals:
    void waiting();
    void stoppedWaiting();
    void connected();
    void disconnected();
};

#endif // PRINTRUNNER_H
