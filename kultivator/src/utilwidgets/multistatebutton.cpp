#include "multistatebutton.h"

MultistateButton::MultistateButton(QWidget *parent) :
    QPushButton(parent)
{
    connect(this, &QPushButton::clicked, this, &MultistateButton::nextState);
}

// Copy all icon paths to local icons
void MultistateButton::setStates(int count, const QVector<QString>& paths)
{
    if (count < 0)
        return;

    size = count;
    state = 0;
    icons.resize(size);
    icons.squeeze();
    if (paths.size())
    {
        // If paths < size out of bounds state will load an empty image
        for (int i = 0; i < paths.size(); ++i)
            icons[i] = QIcon(paths[i]);
        setIcon(icons[0]);
    }
}

void MultistateButton::setClickState(int newState)
{
    state = newState % size;
    setIcon(icons[state]);
}

void MultistateButton::nextState()
{
    int oldState = state;
    setClickState(++state);
    emit stateChanged(oldState);
}
