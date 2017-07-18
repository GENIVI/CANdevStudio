#include "cdstoolbar.h"
#include "ui_cdstoolbar.h"
#include "log.hpp"

CdsToolbar::CdsToolbar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CdsToolbar)
{
    ui->setupUi(this);
    connect(ui->startButton,SIGNAL(clicked(bool)),this,SLOT(startButtonClicked()));
    connect(ui->stopButton,SIGNAL(clicked(bool)),this,SLOT(stopButtonClicked()));
    connect(ui->startButton,SIGNAL(clicked(bool)),this,SIGNAL(simulationStart()));
    connect(ui->stopButton,SIGNAL(clicked(bool)),this,SIGNAL(simulationStop()));
}

void CdsToolbar::startButtonClicked()
{
    cds_debug("start button clicked");
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
}

void CdsToolbar::stopButtonClicked()
{
    cds_debug("stop button clicked");
    ui->stopButton->setEnabled(false);
    ui->startButton->setEnabled(true);
}

CdsToolbar::~CdsToolbar()
{
    delete ui;
}
