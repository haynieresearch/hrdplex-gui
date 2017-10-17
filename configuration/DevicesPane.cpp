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

#include "DevicesPane.h"
#include "DevicesRename.h"
#include "MainWindow.h"
#include "VisualizedDeviceEntry.h"
#include "Environment.h"
#include "gui.h"
#include "GenericDeviceProperties.h"
#include "StringTokenizer.h"

#include <QBitmap>
#include <QString>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdio>

DevicesPane::DevicesPane(QWidget *parent)
  : DeviceMenuProcessor(parent),
    StatusUpdateCollector(),
    mLayout(NULL),
    mListView(NULL),
    mModel(NULL),
    mRenameDlg(NULL),
    mOldDevNum(-1)
{
    ui.setupUi(this);
    std::string iconsPath = Environment::getIconsPath() + "/yellow.png";
    mYellowIcon = new QIcon(iconsPath.c_str());
    iconsPath = Environment::getIconsPath() + "/yellowhigh.png";
    mYellowHighIcon = new QIcon(iconsPath.c_str());
    mMainWindow = dynamic_cast<MainWindow *>(parent);
}

DevicesPane::~DevicesPane()
{
	delete mYellowIcon;
	delete mYellowHighIcon;
	delete mListView;
	mDevices.clear();
}

