#pragma once

#include <QDoubleSpinBox>
#include <QTimer>

class DoubleSpinBox : public QDoubleSpinBox {
        Q_OBJECT
    public:
        explicit DoubleSpinBox( QWidget* parent = 0 );

    signals:
        void signalUndoStep();

    public slots:

    private slots:
        void slotOnValueChanged( double );

    protected:
        virtual void focusInEvent( QFocusEvent* event );

    private:
        double m_OnFocusIn;
        QTimer m_UndoTimer;
};
