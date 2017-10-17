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

#include "StatusRunner.h"
#include "gui.h"
#include "NamedPipe.h"

#include <fcntl.h>

StatusRunner::StatusRunner(SynchronizedQueue& statusQueue, HerculesExecutor * herculesExecutor)
: Runner(statusQueue), mHerculesExecutor(herculesExecutor)
{

}

StatusRunner::~StatusRunner()
{
}

void StatusRunner::run()
{
#ifdef hFramework
	mProcess = mHerculesExecutor->getQProcess();
	connect(mHerculesExecutor->getQProcess(),
			SIGNAL(readyReadStandardError()),
			this,
			SLOT(readStandardError()));
	return;
#else
	QFile& statusFile = NamedPipe::getInstance().getHerculesStatus();
	QByteArray buff;
	buff.resize(512);
	mRunning = true;
    bool lineRead = false;
    int startupCounter = 0;

	while(mRunning)
	{
		int size;
		if ((size = statusFile.readLine(buff.data(),512)) <= 0)
		{
            // we wait up to 2 seconds for data to appear in the log
            if (!lineRead)
            {
                if (startupCounter++ < 200)
                {
                    QThread::msleep(10);
                    continue; // retry reading by returning to main loop
                }
            }
			emit newData();
			break;
		}
        lineRead = true;
		buff[size-1] = '\0';
		mQueue.push_back(buff);
		emit newData();
	}
#endif
}


void StatusRunner::readStandardError()
{
	mProcess->setReadChannel(QProcess::StandardError);
	while (true)
	{
		QByteArray output = mProcess->readLine();
		if (output.length() == 0) break;
		output[output.length()-1]='\0';
		mQueue.push_back(output.data());
		emit newData();
	}
}
