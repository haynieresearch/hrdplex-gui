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

#include "PrinterDialog.h"
#include "ui_PrinterDialog.h"
#include "PrinterItem.h"
#include "PrinterController.h"
#include "PagePrinter.h"
#include "Preferences.h"
#include "FieldTip.h"
#include "Stationery.h"
#include "Environment.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QCloseEvent>
#include <QMenu>
#include <QMovie>
#include <QDir>
#include <QFileInfo>


PrinterDialog::PrinterDialog(QWidget *parent, const QString &item) :
    IpValidator(parent),
    ui(new Ui::PrinterDialog),
    mPrinterItem(NULL),
    mListening(false)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window);

    QStringList stationeries = Preferences::getInstance().stationeryList();
    for (int i=0; i<stationeries.size(); i++)
        ui->stationeryComboBox->addItem(stationeries[i]);

    QStringList decolations = Preferences::getInstance().decolationList();
    for (int i=0; i<decolations.size() ; i++)
        ui->decolationComboBox->addItem(decolations[i]);

    ui->printerNameComboBox->insertItems(0,Preferences::getInstance().printerList());
    ui->printerNameComboBox->setCurrentIndex(0);

    if (item.length() > 0)
    {
        populate(item);
    }
    else
    {
        QString lastPrinter = Preferences::getInstance().lastPrinterName().c_str();
        if (lastPrinter.length() > 0)
        {
            ui->printerNameComboBox->setCurrentText(lastPrinter);
            populate(lastPrinter);
        }
        else
        {
            ui->printerNameComboBox->setCurrentIndex(0);
            populate(ui->printerNameComboBox->currentText());
        }
    }

    connect(ui->DirButton, SIGNAL(clicked()), this, SLOT(dirClicked()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancelClicked()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteClicked()));
    connect(ui->connectButton, SIGNAL(clicked()), this, SLOT(connectClicked()));
    connect(ui->printerNameComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(adjustPrinter(QString)));
    connect(ui->printerNameComboBox, SIGNAL(editTextChanged(QString)), this, SLOT(printerNameChanged(QString)));

    QString path = Environment::getIconsPath().c_str();
    QIcon trayIcon(path + "/printerw.png");
    mSystemTrayIcon = new QSystemTrayIcon(trayIcon, this);
    mSystrayHintEjected = false;

    QMenu *trayIconMenu = new QMenu(this);
    QAction *restoreAction = new QAction(tr("&Hide"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(hideRestore()));
    trayIconMenu->addAction(restoreAction);
    mSystemTrayIcon->setContextMenu(trayIconMenu);
    connect(mSystemTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(systrayClick(QSystemTrayIcon::ActivationReason)));
    mSystemTrayIcon->setToolTip("Printer is connected");

}

PrinterDialog::~PrinterDialog()
{
    delete ui;
    if (!mController.isNull())
    {
        mController->stop();
    }
}

void PrinterDialog::populate(QString printerName)
{
    PrinterItemPtr printer(Preferences::getInstance().printer(printerName));

    ui->IP->setText(printer->mIp);
    ui->PortNumber->setText(QString::number(printer->mPort));
    ui->stationeryComboBox->setCurrentText(printer->mStationery);
    ui->lpiSpinBox->setValue(printer->mLinesPerInch);
    ui->fontSizeSpinBox->setValue(printer->mFontSize);
    ui->spoolDirectory->setText(printer->mSpoolDir);
    ui->decolationComboBox->setCurrentText(printer->mDecolationName);
}

void PrinterDialog::dirClicked()
{
    QString dir = ui->spoolDirectory->text();
    if (dir.isEmpty()) dir = QDir::homePath()+"/Desktop";
    dir = QFileDialog::getExistingDirectory(this,
                tr("Spool directory"),
                dir,
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir.length() > 0) ui->spoolDirectory->setText(dir);
}

