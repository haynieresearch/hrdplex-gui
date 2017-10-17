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

#include "StationeryDialog.h"
#include "ui_StationeryDialog.h"

#include "FieldTip.h"
#include "Environment.h"
#include "PagePrinter.h"
#include "gui.h"
#include "ConfigurationEditor.h"
#include "Preferences.h"

#include <QSpinBox>
#include <QToolTip>
#include <QPainter>
#include <QMessageBox>

#include <sstream>

//TODO: fix paper sizes by qt's sizes and add 1 or 2 fanfold

const char *paperHeight[] =
{
        "11",
        "14",
        "17",
        "11",
        "11",
        "14.875", //Std FanFold
        "46.81",  //A0
        "33.11",  //A1
        "23.39",  //A2
        "16.54",  //A3
        "11.69",  //A4
        "8.27",   //A5
        "0",
};

const char *paperWidth[] =
{
        "8.5",
        "8.5",
        "11",
        "9.5",
        "15",
        "11",   //Std FanFold
        "33.11", //A0
        "23.39", //A1
        "16.54", //A2
        "11.69", //A3
        "8.27",  //A4
        "5.83",  //A5
        "0",
};

const char *paperHeightMm[] =
{
        "279.4",
        "266.7",
        "432",
        "279.4",
        "279.4",
        "377.8",
        "1189", // A0
        "841",  // A1
        "495",  // A2
        "420",  // A3
        "297",  // A4
        "210",  // A5
        "0",
};

const char *paperWidthMm[] =
{
        "215.9",
        "215.9",
        "279",
        "241.3",
        "381",
        "279.4",
        "841",  // A0
        "495",  // A1
        "420",  // A2
        "297",  // A3
        "210",  // A4
        "148",  // A5
        "0",
};

StationeryDialog::StationeryDialog(QWidget *parent, const QString &item) :
    QDialog(parent),
    ui(new Ui::PaperDialog)
{
    ui->setupUi(this);
    unsigned int i;

    QStringList items = Preferences::getInstance().stationeryList();
    if (items.count() > 0) ui->stationeryNameComboBox->insertItems(0,items);

    for (i=0; i<Stationery::paperSizes(); i++)
        ui->paperSizeComboBox->addItem(Stationery::paperSize(i));

    ui->inchesRadioButton->setChecked(true);
    ui->landscapeRadioButton->setChecked(true);
    ui->paperSizeComboBox->setCurrentIndex(0);
    paperSizeChanged(ui->paperSizeComboBox->currentText());
    ui->barsColorComboBox->setCurrentIndex(0);
    ui->decorationCheckBox->setChecked(true);
    ui->customColor->setEnabled(false);

    for (i=0; strcmp(Stationery::mBarsColorNames[i],"Custom") != 0; i++)
    {
        ui->barsColorComboBox->addItem(Stationery::mBarsColorNames[i]);
    }
    ui->barsColorComboBox->addItem("Custom");

    QRegExp validation("^#[A-Fa-f0-9]{6}$");
    QRegExpValidator *validator = new QRegExpValidator(validation);
    ui->customColor->setValidator(validator);

    mTopMarginSpinBox = new InchesSpinBox(ui->paperTab);
    mTopMarginSpinBox->setObjectName(QStringLiteral("topMarginSpinBox"));
    mTopMarginSpinBox->setGeometry(QRect(140, 350, 120, 31));
    mTopMarginSpinBox->setSingleStep(1);

    QString icon1 = Environment::getIconsPath().c_str();
    icon1 += "/landscape.png";
    QPixmap * pixmap1 = new QPixmap(icon1);
    ui->orientationLabel->setPixmap(*pixmap1);

    mPaperPreview = new PaperPreview(this, ui->styleTab);
    mPaperPreview->setGeometry(20,190,450,230);
    repaint();

    if (item.length() > 0)
    {
        ui->stationeryNameComboBox->setCurrentText(item);
    }
    else
    {
        ui->stationeryNameComboBox->setCurrentIndex(0);
    }
    populate();

    connect(ui->stationeryNameComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(stationeryNameChanged(QString)));
    connect(ui->stationeryNameComboBox, SIGNAL(editTextChanged(QString)), this, SLOT(stationeryNameEdited(QString)));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteClicked()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancelClicked()));
    connect(ui->paperSizeComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(paperSizeChanged(QString)));
    connect(ui->inchesRadioButton, SIGNAL(toggled(bool)), this, SLOT(inchesMm(bool)));
    connect(ui->landscapeRadioButton, SIGNAL(toggled(bool)), this, SLOT(landscape(bool)));
    connect(ui->barsColorComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(barsColorChanged(QString)));
    connect(ui->decorationCheckBox, SIGNAL(stateChanged(int)), this, SLOT(decorationToggeled()));
    connect(ui->feedHolesCheckBox, SIGNAL(stateChanged(int)), this, SLOT(feedHolesToggeled()));
    connect(ui->customColor, SIGNAL(textChanged(QString)), this, SLOT(customColorChanged()));

    if (Preferences::getInstance().balloonStationery() && builtinStationery(ui->stationeryNameComboBox->currentText()))
    {
        Preferences::getInstance().setBalloonStationery(true);
        FieldTip *baloon = new FieldTip(this, true);
        baloon->setOwner(ui->stationeryNameComboBox);
        baloon->showMessage("Change this name to define you own stationery", 10000);
    }
}

