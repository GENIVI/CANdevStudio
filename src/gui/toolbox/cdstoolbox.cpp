#include "cdstoolbox.h"
#include "ui_cdstoolbox.h"
#include "log.hpp"

CdsToolbox::CdsToolbox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CdsToolbox)
{
    ui->setupUi(this);
    connect(ui->startButton,SIGNAL(clicked(bool)),this,SLOT(startButtonClicked()));
    connect(ui->stopButton,SIGNAL(clicked(bool)),this,SLOT(stopButtonClicked()));
    connect(ui->startButton,SIGNAL(clicked(bool),this,SIGNAL(simulationStart());
    connect(ui->stopButton,SIGNAL(clicked(bool),this,SIGNAL(simulationStop());
}

void CdsToolbox::startButtonClicked()
{
    cds_debug("start button clicked");
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
    emit simulationStart();
}

void CdsToolbox::stopButtonClicked()
{
    cds_debug("stop button clicked");
    ui->stopButton->setEnabled(false);
    ui->startButton->setEnabled(true);
    emit simulationStop();
}

CdsToolbox::~CdsToolbox()
{
    delete ui;
}
