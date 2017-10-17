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
#include "TapeProperties.h"
#include "ConfigurationEditor.h"
#include "Preferences.h"

#include <QFileDialog>
#include <QMessageBox>

#include <sstream>

const std::string TapeProperties::types[] = {"3410", "3420", "3422", "3430", "3480", "3490", "3590", "9347", "8809", ""};

TapeProperties::TapeProperties(ConfigLine & line, QWidget *parent)
    : GenericDeviceProperties(line, parent)
{
    ui.setupUi(this);
    //standard(true);//temp
    initialize(line);

    connect(ui.okButton, SIGNAL(clicked()), this, SLOT(ok()));
    connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(ui.fileButton, SIGNAL(clicked()), this, SLOT(fileBrowse()));
}

TapeProperties::~TapeProperties()
{

}

void TapeProperties::ok()
{
    if (!deviceNumberValidator(ui.deviceNumber))
        return;
    if (ui.filename->text().isEmpty())
    {
    	QMessageBox * p = new QMessageBox(this);
		p->setText("Please enter file name");
		p->exec();
		ui.filename->setFocus();
		return;
    }

    std::stringstream newLineBuff;

    newLineBuff << ui.deviceNumber->text().toStdString()
            << " " << ui.tapeCombo->currentText().toStdString();

    newLineBuff << " " << ui.filename->text().toStdString();

	if (ui.noAutomountCheckbox->isChecked())
		newLineBuff << " noautomount";
	if (ui.awsCheckbox->isChecked())
		newLineBuff << " awstape";
	if (ui.readonlycheckbox->isChecked())
		newLineBuff << " RO";
	if (ui.deonirqCheckbox->isChecked())
		newLineBuff << " deonirq=1";
	if (ui.compressionCombobox->currentIndex() > 0)
	{
		newLineBuff << " compress=1";
		newLineBuff << " method=" << ui.compressionCombobox->currentText().toStdString();
		newLineBuff << " level=" << ui.compressionLevelSlider->value();
	}
	if (ui.chunkSizeSpinbox->value() != 65535)
	{
		newLineBuff << " chunksize=" << ui.chunkSizeSpinbox->value();
	}
	if (ui.maxsizeNumSpinbox->value() > 0)
	{
		newLineBuff << " maxsize" ;
		switch (ui.maxsizeUnitsSpinbox->currentIndex())
		{
		case (1):
			newLineBuff << "k=";
			break;
		case (2):
			newLineBuff << "m=";
			break;
		default:
			newLineBuff << "=";
			break;
		}
		newLineBuff << ui.maxsizeNumSpinbox->value();
	}
	if (ui.eotmarginSpinbox->value() > 0)
	{
		newLineBuff << " eotmargin=" << ui.eotmarginSpinbox->value();
	}

    newLineBuff << "\n";
    mLine.replaceLine(newLineBuff.str());
    emit updateLine(true);
    this->deleteLater();
}

void TapeProperties::cancel()
{
    emit updateLine(false);
    this->deleteLater();
}

void TapeProperties::standard(bool set)
{
    if (set)
    {
        scsi(false);
        ui.tapeGroupBox->setTitle("Standard Tape");
    }
    //ui.filenameLabel->setVisible(set);
    //ui.filename->setVisible(set);
    //ui.fileButton->setVisible(set);
    ui.awsCheckbox->setVisible(set);
    ui.readonlycheckbox->setVisible(set);
    ui.deonirqCheckbox->setVisible(set);
    ui.compressionCombobox->setVisible(set);
    ui.compressionLevelSlider->setVisible(set);
    ui.chunkSizeSpinbox->setVisible(set);
    ui.chunkSizeLabel->setVisible(set);
    ui.label_slider_1->setVisible(set);
    ui.label_slider_9->setVisible(set);
    ui.maxsizeNumSpinbox->setVisible(set);
    ui.maxsizeUnitsSpinbox->setVisible(set);
    ui.maxsizeLabel->setVisible(set);
    ui.eotmarginSpinbox->setVisible(set);
    ui.eotmarginLabel->setVisible(set);
}

