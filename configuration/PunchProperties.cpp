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

#include "PunchProperties.h"

#include <sstream>
#include <algorithm>

PunchProperties::PunchProperties(ConfigLine& line, QWidget *parent)
    : GenericDeviceProperties(line, parent)
{
    ui.setupUi(this);

    ui.fileName->setValidator(new PunchFileNameValidator(this));

    if (!line.isNew())
        {
            ui.deviceNumber->setText(mLine.getToken(0).c_str());
            ui.punchCombo->setCurrentIndex( 0 );
            if (mLine.size() > 2)
            {
                ui.fileName->setText(mLine.getToken(2).c_str());
                ui.asciiCheckBox->setChecked(false);
                ui.crlfCheckBox->setChecked(false);
                ui.noclearCheckBox->setChecked(false);
                for (int i=3; i< mLine.size(); i++)
                {
                    std::string token =  (mLine.getToken(i));
                    std::transform(token.begin(), token.end(), token.begin(), ::tolower);
                    if (token == "ascii")
                        ui.asciiCheckBox->setChecked(true);
                    if (token == "crlf")
                        ui.crlfCheckBox->setChecked(true);
                    if (token == "noclear")
                        ui.noclearCheckBox->setChecked(true);
                }
            }
            else
            {
                ui.asciiCheckBox->setEnabled(false);
                ui.crlfCheckBox->setEnabled(false);
                ui.noclearCheckBox->setEnabled(false);
            }
        }

        connect(ui.OKButton, SIGNAL(clicked()), this, SLOT(ok()));
        connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

}

PunchProperties::~PunchProperties()
{

}

void PunchProperties::ok()
{
    if (!deviceNumberValidator(ui.deviceNumber))
            return;
    std::stringstream newLineBuff;
    bool fileGiven = (ui.fileName->text().toStdString().length() > 0);

    newLineBuff << ui.deviceNumber->text().toStdString() << " "
            << ui.punchCombo->currentText().toStdString() << " "
            << ui.fileName->text().toStdString()
            << (fileGiven && ui.asciiCheckBox->isChecked() ? " ascii" : " ")
            << (fileGiven && ui.crlfCheckBox->isChecked() ? " crlf" : " ")
            << (fileGiven && ui.noclearCheckBox->isChecked() ? " noclear" : " ")
            << "\n";
    hOutDebug(5, "new line:" << newLineBuff.str());
    mLine.replaceLine(newLineBuff.str());
    emit updateLine(true);
    this->deleteLater();
}

void PunchProperties::cancel()
{
    emit updateLine(false);
    this->deleteLater();
}


void PunchProperties::setBoxesEnabled(bool enabled)
{
    ui.asciiCheckBox->setEnabled(enabled);
    ui.crlfCheckBox->setEnabled(enabled);
    ui.noclearCheckBox->setEnabled(enabled);
}

PunchFileNameValidator::PunchFileNameValidator(QObject * parent)
: QValidator(parent), mParent(dynamic_cast<PunchProperties *>(parent))
{

}

QValidator::State PunchFileNameValidator::validate(QString & v, int & ) const
{
    mParent->setBoxesEnabled(v.length()>0);
    return QValidator::Acceptable;
}
