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

#include "DasdProperties.h"
#include "Preferences.h"

#include <QFileDialog>
#include <QMessageBox>

#include <sstream>

std::string DasdProperties::ckd[] = {"2305", "2311", "2314", "3330", "3340", "3350",
        "3375", "3380", "3390", "9345", ""};

std::string DasdProperties::fba[] = {"3310", "3370", "9332", "9335", "9336", "0671", ""};

DasdProperties::DasdProperties(ConfigLine& configLine, QWidget *parent)
    : GenericDeviceProperties(configLine, parent)
{
    ui.setupUi(this);

    connect(ui.okButton, SIGNAL(clicked()), this, SLOT(ok()));
    connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(ui.typeCombo, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(typeChanged(const QString &)));
    connect(ui.localRadioButton,SIGNAL(toggled(bool)), this, SLOT(localInetChanged()));
    connect(ui.browseButton, SIGNAL(clicked()), this, SLOT(browseClicked()));
    connect(ui.browseShadowButton, SIGNAL(clicked()), this, SLOT(browseShadowClicked()));

    initialize(configLine);
}

DasdProperties::~DasdProperties()
{

}

void DasdProperties::ok()
{
    if (!deviceNumberValidator(ui.deviceNumber))
        return;
    std::stringstream newLineBuff;

    newLineBuff << ui.deviceNumber->text().toStdString() << " "
                << ui.typeCombo->currentText().toStdString();

    if (ui.localRadioButton->isChecked())
    {
    	if (ui.filename->text().isEmpty())
    	{
    		QMessageBox * p = new QMessageBox(this);
			p->setText("Please enter file name");
			p->exec();
			ui.filename->setFocus();
			return;
    	}
        newLineBuff << " " << ui.filename->text().toStdString();
    }
    else
    {
        if (!validateIp(ui.remotehost, false, false))
    		return;
        newLineBuff << " " << ui.remotehost->text().toStdString() << ":" << ui.portSpinBox->value() << ":" << ui.devnoSpinBox->value();
    }

    if (ui.nosynchioCheckBox->isChecked())
        newLineBuff << " nosyncio";
    if (!ui.shadow->text().isEmpty())
        newLineBuff << " sf=" << ui.shadow->text().toStdString();

    if (!isFba(ui.typeCombo->currentText().toStdString()))
    {
        if (ui.cuComboBox->currentIndex()>0)
            newLineBuff << " cu=" << ui.cuComboBox->currentText().toStdString();
        if (!ui.standardRadioButton->isChecked())
        {
            newLineBuff << (ui.ReadonlyRadioButton->isChecked() ? " readonly" : " fakewrite");
        }
    }
    else
    {
		//TODO
    }

    newLineBuff << "\n";
    mLine.replaceLine(newLineBuff.str());
    emit updateLine(true);
    this->deleteLater();
}

void DasdProperties::cancel()
{
    emit updateLine(false);
    this->deleteLater();
}

void DasdProperties::browseClicked()
{
    std::string s = QFileDialog::getOpenFileName(this,
                "Locate DASD File",
                Preferences::getInstance().configDir().c_str(),
                tr("All files(*)")).toUtf8().data();
        ui.filename->setText(s.c_str());
}

void DasdProperties::browseShadowClicked()
{
    std::string s = QFileDialog::getOpenFileName(this,
                "Locate Shadow DASD File",
                Preferences::getInstance().configDir().c_str(),
                tr("All files(*)")).toUtf8().data();
        ui.shadow->setText(s.c_str());
}

void DasdProperties::typeChanged(const QString& newValue)
{
    if (isFba(newValue.toStdString()))
        setFba(true);
    else
        setCkd(true);
}


void DasdProperties::localInetChanged()
{
    if (ui.localRadioButton->isChecked())
    {
        setLocal(true);
    }
    else
        setInet(true);
}

void DasdProperties::setLocal(bool set)
{
    if (set)
        setInet(!set);
    ui.filenameLabel->setVisible(set);
    ui.filename->setVisible(set);
    ui.browseButton->setVisible(set);
    ui.shadowLabel->setVisible(set);
    ui.shadow->setVisible(set);
    ui.browseShadowButton->setVisible(set);
}

