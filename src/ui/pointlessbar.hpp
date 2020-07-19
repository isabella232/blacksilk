#ifndef POINTLESSBAR_HPP
#define POINTLESSBAR_HPP

#include <QLabel>

class PointlessBar : public QLabel {
        Q_OBJECT
    public:
        explicit PointlessBar( QWidget* parent = 0 );
        ~PointlessBar();

    signals:

    public slots:
        void start();
        void stop();
    private:
        QMovie* mMovie;
};

#endif // POINTLESSBAR_HPP
