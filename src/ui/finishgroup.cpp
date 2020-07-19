#include "finishgroup.h"
#include "ui_finishgroup.h"

FinishGroup::FinishGroup( QWidget* parent ) :
    QWidget( parent ),
    ui( new Ui::FinishGroup ) {
    ui->setupUi( this );
}

FinishGroup::~FinishGroup() {
    delete ui;
}

QPushButton* FinishGroup::buttonOk() {
    Q_ASSERT( ui->buttonOK );
    return ui->buttonOK;
}
QPushButton* FinishGroup::buttonQuit() {
    Q_ASSERT( ui->buttonQuit );
    return ui->buttonQuit;
}
QPushButton* FinishGroup::buttonAbout() {
    Q_ASSERT( ui->buttonAbout );
    return ui->buttonAbout;
}
QPushButton* FinishGroup::buttonPresets() {
    Q_ASSERT( ui->buttonPresets );
    return ui->buttonPresets;
}
QFrame* FinishGroup::frameEdit() {
    Q_ASSERT( ui->frameEdit );
    return ui->frameEdit;
}
QFrame* FinishGroup::frameOk() {
    Q_ASSERT( ui->frameOk );
    return ui->frameOk;
}

QPushButton* FinishGroup::buttonEdit() {
    Q_ASSERT( ui->buttonEdit );
    return ui->buttonEdit;
}

void FinishGroup::on_buttonOK_clicked() {
    emit signalOk();
}

void FinishGroup::on_buttonQuit_clicked() {
    emit signalQuit();
}

void FinishGroup::on_buttonAbout_clicked() {
    emit signalAbout();
}
