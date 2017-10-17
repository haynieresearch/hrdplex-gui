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

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "ConfigFile.h"
#include "gui.h"
#include "ConfigurationEditor.h"
#include "DevicesWidget.h"

#include "ui_Configuration.h"

#include <QSyntaxHighlighter>
#include <QDialog>

struct ConfigTableEntry
{
	std::string keyword;
	void (*populator)(Ui::ConfigurationClass *, const ConfigLine *, ConfigurationEditor::Direction dir);
};

class Configuration : public QDialog
{
	Q_OBJECT

public:
	Configuration(ConfigFile* confignFile, bool newConfig = false, QWidget *parent = 0);
	virtual ~Configuration();

	class DoubleDigitSpinBox : public QSpinBox
	{
	public:
		DoubleDigitSpinBox(QWidget * parent = 0, bool appendPlus=false);
	protected:
		virtual int valueFromText(const QString& text) const;
		virtual QString textFromValue(int value) const;
	private:
		bool mAppendPlus;
	};

	class HexSpinBox : public QSpinBox
	{
	public:
		HexSpinBox(QWidget * parent = 0);
		virtual QString textFromValue(int value) const;
	protected:
		virtual int valueFromText(const QString& text) const;
		QValidator::State validate(QString & input, int & pos) const;
	};

protected:
	virtual void resizeEvent ( QResizeEvent * event );

private:
	Ui::ConfigurationClass	ui;
	HexSpinBox				* mLparNum;
	DevicesWidget			* mDevWgt;
	QPlainTextEdit			* mFreeEdit;
	QSyntaxHighlighter      * mSyntaxHighlighter;
	QWidget					* mParent;
	ConfigFile				* mConfigFile;
	bool					mNewConfig;
	bool                    mChanged;    // text changed since last validation
	bool					mTimerSet;	 // validation timer was set

	int						mCurrTab;
	int						mLastSys;	// updated by blockCountChanged()
	static struct ConfigTableEntry mConfigTable[];
	friend                  class ConfigHighlight;

	void initilize();
	void populate(ConfigurationEditor::Direction);
	void handleHighlight(const ConfigLine& configLine, QTextCursor& cursor, QTextCharFormat& format);

signals:
	 void OKSignal();

 private slots:
	 void okPressed();
	 void cancelPressed();
	 void autoMountBrowsePressed();
	 void autoScsiMountChanged();
	 void cpuPrioChanged(int);
	 void devPrioChanged(int);
	 void hercPrioChanged(int);
	 void todPrioChanged(int);
	 void ecpSvmChanged(int);
	 void httpPortChanged(int);
	 void httpRootBrowsePressed();
	 void modPathBrowsePressed();
	 void tabChanged(int);
	 void blockCountChangedSlot();
	 void validateMaxCpu();
	 void validateModels();
};

class ConfigHighlight : public QSyntaxHighlighter
{
	Q_OBJECT
public:
	ConfigHighlight(QWidget * parent);

protected:
	void highlightBlock(const QString &text);

private:
	QDialog  mDummyConfig;
	Ui::ConfigurationClass	mDummyUi;
	QColor mGreen;
	QColor mRed;
	QColor mBlack;
	QColor mBlue;
	QColor mGray;

};

#endif // CONFIGURATION_H
