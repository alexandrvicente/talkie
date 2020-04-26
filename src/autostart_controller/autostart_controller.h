#ifndef AUTOSTARTCONTROLLER_H
#define AUTOSTARTCONTROLLER_H

#include <QObject>

class AutostartController : public QObject
{
    Q_OBJECT
public:
    static AutostartController *shared();
    bool enabled();
    void setEnabled(bool enabled);
private:
    static AutostartController *instance;
    explicit AutostartController(QObject *parent = nullptr);


signals:

};

#endif // AUTOSTARTCONTROLLER_H