bool DevicesPane::notify(const QByteArray& statusLine)
{
    hOutDebug(3,"devices notify:" << statusLine.data() << ".");
	const struct DynDeviceLine * line = reinterpret_cast<const DynDeviceLine *>(statusLine.data());
    int devNo;
    VisualizedDeviceEntry deviceEntry;
    bool deviceAdded = false;
    if (strncmp(line->DEV,"DEV",3) == 0)
    {
        DeviceTypes::Type devType = DeviceTypes::Other;
        switch (line->action)
		{
		case('A'):
			if (strncmp(line->devType,"DASD",4)==0)
				devType= DeviceTypes::DASD;
			else if (strncmp(line->devType,"CTC",3)==0)
				devType= DeviceTypes::CTC;
			else if (strncmp(line->devType,"OSA",3)==0)
				devType= DeviceTypes::Comm;
			else if (strncmp(line->devType,"QETH",4)==0)
				devType= DeviceTypes::Comm;
			else if (strncmp(line->devType,"RDR ",4)==0)
				devType= DeviceTypes::CardReader;
			else if (strncmp(line->devType,"PCH ",4)==0)
				devType= DeviceTypes::CardReader;
			else if (strncmp(line->devType,"PRT ",4)==0)
				devType= DeviceTypes::Printer;
			else if (strncmp(line->devType,"DSP ",4)==0)
				devType= DeviceTypes::Terminal;
			else if (strncmp(line->devType,"TAPE",4)==0)
				devType= DeviceTypes::Tape;
			else if (strncmp(line->devType,"CON",3)==0)
				devType= DeviceTypes::Console;

			devNo =  strtol(line->devNo, NULL, 16);
		{
			VisualizedDeviceEntry deviceEntryPtr(devNo, devType, statusLine.data());
            std::pair<int,VisualizedDeviceEntry> toinsert(devNo, deviceEntryPtr);
			mDevices.insert(toinsert);
			hOutDebug(5, "to be added:" << statusLine.data() << " " << (devNo !=0 ? "added" : "NOT ADDED")) ;
			if (devNo !=0) deviceAdded = true; // after disconnect and attach, herculess starts sending DEVA instead of DEVC
		}

			break;
		case('C'):
			devNo = strtol(line->devNo, NULL, 16);
			if (mDevices.find(devNo) != mDevices.end())
			{
				deviceEntry = mDevices[devNo];

				if (deviceEntry.getItem() != NULL)
				{
					if (line->statusBytes[1] == '1')
					{
						deviceEntry.getItem()->setIcon(*mYellowHighIcon);
					}
					else
					{
						deviceEntry.getItem()->setIcon(*mYellowIcon);
					}
				}
			}
			else
			{
                hOutDebug(3,"sending restart");
				mDevices.clear();
				//emit restartDevices();
			}
			break;
		case('D'):
		{
			devNo = strtol(line->devNo, NULL, 16);
			std::map <int,VisualizedDeviceEntry>::iterator it = mDevices.find(devNo);
			if (it != mDevices.end())
			{
				mDevices.erase(it);
				deviceAdded = true;
			}
		}
			break;
		default:
			break;
		}
	}

    if (deviceAdded)
    {
        hOutDebug(3,mDevices.size() << " Devices.");
        if (mModel == NULL) mModel = new QStandardItemModel();
        mModel->clear();
        QStandardItem *parentItem = mModel->invisibleRootItem();
        int currLine = 0;

		std::map<int, VisualizedDeviceEntry>::iterator it;
        for (int curDev=0; curDev<=DeviceTypes::Comm; curDev++ )
        {
            hOutDebug(5,"curDev:" << curDev);
            bool titleDevAdded = false;
            for (it =  mDevices.begin(); it != mDevices.end(); it++)
            {
                VisualizedDeviceEntry& ent = it->second;
                if (ent.getType() == curDev)
                {
                    hOutDebug(5,"ent:" << ent.getDeviceNumber()<<" "<<DeviceTypes::instance().getName(ent.getType()));
                    if (!titleDevAdded)
                    {
                        std::string iconPath = Environment::getIconsPath() +
								DeviceTypes::instance().getName(ent.getType());
						const QIcon& ic = DeviceTypes::instance().getIcon(ent.getType());
						QStandardItem *item = new QStandardItem(ic, DeviceTypes::instance().getName(ent.getType()));
                        item->setEditable(false);
                        parentItem->appendRow(item);
                        currLine++;
                        titleDevAdded=true;
                    }
                    char ss[255];
                    std::string def = ent.getDefinition();
                    size_t pos=def.find('\r');
                    while (pos != std::string::npos)
                    {
                      def.replace(pos,def.size()," ");
                      pos = def.find('\r');
                    }
                    sprintf(ss,"%4.4X %s",ent.getDeviceNumber(), def.substr(25).c_str());
                    if (ent.getIcon() == NULL)
                    {
                        ent.setIcon(mYellowIcon);
                    }
                    ent.setItem(new QStandardItem(*ent.getIcon(), ss));
                    ent.getItem()->setEditable(false);
                    std::string tt = ent.getDefinition().substr(10,10).c_str() ;
                    tt += ent.getDefinition().substr(25).c_str();
                    ent.getItem()->setToolTip(tt.c_str());
                    ent.setLineNumber(currLine++);
                    parentItem->appendRow(ent.getItem());
                }
            }
        }
        if (mListView != NULL)
        {
            mListView->close();
			delete mListView;
            mListView = NULL;
        }
        mListView = new DeviceListView(this);
        mListView->setModel(mModel);
        mListView->setVisible(true);
        adjustSize();
        connect(mListView, SIGNAL(pressed(QModelIndex)),this,SLOT(mousePressed(QModelIndex)));
        connect(mListView, SIGNAL(deviceClick(QMouseEvent *)), this, SLOT(devicesClick(QMouseEvent *)));
        return true;
    }
    else
    {
    	return false;
    }
}

void DevicesPane::mousePressed(const QModelIndex index)
{
    hOutDebug(3,"pressed " << index.row());
    mClickRow = index.row();
}

void DevicesPane::adjustSize()
{
    if (mListView != NULL)
        mListView->setGeometry(0,0,this->size().width()-10,this->size().height()-10);
}

void DevicesPane::resizeEvent ( QResizeEvent * )
{
    adjustSize();
}

QSize DevicesPane::sizeHint() const
{
    return QSize(160, 100);
}

void DevicesPane::clear()
{
    hOutDebug(3,"DevicesPane clear");
	mDevices.clear();
	if (mModel != NULL)
		mModel->clear();
	if (mListView == NULL)
        return;
    adjustSize();
}

