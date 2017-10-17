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

#ifndef DEVICESPANE_H
#define DEVICESPANE_H

#include "DeviceTypes.h"
#include "DeviceListView.h"
#include "StatusUpdateCollector.h"
#include "DeviceMenuProcessor.h"

#include <QDialog>
#include "ui_Devices.h"
#include <QGridLayout>
#include <QScrollArea>
#include <QLabel>

#include <utility>
#include <vector>
#include <map>

typedef std::pair<int,int> TokenPair;

class VisualizedDeviceEntry;

/*
NOTE: look for BEGIN_DEVICE_CLASS_QUERY in hercules source

note: DEVA=0180 3390 DASD 1001 /media/Data/Hercules/zLinux/CentOS-4.7/180.cckd [3339 cyls]
*/

struct DynDeviceLine
{
    char    DEV[3];
    char    action;
    char    eq;
    char    devNo[4];
    char    pad1;
    char    devTypeN[4];
    char    pad2;
    char    devType[4];
    char    pid3;
    char    statusBytes[4];
};

template <class Key, class T> class dbgmap
{
public:
	dbgmap()
	{
		mMap = new std::map<Key, T>();
	}

	typedef typename std::map<const Key, T>::iterator iterator ;
	typedef typename std::map<Key, T>::const_iterator const_iterator ;

	std::pair< iterator, bool > insert(std::pair< const Key, T> t)
	{
		return mMap->insert(t);
	}

	void clear()
	{
		mMap->clear();
	}

	typename std::map<Key, T>::iterator begin() const
	{
		return mMap->begin();
	}

	iterator end() const
	{
		return mMap->end();
	}

	iterator find(const Key& k)
	{
		return mMap->find(k);
	}

	void erase(iterator p)
	{
		mMap->erase(p);
	}

	size_t size()
	{
		return mMap->size();
	}

	T& operator[] (const Key& k)
	{
		return (*mMap)[k];
	}


private:
	std::map<Key,T> *mMap;
};

class MainWindow;
class DeviceConfigLine;
class DevicesRename;
class DevicesPane : public DeviceMenuProcessor, public StatusUpdateCollector
{
	Q_OBJECT
public:
    DevicesPane(QWidget *parent = 0);
    virtual ~DevicesPane();

	virtual bool	notify(const QByteArray& statusLine);
    void 			clear();

public slots:
    void mousePressed(QModelIndex index);
    void menuRename();
    void menuDelete();
    void doAddDevice(bool);
    void updateDevice(bool);
    void menuProperties();
    void doRename(QString oldDevNum, QString newDevNum);
    void menuInterrupt();
    void menuStatus();
    void menuTraceCCW();
    void rejected();

protected:
    virtual bool 				isRealDev(int lineNumber);
    virtual bool 				canAddSYSG();
    virtual bool 				isConfig() { return false; }
    virtual bool 				realDevice();
    virtual DeviceTypes::Type 	getType(int lineNumber);
    virtual void 				doLoadTape(QString& tapeFileName);
    virtual bool 				hasConfig();
    virtual bool				traced();

private:
    Ui::DevicesClass ui;
    MainWindow 				* mMainWindow;
    QGridLayout 			* mLayout;
    QScrollArea 			* mScrollArea;
    DeviceListView 			* mListView;
    std::vector<TokenPair> 	mTokens;
    std::map<int, VisualizedDeviceEntry> mDevices;
    QStandardItemModel 		* mModel;
    DevicesRename 			* mRenameDlg;
    int 					mOldDevNum;

    QSize 					sizeHint() const;
    void 					clearDevices(QWidget * parent);
    void 					resizeEvent ( QResizeEvent * event );
    void 					adjustSize();
    VisualizedDeviceEntry	* getDeviceEntry();
    QString 				textFromValue(int value) const;
    int                     getNextDev(std::map<int, VisualizedDeviceEntry>::iterator it) const;
    QIcon 					* mYellowIcon;
    QIcon 					* mYellowHighIcon;

signals:
    void restartDevices();
};

#endif // DEVICES_H
