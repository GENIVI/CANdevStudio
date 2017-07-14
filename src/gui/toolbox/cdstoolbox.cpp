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
}

void CdsToolbox::startButtonClicked()
{
    cds_debug("start button clicked");
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
}

void CdsToolbox::stopButtonClicked()
{
    cds_debug("stop button clicked");
    ui->stopButton->setEnabled(false);
    ui->startButton->setEnabled(true);
}

CdsToolbox::~CdsToolbox()
{
    delete ui;
}
