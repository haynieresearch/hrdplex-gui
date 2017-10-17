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

#include "HetInit.h"
#include "gui.h"
#include "UtilityRunner.h"
#include "Preferences.h"

#include <QFileDialog>
#include <QMessageBox>

#include <csignal>


HetInit::HetInit(QWidget *parent)
    : GenericUtility("hetinit",parent)
{
    ui.setupUi(this);

    connect(ui.runButton, SIGNAL(clicked()), this, SLOT(runClicked()));
    connect(ui.exitButton, SIGNAL(clicked()), this, SLOT(exitClicked()));
    connect(ui.browseFileButton, SIGNAL(clicked()), this, SLOT(browseFileClicked()));
    connect(ui.browseDirButton, SIGNAL(clicked()), this, SLOT(browseDirClicked()));

    mVolser = new VolserSpinBox(ui.groupBox);
    mVolser->setObjectName(QString::fromUtf8("spinBox"));
    mVolser->setGeometry(QRect(20, 40, 91, 22));

    ui.directory->setText(Preferences::getInstance().logsDir().c_str());
    ui.directory->setBackgroundRole(QPalette::Window);
    ui.directory->setToolTip("Use the button on the right to select directory");
}

HetInit::~HetInit()
{

}

void HetInit::runClicked()
{
    if (!runOrStopClicked())
    {
        ui.runButton->setText("Run");
        return;
    }

    if (ui.filename->text().isEmpty())
    {
        QMessageBox::warning(this, "hetinit", "Please specify tape file to create",
                QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    std::vector<std::string> parameters;

    if (ui.compressionCheckbox->isChecked())
        parameters.push_back("-d");
    if (ui.nlCheckbox->isChecked())
        parameters.push_back("-n");
    else
    {
        parameters.push_back("-i");
        if (!ui.owner->text().isEmpty()) parameters.push_back(ui.owner->text().toStdString());
    }

    std::string filename = ui.directory->text().toStdString();
    if (filename.size() > 0)
    	filename += "/";
    filename += ui.filename->text().toStdString();
    parameters.push_back(filename);
    parameters.push_back(mVolser->text().toStdString());

    std::string command = "hetinit";

    execute(command, Preferences::getInstance().hercDir(), parameters);
    ui.runButton->setText("Stop");
}


void HetInit::exitClicked()
{
    deleteLater();
}

void HetInit::browseFileClicked()
{
    QString s = QFileDialog::getOpenFileName(this,"Browse for disk to update",ui.filename->text());
    if (s.size() > 0)
    {
        QFileInfo fi(s);
        ui.directory->setText(fi.absolutePath());
        ui.filename->setText(fi.fileName());
    }
}

void HetInit::browseDirClicked()
{
    QString s = QFileDialog::getExistingDirectory(this,"Browse for disk to update",ui.directory->text());
    if (s.size() > 0) ui.directory->setText(s);
}

void HetInit::finishedSlot()
{
    deleteLater();
}

//////////////// VolserSpinBox //////////////////////////////

VolserSpinBox::VolserSpinBox(QWidget * parent) : QAbstractSpinBox(parent)
{
    connect(this, SIGNAL(editingFinished()), this, SLOT(verify()));
    setText("000000");
    verify();
}

VolserSpinBox::~VolserSpinBox()
{

}

QFlags<QAbstractSpinBox::StepEnabledFlag> VolserSpinBox::stepEnabled () const
{
    return mStepFlag ;
}

QString VolserSpinBox::text() const
{
    return lineEdit()->text();
}

void VolserSpinBox::setText(QString text)
{
    lineEdit()->setText(text);
}

void VolserSpinBox::stepBy ( int steps )
{
    if (verify()) // standard numbering
    {
        int value = text().toUInt() + steps;
        if (value > 999999) return;
        if (value < 0) return;
        QString result = QString::number(value);
        if (result.size() < 6)
            result = QString("000000").left(6-result.size()) + result;
        setText(result);
        verify();
    }
}

bool VolserSpinBox::verify()
{
    bool verified = true;
    QString str = text();
    if (str.size() == 6)
    {
        char num[6];
        strncpy(num,text().toUtf8().data(),6);
        for (int i=0; i<6; i++)
        {
            if (!isdigit(num[i]))
            {
                verified = false;
            }
        }
    }
    else
    {
        verified = false;
    }

    if (verified)
    {
        mStepFlag = QAbstractSpinBox::StepUpEnabled | QAbstractSpinBox::StepDownEnabled;
        if (str == "000000") mStepFlag &= ~QAbstractSpinBox::StepDownEnabled;
        if (str == "999999") mStepFlag &= ~QAbstractSpinBox::StepUpEnabled;
    }
    else
    {
        mStepFlag = QAbstractSpinBox::StepNone;
    }
    return verified;
}
