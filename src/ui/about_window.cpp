#include "about_window.h"
#include "ui_about_window.h"

AboutWindow::AboutWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Tool);
    setWindowFlags(Qt::WindowStaysOnTopHint);

    QColor textColor = QApplication::palette().text().color();

    QString sourceText = ui->aboutText->text();
    QString finalText = sourceText
       .replace("<a ", "<a style=\"color:" + textColor.name(QColor::NameFormat::HexRgb) + "\"")
       .replace("{{version}}", QCoreApplication::applicationVersion());

    ui->aboutText->setText(finalText);
}

AboutWindow::~AboutWindow()
{
    delete ui;
}
