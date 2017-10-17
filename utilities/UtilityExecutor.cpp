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

#include "UtilityExecutor.h"
#include "gui.h"
#include "UtilityRunner.h"

#include <QProcess>
#include <QString>
#include <QStringList>
#include <cerrno>
#include <cstdio>

UtilityExecutor::UtilityExecutor(QObject *parent) :QObject(parent)
{
    mProcess = NULL;
}

UtilityExecutor::~UtilityExecutor()
{
}

//TODO convert all std::string parameters to QString
int UtilityExecutor::run(const std::string & pCommand, const std::string& pPath, std::vector<std::string> pParameters, UtilityRunner * runner,
        UtilityRunner * errorRunner, const QString& workingDirectory)
{
    QString program = pPath.c_str();
    if (pPath.size() > 0) program += "/";
    program += pCommand.c_str();
    QStringList arguments;
    for (unsigned int i=0; i<pParameters.size(); i++)
    {
        arguments << pParameters[i].c_str();
    }
    mProcess = new QProcess();
    if (runner != NULL)
    {
        runner->start();
        connect(mProcess,
                SIGNAL(readyReadStandardOutput()),
                runner,
                SLOT(readStandardOutput()));
    }
    if (errorRunner != NULL)
    {
        errorRunner->start();
        connect(mProcess,
                SIGNAL(readyReadStandardError()),
                errorRunner,
                SLOT(readStandardError()));
    }
    for (int i=0 ; i<arguments.size(); i++)
        hOutDebug(3, " " << arguments.value(i).toStdString());
    if (!workingDirectory.isEmpty())
        mProcess->setWorkingDirectory(workingDirectory);
    hOutDebug(3, mProcess->workingDirectory().toStdString());
    mProcess->start(program,arguments);
    Q_PID pid = mProcess->pid();

    if (pid != 0)
        return 0;
    else
        return -1;
}

bool UtilityExecutor::running()
{
    if (mProcess != NULL)
        return mProcess->state() == QProcess::Running;
    else return false;

}

void UtilityExecutor::terminate()
{
    if (mProcess != NULL)
	{
        //mProcess->close();
		mProcess = NULL;
	}
}