void DasdProperties::setInet(bool set)
{
    if (set)
        setLocal(!set);
    ui.remotehostLabel->setVisible(set);
    ui.remotehost->setVisible(set);
    ui.portLabel->setVisible(set);
    ui.portSpinBox->setVisible(set);
    ui.devnoLabel->setVisible(set);
    ui.devnoSpinBox->setVisible(set);
}

bool DasdProperties::isFba(const std::string& dev)
{
    int i=0;
    while (fba[i] != "")
    {
        if (dev == fba[i])
            return true;
        i++;
    }
    return false;
}

void DasdProperties::setFba(bool set)
{
    if (set) setCkd(false);
}

void DasdProperties::setCkd(bool set)
{
    if (set) setFba(false);
    ui.ioModeGroupbox->setVisible(set);
    ui.cuComboBox->setVisible(set);
    ui.cuLabel->setVisible(set);
}

bool DasdProperties::isInet(const std::string& fileName, stringArray & vec)
{
    int count;
    size_t ppos= -1;
    size_t pos = -2;
    for (count = 0; count < 3 && pos != std::string::npos; count++)
    {
        pos = fileName.find(':',ppos+1);
        if (pos != std::string::npos)
        {
            std::string s = fileName.substr(ppos+1,pos-ppos-1);
            vec[count] = s;
            ppos = pos;
        }
        else  if (count ==2)
            vec[2] = fileName.substr(ppos+1);

    }
    return count==3;
}

void DasdProperties::initialize(ConfigLine& line)
{
    ui.deviceNumber->setText(line.getToken(0).c_str());
    int origType = ui.typeCombo->currentIndex();
    for (int i=0; i<ui.typeCombo->maxCount(); i++)
    {
        ui.typeCombo->setCurrentIndex(i);
        if (ui.typeCombo->currentText().toStdString() == line.getToken(1))
            break;
        else
            ui.typeCombo->setCurrentIndex(-1);
    }
    if (ui.typeCombo->currentIndex() == -1)
        ui.typeCombo->setCurrentIndex(origType);
    std::string vec[3];
    if (isInet(line.getToken(2), vec))
    {
        ui.remotehost->setText(vec[0].c_str());
        ui.portSpinBox->setValue(atoi(vec[1].c_str()));
        ui.devnoSpinBox->setValue(atoi(vec[2].c_str()));
        ui.inetRadioButton->setChecked(true);
    }
    else
    {
        std::string token = line.getToken(2);
        ui.filename->setText(token.c_str());
    }
    for (int i=2; i<line.size(); i++)
    {
        if (line.getToken(i).substr(0,3) == "sf=")
        {
            ui.shadow->setText(line.getToken(i).substr(3).c_str());
        }
        else if (line.getToken(i) == "nosyncio" || line.getToken(i) == "syio")
        {
            ui.nosynchioCheckBox->setChecked(true);
        }
        else if (line.getToken(i) == "nosyncio" || line.getToken(i) == "syio")
        {
            ui.nosynchioCheckBox->setChecked(true);
        }
        else if (line.getToken(i) == "readonly" || line.getToken(i) == "rdonly"|| line.getToken(i) == "ro")
        {
            ui.ReadonlyRadioButton->setChecked(true);
        }
        else if (line.getToken(i) == "fakewrite" || line.getToken(i) == "fakewrt"|| line.getToken(i) == "fw")
        {
            ui.fakeWritesRadioButton->setChecked(true);
        }
        else if (line.getToken(i).substr(0,3) == "cu=")
        {
            std::string cu = line.getToken(i).substr(3);
            for (int ii=0; ii<ui.cuComboBox->maxCount(); ii++)
            {
                ui.cuComboBox->setCurrentIndex(ii);
                if (ui.cuComboBox->currentText().toStdString() == cu)
                    break;
                else
                    ui.cuComboBox->setCurrentIndex(-1);
            }
        }
    }

    localInetChanged();
    typeChanged(ui.typeCombo->currentText());

}
