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

#include "LogWidget.h"
#include "Preferences.h"
#include "gui.h"

#include <QTabWidget>
#include <QFile>
#include <QTextBlock>
#include <QTextStream>
#include <QClipboard>
#include <QApplication>

#include <fcntl.h>
#include <time.h>
#ifndef hFramework
#include <sys/time.h>
#endif


#define for_each_log \
	for (int for_loop_i=0; for_loop_i<2; for_loop_i++)
#define current_log mLogs[for_loop_i]

PlainLogWidget::PlainLogWidget(QWidget * parent, const char * suffix)
	:QTextEdit(parent),  mGreen(10,120,10), mYellow(215,201,45), mRed(240,20,20), mBlack(0,0,0), mWhite(250,250,250), mLinesWritten(0)
{
	mSaveLog = Preferences::getInstance().autosaveLog();
	mLogFileLines = Preferences::getInstance().logFileLines();
	QString parm(suffix);
	setLogFileName(parm);
	mIpled = false;
	mDarkBackground = Preferences::getInstance().darkBackground();

    connect(this, SIGNAL(copyAvailable(bool)), this, SLOT(keepSelection(bool)));
    connect(this, SIGNAL(logCopy()), this, SLOT(copy()));
}

void PlainLogWidget::setLogFileName (QString& suffix)
{
	getTimeStamp(true);
	mLogFileName = Preferences::getInstance().logsDir().c_str();
	mLogFileName += "/hercules";
	mLogFileName += mTimeStamp;
	if (suffix.length() != 0)
	{
		mLogFileName += ".";
		mLogFileName += suffix;
	}
	mLogFileName += ".log";
}

void PlainLogWidget::append(const QByteArray & text)
{
	if (Preferences::getInstance().logTimestamp())
	{
		getTimeStamp(false);
	}
	else
		mTimeStamp[0] = '\0';
	QByteArray s = text;
	if (text.data()[0] == '<')
		s = text.mid(24);

	QColor keepC = textColor();

	if (mDarkBackground)
	{
		if (strncmp(text.data(),"HHC",3) == 0)
		{
			if (text[8] == 'I')
				setTextColor(mGreen);
			else if (text.mid(8,1) =="W")
				setTextColor(mYellow);
			else if (text.mid(8,1) =="E")
				setTextColor(mRed);
		}
		else
			setTextColor(mWhite);

	}
	else
	{
		if (strncmp(text.data(),"HHC",3) == 0)
		{
			if (text[8] == 'I')
				setTextColor(mGreen);
			else if (text.mid(8,1) =="W")
				setTextColor(mYellow);
			else if (text.mid(8,1) =="E")
				setTextColor(mRed);
		}
		else
			setTextColor(mBlack);
	}
	int len = s.length()-1;
    if (s[len] == '\n') s[len] = '\0';
	QTextEdit::append(QByteArray(mTimeStamp) + s);
	setTextColor(keepC);
	if (QTextEdit::document()->blockCount()%mLogFileLines == 0)
	{
		writeToFile(Overflow);
	}
}

void PlainLogWidget::getTimeStamp(bool withDate)
{
	struct tm *current;
	time_t now;
	time(&now);
	current = localtime(&now);

	if (withDate)
		strftime(mTimeStamp, 255, "%Y-%m-%d-%H-%M-%S", current);
	else
	{
		strftime(mTimeStamp, 255, "%H:%M:%S", current); //"%m-%d-%y %H:%M:%S"
		memcpy(mTimeStamp+8, " ", 2);
	}
}

QString PlainLogWidget::toPlainText()
{
	return QTextEdit::toPlainText();
}

bool PlainLogWidget::isOSLog()
{
	return false;
}

void PlainLogWidget::writeToFile(WriteType type)
{
	QTextDocument *oldDocument = QTextEdit::document();
	if (mSaveLog || type == MenuCommand)
	{
		QFile file(mLogFileName);

		if (!file.open(QIODevice::Append | QIODevice::Text))
		{
			hOutDebug(3,"Failed to open file " << mLogFileName.toStdString());
			return ;
		}
		QTextStream out(&file);

		QTextBlock block = oldDocument->begin();

		// skip  lines that were already written
		long linecount = 0;
		while(linecount < mLinesWritten && block != oldDocument->end())
		{
			block=block.next();
			linecount++;
		}

		// write new lines
		while(block != oldDocument->end())
		{
			out << block.text().toUtf8().data() << "\n";
			mLinesWritten++;
			block=block.next();
		}
        file.close();
	}
	if (type == Overflow)
	{
		oldDocument->clear();
		QTextDocument *newBlock = new QTextDocument(this);
		QTextEdit::setDocument(newBlock);
		mLinesWritten = 0;
		getTimeStamp(true);
		QString sepLine = "---------------- log was saved at ";
		sepLine += mTimeStamp;
		sepLine += "-------------------------";
		QTextEdit::append(sepLine);
	}
}

