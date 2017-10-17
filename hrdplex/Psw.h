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

#ifndef PSW_H_
#define PSW_H_

#include <StatusUpdateCollector.h>

#include <QDialog>
#include <QLabel>
#include <QStatusBar>

class Psw: public StatusUpdateCollector, public QLabel
{
public:
	enum PswMode
	{
		Docked = 0,
		StatusBar
	};
	Psw( QStatusBar * statusBar );
	virtual ~Psw();

	virtual bool notify(const QByteArray& );
	void setFont();
    inline bool isActive() const { return mActive; }
	void setMode(PswMode mode);
    void standby();
    void setDormant();
    using QLabel::setVisible;
    virtual void setVisible(bool visible, bool modern);
    virtual bool visible();

private:
	void setStatusBar();
	// visible - make status bar visible
	// modern - full status (including man/wait/sys) - used in modern panel
    void setStatusVisible(bool visible, bool modern);

	QString  mLine;
	bool mHasStatusBar;
	bool mVisible;
	bool mActive;
	QStatusBar * mStatusBar;
	Psw::PswMode mMode;
	QLabel * mCpu;
	QLabel * mInstCount;
	QLabel * mSys;
	QLabel * mWait;
	QLabel * mMan;
	QFont  * mFontCourier;
};

#endif /* PSW_H_ */
