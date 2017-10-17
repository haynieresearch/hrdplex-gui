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

#ifndef GENERICUTILITY_H_
#define GENERICUTILITY_H_

#include <QDialog>
#include <QProcess>
#include <string>
#include <vector>

class UtilityExecutor;
class UtilityRunner;

class GenericUtility : public QDialog
{
	Q_OBJECT
public:
	GenericUtility(QString name, QWidget *parent = 0);
	virtual ~GenericUtility();

protected:
	QString mName;
	UtilityExecutor * mExecutor;
	UtilityRunner * mRunner;
	UtilityRunner * mErrorRunner;
	bool mStarted, mFinished, mFinishedOK;
	int mRc;

	bool runOrStopClicked();  // returns false if currently running
    int execute(const std::string & pCommand, const std::string& pPath, std::vector<std::string>& pParameters, const QString &workingDIrectory=QString());
	std::string getFlatCommand(const std::string & command, const std::string& path, std::vector<std::string>& parameters);

protected:
	virtual void finishedSlot() = 0;

private slots:
	void timeout();
	void finished(int result, QProcess::ExitStatus status);
	void errorSlot();
	void runnerError(const QByteArray& line);

signals:
	void error();
	void output(QByteArray line);
};

#endif /* GENERICUTILITY_H_ */
