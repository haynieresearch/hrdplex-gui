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

#include "TapeSplt.h"
#include "gui.h"
#include "UtilityRunner.h"
#include "Preferences.h"
#include <QLineEdit>

#include <QFileDialog>
#include <QMessageBox>

#include <csignal>

TapeSplt::TapeSplt(QWidget *parent)
	: GenericUtility("tapesplt",parent)
{
	ui.setupUi(this);
	ui.tableView->setEditTriggers(QTableView::NoEditTriggers);
	connect(ui.runButton, SIGNAL(clicked()), this, SLOT(runClicked()));
	connect(ui.exitButton, SIGNAL(clicked()), this, SLOT(cancelClicked()));
	connect(ui.inputSelButton, SIGNAL(clicked()), this, SLOT(selectInputClicked()));
	connect(ui.addButton, SIGNAL(clicked()), this, SLOT(addClicked()));
	connect(ui.deleteButton, SIGNAL(clicked()), this, SLOT(deleteClicked()));
	connect(ui.downButton, SIGNAL(clicked()), this, SLOT(downClicked()));
	connect(ui.upButton, SIGNAL(clicked()), this, SLOT(upClicked()));
	mMaxFile = ui.tableView->geometry().width()/8-8;
}

TapeSplt::~TapeSplt()
{

}

void TapeSplt::runClicked()
{
	if (!runOrStopClicked())
	{
		ui.runButton->setText("Run");
		return;
	}

	if (ui.inputFile->text().isEmpty())
	{
		QMessageBox::warning(this, "tapesplt", "Please specify input file to process",
				QMessageBox::Ok, QMessageBox::NoButton);
		return;
	}
	if (!QFile::exists(ui.inputFile->text()))
		{
			QMessageBox::warning(this, "tapesplt", "The specified file does not exist",
					QMessageBox::Ok, QMessageBox::NoButton);
			return;
		}

	std::vector<std::string> parameters;

	parameters.push_back(ui.inputFile->text().toStdString());

	for (int i=0; i<mModel->rowCount(); i++)
	{
		QModelIndex ix = mModel->index(i,0,QModelIndex());
		QString row = mModel->data(ix).toString();
		QString fileName = row.left(mMaxFile).trimmed();
		QString blocks = row.mid(mMaxFile);
		ui.inputFile->setText(fileName);
		parameters.push_back(fileName.toStdString());
		parameters.push_back(blocks.toStdString());
	}

	std::string command = "tapesplt";

	execute(command, Preferences::getInstance().hercDir(), parameters);
	ui.runButton->setText("Stop");
}


void TapeSplt::cancelClicked()
{
	deleteLater();
}

void TapeSplt::selectInputClicked()
{
	QString s = QFileDialog::getOpenFileName(this,"Browse for input tape file",ui.inputFile->text());
	ui.inputFile->setText(s);
}

void TapeSplt::addClicked()
{
	TapeSpltSubDlg * subDlg = new TapeSpltSubDlg(this);
	connect (subDlg, SIGNAL(fileNameSet(QString, int)), this, SLOT(fileNameSetSlot(QString, int)));
	subDlg->exec();
}

void TapeSplt::deleteClicked()
{
	int i = ui.tableView->currentIndex().row();
	if (i>= 0) mModel->removeRow(i);
	QStandardItemModel t;
	ui.tableView->setModel(&t);
	ui.tableView->setModel(mModel);
	ui.tableView->resizeColumnToContents(0);
}

void TapeSplt::downClicked()
{
	int i = ui.tableView->currentIndex().row();
	if (i>=0 && (i+1)<mModel->rowCount())
	{
		QVariant itemA = mModel->rowData(ui.tableView->currentIndex());
		QVariant itemB = mModel->rowData(ui.tableView->currentIndex().sibling(i+1,0));
		mModel->setData(ui.tableView->currentIndex(),itemB);
		mModel->setData(ui.tableView->currentIndex().sibling(i+1,0),itemA);
		ui.tableView->setCurrentIndex(ui.tableView->currentIndex().sibling(i+1,0));
		QStandardItemModel t;
		ui.tableView->setModel(&t);
		ui.tableView->setModel(mModel);
		ui.tableView->resizeColumnToContents(0);	}
}
void TapeSplt::upClicked()
{
	int i = ui.tableView->currentIndex().row();
	if (i>=1 && mModel->rowCount() > 1)
	{
		QVariant itemA = mModel->rowData(ui.tableView->currentIndex().sibling(i-1,0));
		QVariant itemB = mModel->rowData(ui.tableView->currentIndex());
		mModel->setData(ui.tableView->currentIndex(),itemA);
		mModel->setData(ui.tableView->currentIndex().sibling(i-1,0),itemB);
		ui.tableView->setCurrentIndex(ui.tableView->currentIndex().sibling(i-1,0));
		QStandardItemModel t;
		ui.tableView->setModel(&t);
		ui.tableView->setModel(mModel);
		ui.tableView->resizeColumnToContents(0);	}
}
void TapeSplt::fileNameSetSlot(QString fileName, int blocks)
{
	if (fileName == "") return;
	if (ui.tableView->model() == NULL)
	{
		mModel = new HTableModel(this);
	}
	else
	{
		QStandardItemModel t;
		ui.tableView->setModel(&t);
	}
	mModel->insertRow(mModel->rowCount()-1, QModelIndex(), fileName, QString::number(blocks));
	ui.tableView->setModel(mModel);
	ui.tableView->resizeColumnToContents(0);
}

