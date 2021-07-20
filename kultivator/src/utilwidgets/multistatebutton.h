#ifndef MULTISTATEBUTTON_H
#define MULTISTATEBUTTON_H

#include <QPushButton>
#include <QIcon>

// Button that loops through icons on a click and signals which one is currently used
class MultistateButton : public QPushButton
{
    Q_OBJECT

public:
    MultistateButton(QWidget* parent = nullptr);

    void setStates(int count, const QVector<QString>& paths = QVector<QString>());
    // Sets the state, doesn't emit anything.
    void setClickState(int newState);
    // Return current state.
    int currState() { return state; }

signals:
    // Emits previous state.
    void stateChanged(int state);

private slots:
    // Increments on a modulo, emits a signal
    void nextState();

private:
    // Count from zero, -1 us null
    int state = -1;
    int size = 0;

    QVector<QIcon> icons;
};

#endif // MULTICHECKBUTTON_H