bool DevicesPane::isRealDev(int lineNumber)
{
	std::map<int, VisualizedDeviceEntry>::iterator it;
    for (it =  mDevices.begin(); it != mDevices.end(); it++)
    {
        VisualizedDeviceEntry& ent = it->second;
        hOutDebug(5,"compare: " << ent.getLineNumber() << " with " << lineNumber);
        if (ent.getLineNumber() == lineNumber) return true;
    }
    return false;
}

DeviceTypes::Type DevicesPane::getType(int lineNumber)
{
	std::map<int, VisualizedDeviceEntry>::iterator it;
    for (it =  mDevices.begin(); it != mDevices.end(); it++)
    {
        VisualizedDeviceEntry& ent = it->second;
        hOutDebug(5,"compare: " << ent.getLineNumber() << " with " << lineNumber);
        if (ent.getLineNumber() == lineNumber) return ent.getType();
    }
    return DeviceTypes::Other;
}

bool DevicesPane::canAddSYSG()
{
    return false;
}

bool DevicesPane::realDevice()
{
    return (isRealDev(mClickRow));
}

void DevicesPane::doAddDevice(bool)
{
	hOutDebug(2,mCandidateLine->getLine());
	std::string command = mCandidateLine->getLine();
	if (command.length() == 0) return;
	command = "attach " + command ;

	mMainWindow->issueCommand(command);
	mClickRow = -1;
}

void DevicesPane::menuRename()
{
	std::map<int, VisualizedDeviceEntry>::iterator it = mDevices.begin();
	while(it != mDevices.end())
	{
		hOutDebug(2, "rename " << mLastClick << " " <<	it->second.getDeviceNumber()
				<< " " << it->second.getLineNumber());
		if (it->second.getLineNumber() == mLastClick)
		{
			int devno = it->second.getDeviceNumber();
			mRenameDlg = new DevicesRename(devno, getNextDev(it), this);
			connect (mRenameDlg,SIGNAL(accepted(QString, QString)),this, SLOT(doRename(QString, QString)));
			connect (mRenameDlg,SIGNAL(rejected()), this, SLOT(rejected()));
			mRenameDlg->exec();
			break;
		}
		it++;
	}
	mClickRow=-1;
}

void DevicesPane::doRename(QString oldDevNum, QString newDevNum)
{
	hOutDebug(2, "rename " << oldDevNum.toStdString() <<  " " << newDevNum.toStdString());
	mOldDevNum = ConfigurationEditor::parseNum(oldDevNum.toStdString(), 16);
	std::stringstream command;
	command << "define " << oldDevNum.toStdString() << " " << newDevNum.toStdString() ;
	mMainWindow->issueCommand(command.str());
	disconnect(mRenameDlg,0,0,0);
	mRenameDlg->deleteLater();
}

int DevicesPane::getNextDev(std::map<int, VisualizedDeviceEntry>::iterator it) const
{
	int ret = it->second.getDeviceNumber()+1;
	++it;
	while (it != mDevices.end())
	{
		hOutDebug(5,"examining ret=" << ret << " to:" << it->second.getDeviceNumber());
		if (it->second.getDeviceNumber() != ret)
			return ret;
		else
			ret = it->second.getDeviceNumber()+1;
		++it;
	}
	return ret;
}

void DevicesPane::rejected()
{
	hOutDebug(3, "rejected");
	disconnect(mRenameDlg,0,0,0);
	mRenameDlg->deleteLater();
}

void DevicesPane::menuDelete()
{
	VisualizedDeviceEntry * entry = getDeviceEntry();
	if (entry != NULL)
	{
		std::stringstream command;
		command << "detach " << std::hex << entry->getDeviceNumber() ;
		mMainWindow->issueCommand(command.str());
	}
	mClickRow=-1;
	return;
}

