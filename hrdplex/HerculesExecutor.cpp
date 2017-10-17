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

#include "HerculesExecutor.h"
#include "gui.h"
#include "Arguments.h"
#include "NamedPipe.h"

#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cerrno>
#include <iostream>
#ifdef UNISTD_INC
#include <unistd.h>
#endif

int pipeLog[2];
int pipeStatus[2];
int pipeCommand[2];

HerculesExecutor::HerculesExecutor(QMainWindow & mainWindow, int pid)
: mMainWindow(mainWindow), mProcess(NULL), mPid(pid)
{

}

HerculesExecutor::~HerculesExecutor()
{

}

#ifndef hFramework
int HerculesExecutor::run(std::string configName, std::string herculesPath)
{
	int pid;
	int rc =0;

	pid = fork();
	if (pid < 0)
		return -1;
	if (pid == 0)
	{
		std::string hercules = herculesPath;
		if (herculesPath.length() != 0 )
			hercules += "/";
		hercules += "hercules";
        hOutDebug(2,"hercules:" << hercules );

		FILE * fileOut = NamedPipe::getInstance().getHerculesStdout();
		rc = dup2(fileno(fileOut),fileno(stdout));

		FILE * fileErr = NamedPipe::getInstance().getHerculesStderr();
		rc = dup2(fileno(fileErr),fileno(stderr));

		FILE * fileIn = NamedPipe::getInstance().getHerculesStdin();
		if (fileIn == NULL) perror("fifo0");

		rc = dup2(fileno(fileIn),fileno(stdin));
		if (rc != 0) perror("stdin");

		if (herculesPath.length() != 0)
		{
			rc = chdir(herculesPath.c_str());
			if (rc != 0)
			{
				std::cout << "***************************************************************" << "\n"
						<< "hercules could not be started (" <<  rc << ")" << "\n"
		#ifndef Q_WS_MAC
						<< "check that the path specified in Edit/Preferences is correct." << "\n"
		#else
						<< "check that the path specified in HRDPLEX/Preferences is correct." << "\n"
		#endif
						<< "**************************************************************" << "\n";
				fflush(stdout);
				_exit(1);
			}
		}

		if (Arguments::getInstance().resourceFileName().length() > 0)
		{
			std::string resourceFile = "HERCULES_RC=" + Arguments::getInstance().resourceFileName();
			int stat = putenv(const_cast<char *>(resourceFile.c_str()));
			if (stat)
			{
             hOutDebug(0,"failed to define environment variable "<< stat);
			}
		}
		rc = execlp(hercules.c_str(),hercules.c_str(),"-d","-f",configName.c_str(),"EXTERNALGUI",NULL);
        std::cout << "***************************************************************" << "\n"
                << "hercules could not be started (" <<  rc << ")" << "\n"
                << "check that hercules is properly installed and is on the default path " << "\n"
#ifndef Q_WS_MAC
                << "and that the path specified in Edit/Preferences is correct." << "\n"
#else
                << "and that the path specified in HRDPLEX/Preferences is correct." << "\n"
#endif
                << "**************************************************************" << "\n";
        fflush(stdout);
		_exit(1);
	}

	mPid = pid;
	printf("pid=:%d\n",pid);
	return pid;
}

void HerculesExecutor::issueCommand(const char * command)
{
	if (mPid == 0)
		return;

    hOutDebug(2,"issue command:" << command);
	FILE * input = NamedPipe::getInstance().getHerculesCommandsFile();
	if (input)
	{
			fprintf(input,"%s\n", command);
			fflush(input);
			return;
	}
	else
	{
        hOutDebug(3, "input=" << input);
		return;
	}
}

void HerculesExecutor::issueFormattedCommand(const char * format, const char * arg1)
{
	std::vector<char> buffer(strlen(format) + strlen(arg1) + 2);
	sprintf(&buffer[0],format,arg1);
	issueCommand(&buffer[0]);
}

void HerculesExecutor::issueFormattedCommand(const char * format, int arg1)
{
	std::vector<char> buffer(strlen(format) + 64);
	sprintf(&buffer[0],format,arg1);
	issueCommand(&buffer[0]);
}


#else  // hFramework
int HerculesExecutor::run(std::string configName, std::string herculesPath)
{
	mProcess=NULL;
	std::string prog = herculesPath;
	if (prog.size() > 0) prog += "/";
	prog += "hercules";
	QString program = prog.c_str();
	QStringList arguments;
	arguments << "-d" << "-f" << configName.c_str() << "EXTERNALGUI" ;
	mProcess = new QProcess();
	mProcess->start(program,arguments);
	int iter=2;
	#ifndef  Q_WS_WIN
	while(mProcess->state() == QProcess::NotRunning && --iter > 0)
	{
		sleep(1);
	}
	#endif
	if (mProcess->state() != QProcess::Running && mProcess->state() != QProcess::Starting)
		return -1;
	Q_PID pid = mProcess->pid();
	if (pid != 0)
		return 0;
	else
		return -1;
}

void HerculesExecutor::issueCommand(const char * command)
{
	mProcess->write(command);
}

void HerculesExecutor::issueFormattedCommand(const char * format, const char * arg1)
{
	std::string buffer;
	buffer.resize(strlen(format)+strlen(arg1)+2);
	sprintf(&buffer[0],format,arg1);
	mProcess->write(buffer.c_str());
}

void HerculesExecutor::issueFormattedCommand(const char * format, int arg1)
{
	std::string buffer;
	buffer.resize(strlen(format)+64);
	sprintf(&buffer[0],format,arg1);
	mProcess->write(buffer.c_str());
}

bool HerculesExecutor::getLine(char * buff, int max)
{
	bool ready=false;
	while (!ready)
	{
		ready = mProcess->waitForReadyRead(3000);
		if (ready)
		{
            hOutDebug(3,"waiting for log");
			QByteArray output = mProcess->readLine(max);
			int len = output.length();
            hOutDebug(3,"log len=" << len << ":" << output.data());
			if (len > 0)
			{
				strncpy(buff, output.data(), (len>max? max : len));
				buff[len] = '\0';
			}
			else
			{
				ready=false;
			}
		}
	}
	return true;
}

bool HerculesExecutor::getStatusLine(char * buff, int max)
{
	bool ready=false;
	while (!ready)
	{
		ready = mProcess->waitForReadyRead(3000);
		if (ready)
		{
            hOutDebug(3,"waiting for status");
			QByteArray output = mProcess->readAllStandardError();
			int len = output.length();
            hOutDebug(3,"status len=" << len);
			if (len > 0)
			{
				strncpy(buff, output.data(), (len>max? max : len));
				buff[len] = '\0';
			}
			else
			{
				ready=false;
			}
		}
	}
	return true;
}
#endif