bool PrinterDialog::save(bool quiet)
{
    if (validateIp(ui->IP, false, quiet) == false) return false;

    if ((ui->PortNumber->text().toUInt() > 0xffff) ||
        (ui->PortNumber->text().toUInt() == 0))
    {
        if (!quiet)
        {
            QString msg(ui->PortNumber->text());
            msg += " is an ilegal port number";
            QMessageBox::warning(this, "Illegal port number", msg , QMessageBox::Ok);
        }
        return false;
    }

    if (ui->printerNameComboBox->currentText().length() == 0)
    {
        if (!quiet)
        {
            QMessageBox::warning(this, "printer", "Please enter printer name",
                                 QMessageBox::Ok, QMessageBox::NoButton);
            FieldTip *balloon = new FieldTip(this, true);
            balloon->setOwner(ui->printerNameComboBox);
            balloon->showMessage("Give a name to this printer",2000);
            ui->printerNameComboBox->setFocus();
        }
        return false;
    }
    if (ui->printerNameComboBox->currentText().indexOf("\"") >= 0)
    {
        if (!quiet)
        {
            QMessageBox::warning(this, "printer", "Printer name cannot contain quotes",
                                 QMessageBox::Ok, QMessageBox::NoButton);
            ui->printerNameComboBox->setFocus();
        }
        return false;
    }
    mPrinterItem = PrinterItemPtr(new PrinterItem());
    mPrinterItem->mName = ui->printerNameComboBox->currentText();
    mPrinterItem->mIp = ui->IP->text();
    mPrinterItem->mPort = ui->PortNumber->text().toInt();
    mPrinterItem->mStationery = ui->stationeryComboBox->currentText();
    mPrinterItem->mLinesPerInch = ui->lpiSpinBox->value();
    mPrinterItem->mFontSize = ui->fontSizeSpinBox->value();
    mPrinterItem->mSpoolDir = ui->spoolDirectory->text();
    mPrinterItem->mDecolationName = ui->decolationComboBox->currentText();
    Preferences::getInstance().setPrinter(ui->printerNameComboBox->currentText(), *mPrinterItem);
    return true;
}

void PrinterDialog::deleteClicked()
{
    Preferences::getInstance().deletePrinter(ui->printerNameComboBox->currentText());
    ui->printerNameComboBox->clear();
    ui->printerNameComboBox->insertItems(0,Preferences::getInstance().printerList());
    ui->printerNameComboBox->setCurrentIndex(0);
}

void PrinterDialog::cancelClicked()
{
    stop();
    if (save(true))
    {
        Preferences::getInstance().setLastPrinterName(ui->printerNameComboBox->currentText().toStdString());
    }
    deleteLater();
}

void PrinterDialog::stop()
{
    if (mController != NULL)
        mController->stop();

}

void PrinterDialog::adjustPrinter(QString name)
{
    if (Preferences::getInstance().printerList().indexOf(name) >= 0)
    {
        populate(name);
    }
}

void PrinterDialog::printerNameChanged(QString)
{

}

void PrinterDialog::connectClicked()
{
    if (mListening) // disconnect
    {
        if (!mController.isNull())
        {
            mController->stop();
        }
    }
    else
    {
        if (!testDirectory(ui->spoolDirectory->text())) return;
        if (!save(false)) return ;
        if (mPrinterItem == NULL)
            mPrinterItem = PrinterItemPtr(new PrinterItem());
        mPrinterItem->mName = ui->printerNameComboBox->currentText();
        mPrinterItem->mIp = ui->IP->text();
        mPrinterItem->mPort = ui->PortNumber->text().toInt();
        mPrinterItem->mStationery = ui->stationeryComboBox->currentText();
        mPrinterItem->mLinesPerInch = ui->lpiSpinBox->value();
        mPrinterItem->mFontSize = ui->fontSizeSpinBox->value();
        mPrinterItem->mSpoolDir = ui->spoolDirectory->text();
        mPrinterItem->mDecolationName = ui->decolationComboBox->currentText();

        StationeryPtr stationery(Preferences::getInstance().stationery(mPrinterItem->mStationery));
        DecolationRulesPtr decolation(Preferences::getInstance().decolation(mPrinterItem->mDecolationName));

        mPrinter = PdfPrinterPtr(new PdfPrinter(ui->spoolDirectory->text(), stationery, decolation, mPrinterItem->mLinesPerInch, mPrinterItem->mFontSize));
        if (!mController.isNull())
        {
            mController->stop();
        }
        else
        {
            mController = PrinterControllerPtr(new PrinterController(mPrinterItem, mPrinter));
            connect(mController.data(), SIGNAL(connected()), this, SLOT(connected()));
            connect(mController.data(), SIGNAL(waiting()), this, SLOT(waiting()));
            connect(mController.data(), SIGNAL(stoppedWaiting()), this, SLOT(stoppedWaiting()));
        }
        mController->exec();
    }
}

