#include "pointlessbar.hpp"

#include <QMovie>

PointlessBar::PointlessBar( QWidget* parent ) :
    QLabel( parent ),
    mMovie( new QMovie( ":/GUI/Icons/progress.gif" ) ) {
    this->setMovie( mMovie );
    this->hide();
}

PointlessBar::~PointlessBar() {
    delete mMovie;
}

void PointlessBar::start() {
    this->show();
    mMovie->start();
}

void PointlessBar::stop() {
    mMovie->stop();
    this->hide();
}