StationeryDialog::~StationeryDialog()
{
    delete ui;
}

void StationeryDialog::paperSizeChanged(QString value)
{
    if (value.compare("Custom") == 0)
    {
        ui->customSizeH->setEnabled(true);
        ui->customSizeW->setEnabled(true);
        ui->orientationGroupBox->setEnabled(false);

        FieldTip *balloon = new FieldTip(ui->paperTab);
        balloon->setOwner(ui->customSizeH);
        balloon->showMessage("Enter width and height of paper",5000);
    }
    else
    {
        hOutDebug(3,"i " << ui->paperSizeComboBox->currentIndex() << " v " << ui->customSizeW->text().toUtf8().data());
        if (ui->inchesRadioButton->isChecked())
        {
            ui->customSizeW->setText(paperWidth[ui->paperSizeComboBox->currentIndex()]);
            ui->customSizeH->setText(paperHeight[ui->paperSizeComboBox->currentIndex()]);
        }
        else
        {
            ui->customSizeW->setText(paperWidthMm[ui->paperSizeComboBox->currentIndex()]);
            ui->customSizeH->setText(paperHeightMm[ui->paperSizeComboBox->currentIndex()]);
        }
        ui->customSizeH->setEnabled(false);
        ui->customSizeW->setEnabled(false);
        ui->orientationGroupBox->setEnabled(true);
    }
    repaint();
}

void StationeryDialog::barsColorChanged(QString value)
{
    if (value.compare("Custom") == 0)
    {
        ui->customColor->setEnabled(true);
        FieldTip *balloon = new FieldTip(ui->styleTab);
        balloon->setOwner(ui->customColor);
        balloon->showMessage("Enter RGB value in html format (e.g. #f0fff0)",5000);
    }
    else
    {
        ui->customColor->setEnabled(false);
        StationeryPtr stationery = createRecord();
        int rgbSet[3];
        stationery->getBarsColor(rgbSet);
        QString rgb("#");
        char hexBuf[3];
        for (int i=0; i<3 ; i++)
        {
            sprintf(hexBuf,"%2x", rgbSet[i]);
            rgb += hexBuf;
        }
        ui->customColor->setText(rgb);
        if (mPaperPreview != NULL) mPaperPreview->repaint();
    }
    ui->decorationCheckBox->setDisabled( (value.compare("None") == 0) );
    repaint();
}

void StationeryDialog::inchesMm(bool)
{
    if (ui->paperSizeComboBox->currentText().compare("Custom") == 0)
    {

    }
    else
    {
        paperSizeChanged(ui->paperSizeComboBox->currentText());
    }

}

