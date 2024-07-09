#include "mainwindow.h"
#include "src/minirds.h"
#include "src/fm_mpx.h"
#include "src/osc.h"
#include "./ui_mainwindow.h"
#include <iostream>

int samples_target=2048;
int samples_got=0;
float fft_samples[2048];

void MainWindow::updatedisplay()
{
TSigStats *psigstats=GetSignalStats();

for (int x=0;samples_got<=2048 && x<psigstats->sample_counter;x++) {
fft_samples[samples_got]=psigstats->samples[samples_got];
samples_got++;
}

if ( samples_got == 2048) {
ui->widget_9->calculateFFT2(2048,fft_samples);
samples_got=0;
}

if (psigstats->outvol != ui->outvolumemeter->volume()) {
ui->outvolumemeter->setVolume(psigstats->outvol);
}
return;
}

void MainWindow::showEvent(QShowEvent *event)   // Call set parameters
{
    QMainWindow::showEvent(event);
    QTimer::singleShot(100, this, SLOT(window_shown()));
    return;
}

void MainWindow::window_shown() {
    //std::cout << "Running" << std::endl;
    if (rdsStarted == 0) {
    rdsStarted = 1;
    int ret=RDSmain();
    init_soundio("{0.0.0.00000000}.{81110f5c-4fe5-4545-a09f-a729bb9d5295}",0.22,192000);

    ui->scavolumemeter->setDirection(Volumemeter::Vertical);
    ui->lvolumemeter->setDirection(Volumemeter::Vertical);
    ui->rvolumemeter->setDirection(Volumemeter::Vertical);
    ui->outvolumemeter->setDirection(Volumemeter::Vertical);

    ui->scavolumemeter->setVolume(0.0f);
    ui->lvolumemeter->setVolume(0.0f);
    ui->rvolumemeter->setVolume(0.0f);
    ui->outvolumemeter->setVolume(100.0f);
    ui->widget_9->mInit();

    QLed::ledColor col = QLed::ledColor::Red;
    QLed::ledShape shp = QLed::ledShape::Circle;

    ui->widget_10->setOnColor(col);
    ui->widget_10->setOffColor(col);
    ui->widget_10->setShape(shp);
    ui->widget_10->setValue(true);
    set_output_volume(50.0f);

    set_carrier_volume_raw(0,0.0001f);
    set_carrier_volume_raw(1,0.0001f);
    set_carrier_volume_raw(2,0.0001f);
    set_carrier_volume_raw(3,0.0001f);
    set_carrier_volume_raw(4,0.0001f);



    }
    return;
}
void MainWindow::on_dial_valueChanged(int value)
{
float myval = value * 1.0;
QString output_level = QString::number(myval);
ui->label_4->setText(output_level);
set_output_volume(myval);
return;
}


void MainWindow::on_dial_2_valueChanged(int value)
{
float myval = (value /30.0) * 0.01000f;
QString output_level = QString::number(myval);
ui->label_14->setText(output_level);
set_carrier_volume_raw(0,myval);
return;
}

void MainWindow::on_dial_3_valueChanged(int value)
{
QString output_level = QString::number(value);
}

void MainWindow::on_dial_4_valueChanged(int value)
{
float myval = (value /60.0)*0.01000;
QString output_level = QString::number(myval);
ui->label_16->setText(output_level);
set_carrier_volume_raw(1,myval);
return;
}

void MainWindow::on_dial_5_valueChanged(int value)
{
float myval = (value /0.8) * 0.01000f;
QString output_level = QString::number(myval);
ui->label_17->setText(output_level);
set_carrier_volume_raw(2,myval);
return;
}
void MainWindow::on_dial_6_valueChanged(int value)
{
float myval = (value /0.8) *0.01000f;
QString output_level = QString::number(myval);
ui->label_18->setText(output_level);
set_carrier_volume_raw(3,myval);
return;
}

void MainWindow::on_dial_7_valueChanged(int value)
{
float myval = (value /0.8)*0.01000f;
QString output_level = QString::number(myval);
ui->label_19->setText(output_level);
set_carrier_volume_raw(4,myval);
return;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->groupBox_sca->setVisible(0);

    //display update init
    ptimer= new QTimer(this);
    ptimer->setInterval(30);
    ptimer->start();
    connect(ptimer,SIGNAL(timeout()),this,SLOT(updatedisplay()));
}

MainWindow::~MainWindow()
{
    delete ui;

}