void PrinterDialog::connected()
{
    ui->connectedLabel->setText("Connected");
    mSystemTrayIcon->setVisible(true);
    setProtected(true);
}

void PrinterDialog::waiting()
{
    ui->connectButton->setText("Disconnect");
    ui->connectedLabel->setText("Waiting...");
    mListening = true;
    setProtected(true);
}

void PrinterDialog::stoppedWaiting()
{
    ui->connectButton->setText("Connect");
    ui->connectedLabel->setText("Disconnected");
    mListening = false;
    setProtected(false);
}

bool PrinterDialog::isConnected()
{
    return (ui->connectedLabel->text().compare("Connected") == 0);
}

void PrinterDialog::closeEvent(QCloseEvent *event)
{
    if (isConnected())
    {
        hideRestore();
        event->ignore();
        return;
    }
    if (!save(true))
    {
        event->ignore();
        return;
    }
    deleteLater();
}

void PrinterDialog::hideRestore()
{
    if (isVisible())
    {
        setVisible(false);
        systrayHint();
        mSystemTrayIcon->contextMenu()->actions().at(0)->setText("Restore");

    }
    else
    {
        setVisible(true);
        mSystemTrayIcon->contextMenu()->actions().at(0)->setText("Hide");
    }
}

void PrinterDialog::systrayClick(QSystemTrayIcon::ActivationReason)
{
    hideRestore();
}

void PrinterDialog::systrayHint()
{
    if (!mSystrayHintEjected)
    {
        mSystemTrayIcon->showMessage("The printer is still connected and operational","Click the Printer icon to restore its dialog",QSystemTrayIcon::Information,5000);
        mSystemTrayIcon->setToolTip("Click on this icon to restore the dialog");
        mSystrayHintEjected = true;
    }
}

void PrinterDialog::setProtected(bool protect)
{
    ui->printerNameComboBox->setEnabled(!protect);
    ui->IP->setEnabled(!protect);
    ui->PortNumber->setEnabled(!protect);
    ui->stationeryComboBox->setEnabled(!protect);
    ui->lpiSpinBox->setEnabled(!protect);
    ui->fontSizeSpinBox->setEnabled(!protect);
    ui->spoolDirectory->setEnabled(!protect);
    ui->decolationComboBox->setEnabled(!protect);
    ui->deleteButton->setEnabled(!protect);
}

bool PrinterDialog::testDirectory(QString dirName)
{
    QDir dir(dirName);
    if (!dir.exists())
    {
        QString msg("Directory \"" + dirName);
        msg += "\" does not exist";
        QMessageBox::warning(this, "non-existing directory", msg , QMessageBox::Ok);
        ui->spoolDirectory->setFocus();
        return false;
    }
    QFileInfo fileInfo(dirName);
    if (!fileInfo.isDir())
    {
        QString msg("\"" + dirName);
        msg += "\" is not a directory";
        QMessageBox::warning(this, "invalid directory", msg , QMessageBox::Ok);
        ui->spoolDirectory->setFocus();
        return false;

    }
    if (!fileInfo.isWritable())
    {
        QString msg("You do not have permission to write into directory \"" + dirName);
        msg += "\"";
        QMessageBox::warning(this, "invalid directory", msg , QMessageBox::Ok);
        ui->spoolDirectory->setFocus();
        return false;

    }
    return true;
}