void StationeryDialog::landscape(bool checked)
{
    QString icon1 = Environment::getIconsPath().c_str();
    if (checked)
    {
        icon1 += "/landscape.png";
        QPixmap * pixmap1 = new QPixmap(icon1);
        ui->orientationLabel->setPixmap(*pixmap1);
    }
    else
    {
        icon1 += "/portrait.png";
        QPixmap * pixmap1 = new QPixmap(icon1);
        ui->orientationLabel->setPixmap(*pixmap1);

    }
}

void StationeryDialog::save(QString name)
{
    //TODO - sort(??)
    if (name.length() == 0) return;
    if (name.indexOf("\"") >= 0) //TODO: here is too late (in case combo box was changed
    {
        QMessageBox::warning(this, "stationery", "Stationery name cannot contain quotes",
            QMessageBox::Ok, QMessageBox::NoButton);
        ui->stationeryNameComboBox->setFocus();
        return;
    }
    QSharedPointer<Stationery> st(createRecord());
    Preferences::getInstance().setStationery(name, *st);
}

void StationeryDialog::stationeryNameChanged(QString)
{
    save(mCurrentName);
    mCurrentName = ui->stationeryNameComboBox->currentText();
    populate();
    repaint();
}

void StationeryDialog::stationeryNameEdited(QString name)
{
    setProtected(builtinStationery(name));
}

void StationeryDialog::cancelClicked() //TODO allow exit when there is no name
{
    if (ui->stationeryNameComboBox->currentText().length() == 0)
    {
        QMessageBox::warning(this, "stationery", "Please enter stationery name",
            QMessageBox::Ok, QMessageBox::NoButton);
        FieldTip *balloon = new FieldTip(this, true);
        balloon->setOwner(ui->stationeryNameComboBox);
        balloon->showMessage("Give a name to this stationery",2000);
        ui->stationeryNameComboBox->setFocus();
        return;
    }

    save(ui->stationeryNameComboBox->currentText());
    deleteLater();
}

bool StationeryDialog::builtinStationery(QString stationeryName)
{
    if (stationeryName == "Legacy Green")
        return true;
    else
        return false;
}

void StationeryDialog::deleteClicked()
{
    QString stationeryToDelete = ui->stationeryNameComboBox->currentText();
    mCurrentName = "";
    Preferences::getInstance().deleteStationery(stationeryToDelete);
    ui->stationeryNameComboBox->clear();
    QStringList items = Preferences::getInstance().stationeryList();
    if (items.count() > 0) ui->stationeryNameComboBox->insertItems(0,items);
    else ui->stationeryNameComboBox->clear();
    ui->stationeryNameComboBox->setCurrentIndex(0);
}

StationeryPtr StationeryDialog::createRecord()
{
    QSharedPointer<Stationery> ret(new Stationery(ui->stationeryNameComboBox->currentText(),
                                                  ui->paperSizeComboBox->currentText(),
                                                  ui->customSizeW->text(),
                                                  ui->customSizeH->text(),
                                                  mTopMarginSpinBox->value(),
                                                  ui->mmRadioButton->isChecked(),
                                                  ui->portraitRadioButton->isChecked(),
                                                  ui->barsColorComboBox->currentText(),
                                                  ui->customColor->text(),
                                                  ui->decorationCheckBox->isChecked(),
                                                  ui->feedHolesCheckBox->isChecked()));

    return ret;
}

void StationeryDialog::populate()
{
    QSharedPointer<Stationery> st = QSharedPointer<Stationery>(Preferences::getInstance().stationery(ui->stationeryNameComboBox->currentText()));
    ui->paperSizeComboBox->setCurrentText(st->mPaperSize);
    ui->customSizeW->setText(st->mSize1);
    ui->customSizeH->setText(st->mSize2);
    mTopMarginSpinBox->setValue(st->mTopMargin);
    ui->mmRadioButton->setChecked(st->mMillimeters);
    ui->portraitRadioButton->setChecked(st->mPortrait);
    ui->landscapeRadioButton->setChecked(!st->mPortrait);
    ui->barsColorComboBox->setCurrentText(st->mBarsColor);
    ui->customColor->setText(st->mBarsRGB);
    ui->decorationCheckBox->setChecked(st->mDecorated);
    ui->feedHolesCheckBox->setChecked(st->mTractorFeedHoles);

    if (ui->paperSizeComboBox->currentText().compare("Custom") == 0)
    {
        ui->customSizeH->setEnabled(true);
        ui->customSizeW->setEnabled(true);
    }
    barsColorChanged(ui->barsColorComboBox->currentText());
    paperSizeChanged(st->mPaperSize);
    bool disabled = builtinStationery(ui->stationeryNameComboBox->currentText());
    setProtected(disabled);
    hOutDebug(3,ui->paperSizeComboBox->currentText().toUtf8().data());
    if (ui->paperSizeComboBox->currentText().compare("Custom") == 0)
    {
        ui->orientationGroupBox->setEnabled(false);
    }
}

