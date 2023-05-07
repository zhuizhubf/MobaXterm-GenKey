
#include "mainwindow.h"

#include "genkey.h"

#include <layoutbuilder.h>

#include <QDebug>
#include <QLabel>
#include <QLineEdit>
#include <QVersionNumber>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setCentralWidget(new QWidget(this));

    using namespace Layouting;

    number = new QLineEdit("1", this);
    name = new QLineEdit("zhuizhu", this);
    version = new QLineEdit("23.1", this);

    error = new QLabel(this);

    // clang-format off
    Column{
        Grid{
            new QLabel("Number:", this), number, br, // 1
            new QLabel("Name:", this), name, br,     // 2
            new QLabel("Version:", this), version,   // 3
        },
        st,
        hr,
        // clang-format on
        Column{PushButton{text("Generate"), onClicked([this] {
                              qDebug() << number << name << version << error;
                              int count = number->text().toInt();
                              const QString Name = name->text().trimmed();
                              QVersionNumber versionNumber = QVersionNumber::fromString(
                                  version->text());
                              if (versionNumber.segmentCount() != 2) {
                                  error->setText("version err format: xx.xx");
                                  error->show();
                              };

                              QByteArray success = GenerateLicense(LicenseType::Professional,
                                                                   count,
                                                                   Name,
                                                                   versionNumber.majorVersion(),
                                                                   versionNumber.microVersion());

                              error->setText("success: " + success);
                              error->show();
                          })}},
        error,
    }
        .attachTo(centralWidget());
    // clang-format on
    error->hide();
}

MainWindow::~MainWindow() {}