QString TapeSplt::genLine(QString file, int blocks)
{
	QString ret = file.leftJustified(mMaxFile,' ') + QString::number(blocks);
	return file.leftJustified(mMaxFile,' ') + QString::number(blocks);
}


void TapeSplt::finishedSlot()
{
	if (mFinishedOK)
	{
		QMessageBox::information(this, "tapsplt", "Tape split successfully completed!",
			QMessageBox::Ok,
			QMessageBox::NoButton);
		deleteLater();
	}
	else
		emit error();

	ui.runButton->setText("Run");
}

TapeSpltSubDlg::TapeSpltSubDlg(QWidget * parent) : QDialog(parent)
{
	setupUi();
	connect(selOutputButton, SIGNAL(clicked()), this, SLOT(selectOutputSlot()));
	connect(addButton, SIGNAL(clicked()), this, SLOT(addSlot()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelSlot()));
}

TapeSpltSubDlg::~TapeSpltSubDlg()
{

}

void TapeSpltSubDlg::setupUi()
{
	resize(554, 204);
	label = new QLabel(this);
	label->setObjectName(QString::fromUtf8("label"));
	label->setGeometry(QRect(30, 30, 461, 16));
	lineEdit = new QLineEdit(this);
	lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
	lineEdit->setGeometry(QRect(30, 60, 461, 20));
	selOutputButton = new QPushButton(this);
	selOutputButton->setObjectName(QString::fromUtf8("selOutputButton"));
	selOutputButton->setGeometry(QRect(510, 60, 31, 23));
	spinBox = new QSpinBox(this);
	spinBox->setObjectName(QString::fromUtf8("spinBox"));
	spinBox->setGeometry(QRect(30, 120, 42, 22));
	spinBox->setMaximum(9);
	spinBox->setValue(1);
	label_2 = new QLabel(this);
	label_2->setObjectName(QString::fromUtf8("label_2"));
	label_2->setGeometry(QRect(30, 90, 251, 16));
	addButton = new QPushButton(this);
	addButton->setObjectName(QString::fromUtf8("addButton"));
	addButton->setGeometry(QRect(30, 160, 75, 23));
	cancelButton = new QPushButton(this);
	cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
	cancelButton->setGeometry(QRect(460, 160, 75, 23));


	setWindowTitle("Add Output Tape");
	label->setText("Name of output file:");
	selOutputButton->setText("...");
	label_2->setText("Number of blocks to extarct into this tape file:");
	addButton->setText("Add");
	cancelButton->setText("Cancel");
}

void TapeSpltSubDlg::selectOutputSlot()
{
	QString s = QFileDialog::getOpenFileName(this,"Browse for output tape file",lineEdit->text());
	lineEdit->setText(s);
}

void TapeSpltSubDlg::addSlot()
{
	emit fileNameSet(lineEdit->text(), spinBox->value());
	deleteLater();
}

void TapeSpltSubDlg::cancelSlot()
{
	deleteLater();
}

//////////////////////////////////////

HTableModel::HTableModel(QObject *o) : QAbstractTableModel(o)
{

}

HTableModel::~HTableModel()
{

}

bool HTableModel::insertRow(int row, const QModelIndex &index, QString filename, QString blocks)
{
	hOutDebug(3,"insert " << filename.toStdString() << " to row "<< row);

	if (row > static_cast<int>(mTable.size())) return false;
	HTableType::iterator it = mTable.begin()+(1+row);
	QStringList list;
	list << filename << blocks ;

	beginInsertRows(index, row+1, row+2);
	mTable.insert(it, list);
	endInsertRows();

	return true;
}

bool HTableModel::removeRow ( int row, const QModelIndex & parent)
{
	if (parent.isValid()) return false;
	if (row >= static_cast<int>(mTable.size())) return false;
	HTableType::iterator it = mTable.begin()+row;
	mTable.erase(it);
	return true;
}

int HTableModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid()) return 0;
	else return mTable.size();
}

int HTableModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid()) return 0;
	else return 2;
}

QVariant HTableModel::data(const QModelIndex &index, int role) const
{
	if (index.row() >= static_cast<int>(mTable.size())) return QVariant();
	QStringList list = mTable[index.row()];
	hOutDebug(5,"data(): request for col=" << index.column() << ";row=" << index.row() <<";role=" << role);
	hOutDebug(5,"        list with size=" << list.size());
	if (role != Qt::DisplayRole) return QVariant();
	if (index.column() == 0) return QVariant(list[0]);
	else return QVariant(list[1]);
}

QVariant HTableModel::rowData(const QModelIndex &index) const
{
	if (index.row() >= static_cast<int>(mTable.size())) return QVariant();
	QStringList list = mTable[index.row()];
	return QVariant(list);
}

QVariant HTableModel::headerData ( int col, Qt::Orientation orientation, int role ) const
{
	if (orientation == Qt::Horizontal && role == 0)
	{
		if (col == 0) return QVariant(QString("File-Name"));
		else return QVariant(QString("Blocks"));
	}
	else
	{
		return QVariant();
	}
}

bool HTableModel::setData(const QModelIndex &index, const QVariant &value, int)
{
	if (static_cast<int>(mTable.size()) <= index.row()) return false;
	if (!value.isValid() || !value.canConvert(QVariant::StringList)) return false;

	QStringList list = value.toStringList();
	mTable[index.row()] = list;
	return true;
}
