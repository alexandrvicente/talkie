#ifndef UPDATE_WINDOW_H
#define UPDATE_WINDOW_H

#include <QDialog>
#include <QBitmap>
#include <QPainter>
#include "../updater/updater.h"

namespace Ui {
class UpdateWindow;
}

class UpdateWindow : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateWindow(Updater *updater, QString version, QString changelog, QString url, QString signature);
    ~UpdateWindow();

private:
    Ui::UpdateWindow *ui;
    QString version;
};

#endif // UPDATE_WINDOW_H
