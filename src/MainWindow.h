#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QString>
#include <QSerialPortInfo>
#include <QSerialPort>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private:
	Ui::MainWindow *ui;

	QDir targetDirectory;
	double distance;
	QString notes;
	QString uuid;
	QList<QSerialPortInfo> ports;
	
	void setTargetDirectory(const QDir& directory);

	void browse();
	void updateDirectoryView();
	void updateNotes();
	void updateUUID();
	void updateDistance();
	void rescanSerial();
	void populateCOMPortCombo();

	void go();
	QString getSerialLine(QSerialPort& port);
	bool writeData(const QStringList& data);
};

#endif // MAINWINDOW_H
