#ifndef HOTKEYINPUT_H
#define HOTKEYINPUT_H

#include <QLineEdit>
#include <QKeyEvent>
#include <QKeySequence>
#include <QVector>

class HotkeyInput : public QLineEdit
{
    Q_OBJECT
private:
    QKeySequence keySequence;
public:
    HotkeyInput(QWidget *parent = nullptr);
    ~HotkeyInput(){}
    QKeySequence getKeySequence();
public slots:
    void setKeySequence(QKeySequence keySequence);
signals:
    void keySequenceChanged(QKeySequence keySequence);
protected:
    void keyPressEvent (QKeyEvent *event);
};

#endif // HOTKEYINPUT_H
