#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>

namespace Ui {
class About;
}

class QNetworkAccessManager;
class QNetworkReply;

//! \brief QDialog class to show the about and license texts
class About : public QDialog {
        Q_OBJECT

    public:
        explicit About( QWidget* parent = 0 );
        ~About();

    private slots:
        void on_boxLicense_currentIndexChanged( int index );
        void on_buttonWebsite_clicked();
        void on_buttonFeedback_clicked();

    private:
        Ui::About* ui;
};

#endif // ABOUT_H
