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

#ifndef PRINTERCONTROLLER_H
#define PRINTERCONTROLLER_H

#include "SynchronizedQueue.h"
#include "gui.h"

#include <QObject>

mk_shared_ptr(PdfPrinter)
mk_shared_ptr(PrintRunner)
mk_shared_constptr(PrinterItem)

class PrinterController : public QObject
{
    Q_OBJECT
public:
    PrinterController(PrinterItemConstPtr printerItem, PdfPrinterPtr printer);
    ~PrinterController();

    void exec();
    void stop();

private:
    PrinterItemConstPtr mPrinterItem;
    PdfPrinterPtr mPrinter;
    PrintRunnerPtr mRunner;
    SynchronizedQueue mQueue;

private slots:
    void lineReceived();

signals:
    void connected();
    void disconnected();
    void waiting();
    void stoppedWaiting();
};

#endif // PRINTERCONTROLLER_H
