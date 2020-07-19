#pragma once

#include <QSpinBox>
#include <QTimer>

class SpinBox : public QSpinBox {
        Q_OBJECT
    public:
        explicit SpinBox( QWidget* parent = 0 );

    signals:
        void signalUndoStep();

    public slots:

    private slots:
        void slotOnValueChanged( int );

    protected:
        virtual void focusInEvent( QFocusEvent* event );

    private:
        int m_OnFocusIn;
        QTimer m_UndoTimer;
};