void PlainLogWidget::preferencesChanged()
{
	mLogFileLines = Preferences::getInstance().logFileLines(); // might have been updated
    bool prevDarkBackground = mDarkBackground;
	mDarkBackground = Preferences::getInstance().darkBackground();
    if (mDarkBackground != prevDarkBackground)
    {
        QBrush brush(QColor(180,0,0));
        for (QTextBlock tb=QTextEdit::document()->begin();
             tb != QTextEdit::document()->end();
             tb = tb.next())
        {
            tb.blockFormat().setForeground(mGreen);

        }
    }
}

void PlainLogWidget::setIpled(bool ipled)
{
	mIpled = ipled;
}

void PlainLogWidget::clear()
{
    QTextEdit::clear();
}

void PlainLogWidget::keepSelection(bool selected)
{
    hOutDebug(4, "copyAvailable " << (selected?"y":"f"));
    mSelectedTextExists = selected;
}

bool PlainLogWidget::textIsSelected()
{
    return mSelectedTextExists;
}

QString PlainLogWidget::copySelectedText()
{
    if (mSelectedTextExists)
    {
        emit logCopy();
        hOutDebug(4, "clipboard: '" << QApplication::clipboard()->text().toStdString() << "'");
    }
    return QApplication::clipboard()->text();
}

LogWidget::LogWidget(QWidget * parent)
: PlainLogWidget(NULL), cHercIndex(0), cOsIndex(1)
{
	mTabWidget = new QTabWidget(parent);
	mLogs[cHercIndex] = new PlainLogWidget(this, "hercules");
	mLogs[cOsIndex] = new PlainLogWidget(this, "os");
	mTabWidget->addTab(mLogs[cHercIndex], "Hercules");
	mTabWidget->addTab(mLogs[cOsIndex], "OS");

	for_each_log
	{
		current_log->setReadOnly(true);
		current_log->setVisible(true);
	}
	mIpled = false;
}

LogWidget::~LogWidget()
{
}

QTabWidget * LogWidget::tabWidget()
{
	return mTabWidget;
}
void LogWidget::clear()
{
	for_each_log
		current_log->clear();
}

void LogWidget::setReadOnly(bool ro)
{
	for_each_log
		current_log->setReadOnly(ro);
}

void LogWidget::append(const QByteArray & text)
{
	QByteArray s = text;
    hOutDebug(3, s.data());
	if (text.data()[0] == '<')
		s = text.mid(24);
	if (text.startsWith("HHC") || !mIpled)
		mLogs[cHercIndex]->append(s);
	else
		mLogs[cOsIndex]->append(s);
}

void LogWidget::setFont(const QFont & font)
{
	for_each_log
		current_log->setFont(font);
}

void LogWidget::setTextBackgroundColor  (const QColor &color)
{
	for_each_log
		current_log->setTextBackgroundColor(color);
}

void LogWidget::setTextColor  (const QColor &color)
{
	for_each_log
		current_log->setTextColor(color);
}

QString LogWidget::toPlainText()
{
	QString ret;
	for_each_log
		ret += current_log->toPlainText();
	return ret;
}

bool LogWidget::empty()
{
	return mLogs[cHercIndex]->document()->isEmpty() && mLogs[cOsIndex]->document()->isEmpty();
}

bool LogWidget::isOSLog()
{
	return (mTabWidget->currentIndex() == 1);
}

void LogWidget::writeToFile(WriteType writeType)
{
	for_each_log
		current_log->writeToFile(writeType);
}

void LogWidget::preferencesChanged()
{
	for_each_log
		current_log->preferencesChanged();
}

void LogWidget::setIpled(bool ipled)
{
	mIpled = ipled;
}

bool LogWidget::textIsSelected()
{
    return mLogs[mTabWidget->currentIndex()]->textIsSelected();
}

QString LogWidget::copySelectedText()
{
    return mLogs[mTabWidget->currentIndex()]->copySelectedText();
}
