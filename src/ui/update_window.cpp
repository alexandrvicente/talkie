#include "update_window.h"
#include "ui_update_window.h"

UpdateWindow::UpdateWindow(Updater *updater, QString version, QString changelog, QString url, QString signature) :
    QDialog(),
    ui(new Ui::UpdateWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Tool);
    setWindowFlags(Qt::WindowStaysOnTopHint);

    this->version = version;
    ui->headerLabel->setText(
                ui->headerLabel->text()
                .replace("{{currentVersion}}", QCoreApplication::applicationVersion())
                .replace("{{newVersion}}", version)
                );
    ui->changelog->setMarkdown(changelog);

    connect(ui->skipButton, &QPushButton::clicked, this, [this, version] () {
        QSettings settings;
        settings.setValue("skippedUpdateVersion", version);
        hide();
    });
    connect(ui->laterButton, &QPushButton::clicked, this, [this, version] () {
        QSettings settings;
        settings.setValue("skippedUpdateVersion", "");
        hide();
    });
    connect(ui->installButton, &QPushButton::clicked, this, [this, updater, url, signature] () {
        updater->downloadAndInstall(url, signature);
        hide();
    });
}

UpdateWindow::~UpdateWindow()
{
    delete ui;
}
