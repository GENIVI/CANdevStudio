#include "cdstoolbar.h"
#include "ui_cdstoolbar.h"
#include "log.hpp"

CdsToolbar::CdsToolbar(QWidget *parent) :
    QWidget(parent),
    ui(std::make_unique<Ui::CdsToolbar>())
{
    ui->setupUi(this);
    connect(ui->startButton,SIGNAL(clicked(bool)),this,SIGNAL(simulationStart()));
    connect(ui->stopButton,SIGNAL(clicked(bool)),this,SIGNAL(simulationStop()));
    connect(ui->startButton,SIGNAL(clicked(bool)),ui->startButton,SLOT(setEnabled(bool)));
    connect(ui->startButton,SIGNAL(clicked(bool)),ui->stopButton,SLOT(setDisabled(bool)));
    connect(ui->stopButton,SIGNAL(clicked(bool)),ui->startButton,SLOT(setDisabled(bool)));
    connect(ui->stopButton,SIGNAL(clicked(bool)),ui->stopButton,SLOT(setEnabled(bool)));
}

CdsToolbar::~CdsToolbar()
{
}
