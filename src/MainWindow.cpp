#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <iostream>
#include <QFileDialog>
#include <QFile>
#include <string>
#include <QTextStream>

MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	ports{ QSerialPortInfo::availablePorts() }
{
	ui->setupUi(this);
	
	populateCOMPortCombo();
	
	connect(ui->browseButton, &QPushButton::clicked,
			this, &MainWindow::browse);
	connect(ui->distanceInput, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
			this, &MainWindow::updateDistance);
	connect(ui->notesEdit, &QPlainTextEdit::textChanged,
			this, &MainWindow::updateNotes);
	connect(ui->rescanButton, &QPushButton::clicked,
			this, &MainWindow::rescanSerial);
	connect(ui->goButton, &QPushButton::clicked,
			this, &MainWindow::go);
	connect(ui->uuidEdit, &QLineEdit::textChanged,
			this, &MainWindow::updateUUID);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::browse()
{
	QString path{ QFileDialog::getExistingDirectory(this, "Select Directory") };
	setTargetDirectory(QDir{ path });
}

void MainWindow::setTargetDirectory(const QDir& directory)
{
	targetDirectory = directory;
	updateDirectoryView();
}

void MainWindow::updateDirectoryView()
{
	ui->pathEdit->setText(targetDirectory.absolutePath());
}

void MainWindow::updateDistance()
{
	distance = ui->distanceInput->value();
}

void MainWindow::updateNotes()
{
	notes = ui->notesEdit->toPlainText();
}

void MainWindow::updateUUID()
{
	uuid = ui->uuidEdit->text().toUpper();
}

void MainWindow::populateCOMPortCombo()
{
	for(QSerialPortInfo port : ports)
		ui->comPortCombo->addItem(port.portName() + QString(" - ") + port.description());
}

void MainWindow::rescanSerial()
{
	ui->comPortCombo->clear();
	ports = QSerialPortInfo::availablePorts();
	populateCOMPortCombo();
}

void MainWindow::go()
{
	ui->progressBar->setValue(0);
	QSerialPort port{ ports[ui->comPortCombo->currentIndex()] };
	std::cout << port.portName().toStdString() << '\n' << port.baudRate() << "\n\n";
	port.open(QIODevice::ReadWrite);

	QStringList readings{};
	for (int i{ 0 }; i < 10; ++i)
	{
		port.write("go\n");
		try
		{
			if (getSerialLine(port) == "rc\n")
			{
				std::cout << "Connected to Arduino...\n";
				if (getSerialLine(port) == "btrd\n")
				{
					std::cout << "Recieving data from bluetooth adapter...\n";
					QString data{ getSerialLine(port) };
					readings.append(data.mid(data.indexOf(uuid) + 57, 4));
					std::cout << readings[i].toStdString() << '\n';
					ui->progressBar->setValue(i + 1);
				}
				else
					throw "Bluetooth adapter failed to respond: try again.";
			}
			else
				throw "Unable to communicate with arduino. Check the right COM port is selected and try again.\n";

		}
		catch (const char* message)
		{
			std::cerr << message << '\n';
			break;
		}
	}
	writeData(readings);
}


QString MainWindow::getSerialLine(QSerialPort& port)
{
	QString ret{};
	do
	{
		if (port.waitForReadyRead(5000))
			ret += port.readAll();
		else
			return "fail";
	} while (!ret.contains('\n'));
	return ret;
}

bool MainWindow::writeData(const QStringList& data)
{
	QString target{ targetDirectory.absolutePath() + '/' + QString::fromStdString(std::to_string(distance)) + "m.csv" };
	QFile file{ target };
	if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		std::cerr << "Failed to write file.\n";
		return false;
	}
	QTextStream ostream{ &file };
	ostream << notes << '\n';
	for (QString item : data)
		ostream << item << '\n';
	file.close();
	return true;
}