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

#include "gui.h"
#include "DeviceMenuProcessor.h"
#include "GenericDeviceProperties.h"
#include "NewReaderFile.h"

#include <QMouseEvent>

DeviceMenuProcessor::DeviceMenuProcessor(QWidget *parent) :
	QWidget(parent), mClickRow(-1)
{

}

DeviceMenuProcessor::~DeviceMenuProcessor()
{

}

void DeviceMenuProcessor::devicesClick(QMouseEvent * event)
{
    hOutDebug(5, "devicesClick(" << mClickRow << "): " <<
    (event->button() == Qt::RightButton ? "Right " : " ?" )
    << " at" << event->x());
    mLastClick = mClickRow;

    if ( event->button() == Qt::RightButton )
    {
    	mMenu.clear();

    	QMenu * mMenuAdd;
		mMenuAdd = mMenu.addMenu("Add...");
		mMenuAdd->clear();
		mMenuAdd->addAction("Terminal", this, SLOT(menuAddTerminal()));
		if (canAddSYSG())
			mMenuAdd->addAction("SYSG", this, SLOT(menuAddSYSG()));
		mMenuAdd->addAction("Console", this, SLOT(menuAddConsole()));
		mMenuAdd->addAction("Printer", this, SLOT(menuAddPrinter()));
		mMenuAdd->addAction("Card Reader", this, SLOT(menuAddCardReader()));
		mMenuAdd->addAction("Card Punch", this, SLOT(menuAddCardPunch()));
		mMenuAdd->addAction("Tape Drives", this, SLOT(menuAddTape()));
		mMenuAdd->addAction("CTC", this, SLOT(menuAddCTC()));
		mMenuAdd->addAction("Dasd", this, SLOT(menuAddDasd()));

		if (realDevice())
		{
			mMenu.addAction("Delete", this, SLOT(menuDelete()));
			if (!isConfig())
			{
				mMenu.addAction("Rename", this, SLOT(menuRename()));
				if (hasConfig())
					mMenu.addAction("Re-Initialize...", this, SLOT(menuProperties()));
				mMenu.addAction("Attention Interrupt", this, SLOT(menuInterrupt()));
				mMenu.addAction("Subchannel Status", this, SLOT(menuStatus()));
				if (traced())
					mMenu.addAction(*new QIcon(":/icons/16x16/check.png"),"Trace CCW", this, SLOT(menuTraceCCW()));
				else
					mMenu.addAction("Trace CCW", this, SLOT(menuTraceCCW()));
				if (this->getType(mClickRow) == DeviceTypes::Tape)
				{
					hOutDebug(3, "TAPE");
					mMenu.addSeparator();
					mMenu.addAction("Load Tape", this, SLOT(menuLoad()));
					mMenu.addAction("Unload Tape", this, SLOT(menuUnload()));
				}
				else
				{
					hOutDebug(3, "not tape");
				}
			}
			else
			{
				mMenu.addSeparator();
				mMenu.addAction("Properties...", this, SLOT(menuProperties()));
				mMenu.move(event->globalX(), event->globalY());
			}
		}

        mMenu.move(event->globalX(), event->globalY());
        mMenu.setVisible(true);
        mClickRow = -1;
    }
}

void DeviceMenuProcessor::menuAddTerminal()
{
	menuAddItem(DeviceTypes::Terminal);
}

void DeviceMenuProcessor::menuAddConsole()
{
    menuAddItem(DeviceTypes::Console);
}

void DeviceMenuProcessor::menuAddPrinter()
{
    menuAddItem(DeviceTypes::Printer);
}

void DeviceMenuProcessor::menuAddCardReader()
{
    menuAddItem(DeviceTypes::CardReader);
}

void DeviceMenuProcessor::menuAddCardPunch()
{
    menuAddItem(DeviceTypes::CardPunch);
}

void DeviceMenuProcessor::menuAddTape()
{
    menuAddItem(DeviceTypes::Tape);
}

void DeviceMenuProcessor::menuAddCTC()
{
    menuAddItem(DeviceTypes::CTC);
}

void DeviceMenuProcessor::menuAddDasd()
{
    menuAddItem(DeviceTypes::DASD);
}

void DeviceMenuProcessor::menuAddSYSG()
{
    menuAddItem(DeviceTypes::Sysg);
}

void DeviceMenuProcessor::menuAddItem(int iType)
{
    hOutDebug(5, "add Item" << mClickRow);
    if (iType < 0 || iType > DeviceTypes::Comm) return;
    DeviceTypes::Type type = static_cast<DeviceTypes::Type> (iType);
    mCandidateLine = new DeviceConfigLine("\n");
    mCandidateLine->setNew(true);

    GenericDeviceProperties * mProp = NULL;

    try
    {
        mProp = GenericDeviceProperties::classFactory(*mCandidateLine,this,type);
    }
    catch (...)
    {
        hOutDebug(2,"exception caught in class factory (add)");
    }

    connect(mProp, SIGNAL(updateLine(bool)), this, SLOT(doAddDevice(bool)));
    mProp->setVisible(true);

    mClickRow=-1;
}

void DeviceMenuProcessor::menuLoad()
{
	NewReaderFile * newTapeFile = new NewReaderFile(this);
	connect(newTapeFile, SIGNAL(okPressed(QString&)), this, SLOT(newTapeFileOK(QString&)));
	newTapeFile->exec();
}

void DeviceMenuProcessor::menuUnload()
{
	QString s("*");
	if (!isConfig()) doLoadTape(s);
}


void DeviceMenuProcessor::newTapeFileOK(QString& tapeFileName)
{
	if (!isConfig()) doLoadTape(tapeFileName);
}