void DevicesPane::menuInterrupt()
{
	VisualizedDeviceEntry * entry = getDeviceEntry();
	if (entry != NULL)
	{
		std::stringstream command;
		command << "i " << std::hex << entry->getDeviceNumber() ;
		mMainWindow->issueCommand(command.str());
	}
	mClickRow=-1;
	return;
}

void DevicesPane::menuStatus()
{
	VisualizedDeviceEntry * entry = getDeviceEntry();
	if (entry != NULL)
	{
		std::stringstream command;
		command << "ds " << std::hex << entry->getDeviceNumber() ;
		mMainWindow->issueCommand(command.str());
	}
	mClickRow=-1;
	return;
}

bool DevicesPane::traced()
{
	VisualizedDeviceEntry * entry = getDeviceEntry();
	if (entry != NULL)
	{
		return entry->traced();
	}
	return false;
}


void DevicesPane::menuTraceCCW()
{
	VisualizedDeviceEntry * entry = getDeviceEntry();
	if (entry != NULL)
	{
		std::stringstream command;
		command << "t" << (entry->traced() ? "- " : "+ ") << std::hex << entry->getDeviceNumber() ;
		mMainWindow->issueCommand(command.str());
		entry->setTraced(!entry->traced());
	}
	mClickRow=-1;
	return;
}

bool DevicesPane::hasConfig()
{
	VisualizedDeviceEntry * entry = getDeviceEntry();
	if (entry == NULL) return false;
	QString devNo = textFromValue(entry->getDeviceNumber());
	mCandidateLine = mMainWindow->getConfigurationFile()->getDevice(devNo.toStdString());
	return mCandidateLine!= NULL;
}

void DevicesPane::menuProperties()
{
	hOutDebug(3,"line=" << mCandidateLine->getLine());

	GenericDeviceProperties * mProp = NULL;

    try
    {
        mProp = GenericDeviceProperties::classFactory(*mCandidateLine,
        		this,
        		mCandidateLine->getDeviceType());
    }
    catch (...)
    {
        hOutDebug(2, "exception caught in class factory (add)");
    }

    connect(mProp, SIGNAL(updateLine(bool)), this, SLOT(updateDevice(bool)));
    mProp->setVisible(true);
	mClickRow=-1;
}

void DevicesPane::updateDevice(bool done)
{
	if (!done || mCandidateLine == NULL)
	{
		mClickRow = -1;
		return;
	}
	hOutDebug(5,mCandidateLine->getLine());
	std::string command = mCandidateLine->getLine();
	command = mCandidateLine->getToken(0) + " " + mCandidateLine->getMultiToken(2, 0);
	if (command.length() == 0) return;
	command = "devinit " + command ;

	mMainWindow->issueCommand(command);
	mClickRow = -1;
}

void DevicesPane::doLoadTape(QString& tapeFileName)
{
	VisualizedDeviceEntry * entry = getDeviceEntry();
	if (entry != NULL)
	{
		std::stringstream command;
		command << "devinit " << std::hex << entry->getDeviceNumber() << " " <<tapeFileName.toStdString() ;

		mMainWindow->issueCommand(command.str());
	}
	mClickRow=-1;
	return;
}

VisualizedDeviceEntry* DevicesPane::getDeviceEntry()
{
	hOutDebug(3,"getDeviceEntry - mLastClick=" << mLastClick);
	std::map<int, VisualizedDeviceEntry>::iterator it = mDevices.begin();
	while(it != mDevices.end())
	{
		if (it->second.getLineNumber() == mLastClick)
		{
			return &(it->second);
			break;
		}
		it++;
	}
	return NULL;
}

QString DevicesPane::textFromValue(int value) const
{
	std::stringstream ss;
	ss << std::hex << value;
	char formatted[5];
	if (value <= 0xffff)
		sprintf(formatted,"%4.4X",(value));
	else
		strcpy(formatted,"0000");
	QString ret(formatted);
    hOutDebug(5,"textFromValue:" << value << "='" << ret.toStdString());
	return ret;
}
