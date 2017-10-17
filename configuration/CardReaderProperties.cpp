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
#include "CardReaderProperties.h"
#include "NewReaderFile.h"

#include <sstream>

CardReaderProperties::CardReaderProperties(ConfigLine& line, QWidget *parent)
    : GenericDeviceProperties(line, parent), mNewReaderFile(NULL)
{
	ui.setupUi(this);
	ReaderFileListView * listView = static_cast<ReaderFileListView *>(ui.fileNamesListView);
	listView->setReaderParent(this);

    initialize(line);
    connect(ui.fileNamesListView, SIGNAL(pressed (const QModelIndex &)), this, SLOT(itemPressed( const QModelIndex &)));
    connect(ui.sockdev, SIGNAL(textChanged(const QString &)), this, SLOT(sockdevChanged()));
    connect(ui.addButton, SIGNAL(pressed()), this, SLOT(addPressed()));
    connect(ui.deleteButton, SIGNAL(pressed()), this, SLOT(deletePressed()));
    connect(ui.OKButton, SIGNAL(pressed()), this, SLOT(ok()));
    connect(ui.cancelButton, SIGNAL(pressed()), this, SLOT(cancel()));
    connect(ui.sockdevCheckBox, SIGNAL(stateChanged(int)), this, SLOT(sockdevChanged()));
}

CardReaderProperties::~CardReaderProperties()
{

}

void CardReaderProperties::ok()
{
    if (!deviceNumberValidator(ui.deviceNumber))
        return;
    std::stringstream newLineBuff;

    newLineBuff << ui.deviceNumber->text().toStdString()
        << " " << ui.deviceCombo->currentText().toStdString();
    if (ui.sockdevCheckBox->isChecked())
    {
        newLineBuff << " " << ui.sockdev->text().toStdString();
    }
    else
    {
        if (mModel!=NULL)
        {
            for (int i=0; i<mModel->rowCount(); i++)
            {
               newLineBuff << " " << mModel->item(i)->text().toStdString();
            }
        }
    }
    if (ui.sockdevCheckBox->isChecked())
    {
        newLineBuff << " sockdev" ;
    }
    else
        {
        if (ui.eofRadioButton->isChecked())
        {
            newLineBuff << " eof";
        }
        if (ui.intrqRadioButton->isChecked())
        {
            newLineBuff << " intrq";
        }
        if (ui.multifileCheckBox->isChecked())
        {
            newLineBuff << " multifile";
        }
    }

    if (ui.truncCheckBox->isChecked())
    {
        newLineBuff << " trunc";
    }
    if (ui.autopadCheckBox->isChecked())
    {
        newLineBuff << " autopad";
    }
    if (ui.ebcdicRadioButton->isChecked())
    {
        newLineBuff << " ebcdic";
    }
    if (ui.asciiRadioButton->isChecked())
    {
        newLineBuff << " ascii";
    }

    newLineBuff << "\n";
    mLine.replaceLine(newLineBuff.str());
    emit updateLine(true);
    this->deleteLater();
}

void CardReaderProperties::cancel()
{
    emit updateLine(false);
    this->deleteLater();
}

void CardReaderProperties::itemPressed(const QModelIndex & index)
{
    mDragCandidate = index.row();
    mDragCount = mModel->rowCount();
    hOutDebug(5, "index pressed:" << mDragCandidate << std::endl);
}
void CardReaderProperties::shouldBeDeleted()
{
    hOutDebug(5, "should be deleted:" << mDragCandidate << std::endl);
    if (mModel->rowCount() > mDragCount)
        mModel->removeRow(mDragCandidate);
    mDragCandidate=-1;
}

void CardReaderProperties::addPressed()
{
	if (mNewReaderFile != NULL)
		mNewReaderFile->close();
    mNewReaderFile = new NewReaderFile(this);
    mNewReaderFile->move(this->geometry().x(),this->geometry().y());
    connect(mNewReaderFile, SIGNAL(okPressed(QString &)), this, SLOT(addFile(QString&)));
    connect(mNewReaderFile, SIGNAL(destroyed(QObject *)), this, SLOT(newFileFinished(QObject *)));
    hOutDebug(5, "add pressed:" << mDragCandidate << std::endl);
}

