#ifndef FINISHGROUP_H
#define FINISHGROUP_H

#include <QWidget>

class QPushButton;
class QFrame;

namespace Ui {
class FinishGroup;
}

//! \brief Container for buttons ok, quit, reset, about
class FinishGroup : public QWidget {
        Q_OBJECT

    public:
        explicit FinishGroup( QWidget* parent = 0 );
        ~FinishGroup();

        QPushButton* buttonOk();
        QPushButton* buttonQuit();
        QPushButton* buttonAbout();
        QPushButton* buttonEdit();
        QPushButton* buttonPresets();
        QFrame* frameEdit();
        QFrame* frameOk();

    signals:
        void signalOk();
        void signalQuit();
        void signalAbout();

    private slots:
        void on_buttonOK_clicked();
        void on_buttonQuit_clicked();
        void on_buttonAbout_clicked();

    private:
        Ui::FinishGroup* ui;
};

#endif // FINISHGROUP_H
