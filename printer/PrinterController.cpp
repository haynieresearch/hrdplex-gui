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

#include "PrinterController.h"
#include "PrintRunner.h"
#include "PagePrinter.h"
#include "gui.h"

/*
 * PrinterController ---> runner -+-> PrinterSocket.connect
 *     V   A                V
 *     |   |                |
 *     |   +---newData----- +
 *     |
 *     +-----------> Printer.print
 */

PrinterController::PrinterController(PrinterItemConstPtr printerItem, PdfPrinterPtr printer) :
    mPrinterItem(printerItem), mPrinter(printer), mRunner(NULL)
{
}

PrinterController::~PrinterController()
{

}

void PrinterController::exec()
{
    if (mRunner.isNull())
    {
        mRunner = PrintRunnerPtr(new PrintRunner(mQueue, mPrinterItem, 132*80*2));
        connect(mRunner.data(), SIGNAL(newData()), this, SLOT(lineReceived()));
        connect(mRunner.data(), SIGNAL(connected()), this, SIGNAL(connected()));
        connect(mRunner.data(), SIGNAL(disconnected()), this, SIGNAL(disconnected()));
        connect(mRunner.data(), SIGNAL(waiting()), this, SIGNAL(waiting()));
        connect(mRunner.data(), SIGNAL(stoppedWaiting()), this, SIGNAL(stoppedWaiting()));
    }
    mRunner->start();
    hOutDebug(3,"Print runner started");
}

void PrinterController::stop()
{
    if (!mRunner.isNull()) {
        if (mRunner->isRunning())
        {
            mRunner->stop();
        }
        for (int i=0; i<10 && !mRunner->isFinished(); i++)
            QThread::msleep(50);
        mPrinter->close();
    }
}


void PrinterController::lineReceived()
{
    while(!mQueue.empty())
    {
        mPrinter->print(mQueue.front());
        mQueue.pop_front();
    }
}