void CardReaderProperties::deletePressed()
{
    hOutDebug(5, "delete pressed:" <<  std::endl);
    if (ui.fileNamesListView->currentIndex().row() >= 0)
    {
        mModel->removeRow(ui.fileNamesListView->currentIndex().row());
    }
    ui.sockdevCheckBox->setEnabled (mModel->rowCount() == 0);
}

void CardReaderProperties::addFile(QString & file)
{
    hOutDebug(5,"pickupFile:" << file.toStdString() << std::endl);
    QStandardItem *parentItem = mModel->invisibleRootItem();
    parentItem->appendRow(new QStandardItem(file));
    ui.sockdevCheckBox->setEnabled (mModel->rowCount() == 0);
}

void CardReaderProperties::newFileFinished(QObject *)
{
	mNewReaderFile = NULL;
}

void CardReaderProperties::sockdevChanged()
{
    setSockdev(ui.sockdevCheckBox->isChecked());
    ui.sockdevCheckBox->setEnabled (ui.sockdev->text().isEmpty());
}

void CardReaderProperties::setSockdev(bool enabled)
{
    ui.addButton->setVisible(!enabled);
    ui.deleteButton->setVisible(!enabled);
    ui.fileNamesListView->setVisible(!enabled);
    ui.label->setVisible(!enabled);
    ui.multifileCheckBox->setVisible(!enabled);
    ui.eofGroupBox->setVisible(!enabled);

    ui.sockdev->setVisible(enabled);
    ui.sockspecLabel->setVisible(enabled);

}

void CardReaderProperties::initialize(const ConfigLine & line)
{
    if (line.size() > 0)
        ui.deviceNumber->setText(line.getToken(0).c_str());
    std::string dev = line.getToken(1);
    if (dev == "2501")
        ui.deviceCombo->setCurrentIndex(1);
    else if (dev == "3505")
        ui.deviceCombo->setCurrentIndex(2);
    else  ui.deviceCombo->setCurrentIndex(0);

    mModel = new QStandardItemModel(0,3);
    mModel->clear();
    QStandardItem *parentItem = mModel->invisibleRootItem();
	ui.autoDetectRadioButton->setChecked(true); // default
	ui.eofNoneRadioButton->setChecked(true); // default

    int index = line.size()-1;
    if (index >= 2)
    {
        std::string keywords = "sockdev eof intrq multifile ebcdic ascii trunc autopad";
        ui.sockdevCheckBox->setChecked(false);
        while (keywords.find(line.getToken(index)) != std::string::npos)
        {
            if (line.getToken(index) == "sockdev")
            {
                ui.sockdevCheckBox->setChecked(true);
            }
            else if (line.getToken(index) == "eof")
            {
                ui.eofRadioButton->setChecked(true);
            }
            else if (line.getToken(index) == "intrq")
            {
                ui.intrqRadioButton->setChecked(true);
            }
            else if (line.getToken(index) == "multifile")
            {
                ui.multifileCheckBox->setChecked(true);
            }
            else if (line.getToken(index) == "ebcdic")
            {
                ui.ebcdicRadioButton->setChecked(true);
            }
            else if (line.getToken(index) == "ascii")
            {
                ui.asciiRadioButton->setChecked(true);
            }
            else if (line.getToken(index) == "trunc")
            {
                ui.truncCheckBox->setChecked(true);
            }
            else if (line.getToken(index) == "autopad")
            {
                ui.autopadCheckBox->setChecked(true);
            }
            index--;
        }
        if (!ui.sockdevCheckBox->isChecked())
        {
            for (int i=0; i<=index-2; i++)
            {
                parentItem->appendRow(new QStandardItem(line.getToken(i+2).c_str()));
            }
        }
        else
        {
           ui.sockdev->setText(line.getToken(2).c_str());
        }
    }
    sockdevChanged();
    ui.fileNamesListView->setModel(mModel);
    ui.fileNamesListView->setViewMode(QListView::ListMode);
}