void StationeryDialog::setProtected(bool disabled)
{
    ui->paperSizeComboBox->setEnabled(!disabled);
    ui->customSizeW->setEnabled(!disabled);
    ui->customSizeH->setEnabled(!disabled);
    mTopMarginSpinBox->setEnabled(!disabled);
    ui->orientationGroupBox->setEnabled(!disabled);
    ui->groupBox_2->setEnabled(!disabled);
    ui->feedHolesCheckBox->setEnabled(!disabled);
    ui->decorationCheckBox->setEnabled(!disabled);
    ui->barsColorComboBox->setEnabled(!disabled);
    ui->deleteButton->setEnabled(!disabled);
}

void StationeryDialog::feedHolesToggeled()
{
    repaint();
}

void StationeryDialog::decorationToggeled()
{
    repaint();
}

void StationeryDialog::customColorChanged()
{
    if (ui->customColor->text().length() != 7)
        return;
    mPaperPreview->setBarsRgb(ConfigurationEditor::parseNum(ui->customColor->text().right(6).toStdString(), 16));
    repaint();
}

//
// InchesSpinBox
//

InchesSpinBox::InchesSpinBox(QWidget *parent) : QSpinBox(parent)
{
    mNumerator = 8;
    setRange(1,32);
    setSingleStep(1);
    setValue(mNumerator);
}

void InchesSpinBox::setValue(int val)
{
    mNumerator = val;
    QSpinBox::setValue(mNumerator);
}

int InchesSpinBox::value()
{
    valueFromText(this->text());
    return mNumerator;
}

int InchesSpinBox::valueFromText(const QString &text) const
{
    int slashPos = text.indexOf('/');
    bool ok;

    int numerator = text.left(slashPos).toInt(&ok);
    if (!ok) return 8;

    int denominator = text.mid(slashPos+1).toInt(&ok);
    hOutDebug(3, "den=" << denominator << " OK=" << (ok? "Y" : "N"));
    if (!ok) return 8;

    if (denominator%2) return mNumerator;
    if (denominator == 2) mNumerator = numerator * 8;
    else if (denominator == 4) mNumerator = numerator * 4;
    else if (denominator == 8) mNumerator = numerator * 2;
    else mNumerator = numerator;
    return mNumerator;
}

QString InchesSpinBox::textFromValue(int val) const
{
    std::stringstream ss;

    if ((val%16) == 0)
    {
        ss << (val/16) ;
    }
    else if ((val%8) == 0)
    {
        ss << (val/8) << "/" << "2";
    }
    else if ((val%4) == 0)
    {
        ss << (val/4) << "/" << "4";
    }
    else if ((val%2) == 0)
    {
        ss << (val/2) << "/" << "8";
    }
    else\
    {
        ss << val << "/" << "16";
    }
    //ss << "  (" << val << "/16)";

    hOutDebug(7, "text=" << ss.str());
    return ss.str().c_str();
}



//
//  PaperPreview
//


PaperPreview::PaperPreview(StationeryDialog *dialog, QWidget *parent) :
    QWidget(parent), mDialog(dialog)
{

}

void PaperPreview::paintEvent(QPaintEvent *)
{

    QPainter p(this);
    StationeryPtr s = mDialog->createRecord();
    PagePrinter pp(s, p);
    pp.preparePage(true);
}
