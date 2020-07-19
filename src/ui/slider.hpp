#ifndef SLIDER_HPP
#define SLIDER_HPP

#include <QSlider>


class Slider : public QSlider {
        Q_OBJECT
    public:
        explicit Slider( QWidget* parent = 0 );
        ~Slider();

    signals:
        void signalMouseReleased();

    public slots:
    private:
        virtual void mouseReleaseEvent( QMouseEvent* );
};

#endif // SLIDER_HPP
