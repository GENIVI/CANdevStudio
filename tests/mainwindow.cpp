#include <candevice/candevice.h>
#define CATCH_CONFIG_RUNNER
#include <fakeit.hpp>
#include "mainwindow.h"
#include "log.hpp"

std::shared_ptr<spdlog::logger> kDefaultLogger;


TEST_CASE("load file, loaded properly", "[mainwindow]") {

    std::shared_ptr<Ui::MainWindow>ui;
    QApplication a();
    QWidget *parent= nullptr;
    MainWindow *t=new MainWindow(parent,ui);


   // ui.get().actionLoad;


}

int main(int argc, char *argv[])
{
    return Catch::Session().run(argc, argv);
}
