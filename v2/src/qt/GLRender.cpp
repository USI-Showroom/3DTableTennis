#include "GLRender.h"
#include "CCanvas.h"

GLRender::GLRender(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::GLRender)
{
    ui->setupUi(this);    

	ui->centralLayout->addWidget(new CCanvas(this));
}
 