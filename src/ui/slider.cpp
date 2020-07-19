
#include "slider.hpp"

Slider::Slider( QWidget* parent ) : QSlider( parent ) {

}

Slider::~Slider() {

}

void Slider::mouseReleaseEvent( QMouseEvent* ) {
    emit signalMouseReleased();
}