void TapeProperties::scsi(bool set)
{
    if (set)
    {
        standard(false);
        ui.tapeGroupBox->setTitle("SCSI Tape");
    }
    ui.noergCheckbox->setVisible(set);
    ui.blkid32Checkbox->setVisible(set);
}

void TapeProperties::fileBrowse()
{
    std::string s = QFileDialog::getOpenFileName(this,
                    "Browse For Tape Files",
                    Preferences::getInstance().configDir().c_str(),
                    tr("All files(*)")).toStdString();
    ui.filename->setText(s.c_str());

}

void TapeProperties::initialize(ConfigLine & line)
{
    ui.deviceNumber->setText(line.getToken(0).c_str());
    std::string dev = line.getToken(1);
    int type=0;
    while(types[type].length() > 0)
    {
        if (dev == types[type])
            ui.tapeCombo->setCurrentIndex(type);
        type++;
    }
    standard(true);
    ui.filename->setText(line.getToken(2).c_str());

    for (int i=3; i<line.size(); i++)
    {
        QString token = line.getToken(i).c_str();
        hOutDebug(3,"token=" << token.toStdString());
        if (token.toLower() == "--no-erg")
        {
            ui.noergCheckbox->setChecked(true);
        }
        else if (token.toLower() == "--blkid-32")
        {
            ui.blkid32Checkbox->setChecked(true);
        }
        else if (token.toLower() == "awstape")
        {
            ui.awsCheckbox->setChecked(true);
        }
        if ( (token.toLower() == "compress=1") ||
             (token.toLower() == "idrc=1") )
        {
            if (ui.compressionCombobox->currentIndex() == 0)
                ui.compressionCombobox->setCurrentIndex(1);
        }
        if (token.toLower().toStdString().substr(0,7) == "method=")
        {
            if (token.toLower().toStdString().substr(7,1) == "z")
                ui.compressionCombobox->setCurrentIndex(1);
            else
                ui.compressionCombobox->setCurrentIndex(2);
        }
        if (token.toLower().toStdString().substr(0,6) == "level=")
        {
            int level = ConfigurationEditor::parseNum(token.toStdString().substr(6,1).c_str());
            if (level >= 1 && level <=9)
                ui.compressionLevelSlider->setValue(level);
        }
        if (token.toLower().toStdString().substr(0,10) == "chunksize=")
        {
            int chunksize = ConfigurationEditor::parseNum(token.toStdString().substr(10).c_str());
            if (chunksize >= 4096 && chunksize <=65535)
                ui.chunkSizeSpinbox->setValue(chunksize);

        }
        if (token.toLower().toStdString().substr(0,8) == "maxsize=")
        {
            int maxsize = ConfigurationEditor::parseNum(token.toStdString().substr(8).c_str());
            ui.maxsizeUnitsSpinbox->setCurrentIndex(0);
            ui.maxsizeNumSpinbox->setValue(maxsize);
        }
        if (token.toLower().toStdString().substr(0,9) == "maxsizek=")
        {
            int maxsize = ConfigurationEditor::parseNum(token.toStdString().substr(9).c_str());
            ui.maxsizeUnitsSpinbox->setCurrentIndex(1);
            ui.maxsizeNumSpinbox->setValue(maxsize);
        }
        if (token.toLower().toStdString().substr(0,9) == "maxsizem=")
        {
            int maxsize = ConfigurationEditor::parseNum(token.toStdString().substr(9).c_str());
            ui.maxsizeUnitsSpinbox->setCurrentIndex(2);
            ui.maxsizeNumSpinbox->setValue(maxsize);
        }
        if (token.toLower().toStdString().substr(0,10) == "eotmargin=")
        {
            int eotmargin = ConfigurationEditor::parseNum(token.toStdString().substr(10).c_str());
            ui.eotmarginSpinbox->setValue(eotmargin);
        }
        if ( (token.toLower() == "readonly=1") ||
             (token.toLower() == "ro") ||
             (token.toLower() == "noring") )
        {
            ui.readonlycheckbox->setChecked(true);
        }
        if (token.toLower() == "deonirq=1")
        {
            ui.deonirqCheckbox->setChecked(true);
        }
        if (token.toLower() == "noautomount")
        {
            ui.noAutomountCheckbox->setChecked(true);
        }
    }
}
