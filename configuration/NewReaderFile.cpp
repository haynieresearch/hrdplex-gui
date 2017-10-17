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

#include "NewReaderFile.h"
#include "Preferences.h"

#include <QFileDialog>


NewReaderFile::NewReaderFile(QWidget * parent)
: QDialog(parent),
mLabel(this),
mFileName(this),
mBrowseButton(this),
mOKButton(this),
mCancelButton(this)
{
    initialize();
	move(parent->x()+50,parent->y()+50);

    connect(&mCancelButton, SIGNAL(pressed()), this, SLOT(doCancel()));
    connect(&mOKButton, SIGNAL(pressed()), this, SLOT(doOK()));
    connect(&mBrowseButton, SIGNAL(pressed()), this, SLOT(doBrowse()));
    this->setVisible(true);
}

NewReaderFile::~NewReaderFile()
{
}

void NewReaderFile::doCancel()
{
    this->deleteLater();
}

void NewReaderFile::doOK()
{
    if (!mFileName.text().isEmpty())
    {
        QString str = mFileName.text();
        emit okPressed(str);
    }
    doCancel();
}

void NewReaderFile::doBrowse()
{
    std::string s = QFileDialog::getOpenFileName(this,
            "Locate reader file(s)",
            Preferences::getInstance().configDir().c_str(),
            tr("All files(*)")).toUtf8().data();
    mFileName.setText(s.c_str());
}

void NewReaderFile::initialize()
{
	if (objectName().isEmpty())
		setObjectName(QString::fromUtf8("NewReaderFile"));

    int x = this->geometry().x();
    int y = this->geometry().x();
    this->setGeometry(x, y, 400, 90);

    mLabel.setText("Enter file name to be used as reader file");
    mLabel.setGeometry(5,5,300,12);

    mFileName.setGeometry(5, 30,300,30);

    mBrowseButton.setGeometry(310,40,30,20);
    mBrowseButton.setText("...");

    mOKButton.setGeometry(5,60,50,30);
    mOKButton.setText("OK");

    mCancelButton.setGeometry(310,60,50,30);
    mCancelButton.setText("Cancel");
}
