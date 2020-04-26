#include "hotkey_input.h"

const QVector<Qt::KeyboardModifier> modifiers = {
    Qt::NoModifier,
    Qt::ShiftModifier,
    Qt::ControlModifier,
    Qt::AltModifier,
    Qt::MetaModifier,
    Qt::KeypadModifier,
    Qt::GroupSwitchModifier,
    Qt::KeyboardModifierMask
};

HotkeyInput::HotkeyInput(QWidget *parent)
    : QLineEdit(parent)
{
}

QKeySequence HotkeyInput::getKeySequence()
{
    return keySequence;
}

void HotkeyInput::setKeySequence(QKeySequence keySequence)
{
    if(keySequence != this->keySequence){
        this->keySequence = keySequence;
        setText(keySequence.toString(QKeySequence::NativeText));
        emit keySequenceChanged(keySequence);
    }
}

void HotkeyInput::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    Qt::Key qtKey = static_cast<Qt::Key>(key);

    if(qtKey == Qt::Key_Escape){
        clearFocus();
        return;
    }

    if(qtKey == Qt::Key_Backspace){
        setKeySequence(QKeySequence());
        return;
    }

    if(qtKey == Qt::Key_Shift || qtKey == Qt::Key_Control || qtKey == Qt::Key_Alt || qtKey == Qt::Key_Meta){
        return;
    }

    Qt::KeyboardModifiers eventModifiers = event->modifiers();

    bool noEventModifiers = true;

    foreach(Qt::KeyboardModifier modifier, modifiers){
        if(eventModifiers.testFlag(modifier)){
            key += modifier;
            noEventModifiers = false;
        }
    }

    if(noEventModifiers){
        return;
    }

    QKeySequence keySequence(key);

    setKeySequence(keySequence);
}
