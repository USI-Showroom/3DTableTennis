#ifndef GLRENDER_H
#define GLRENDER_H

#include "ui_GLRender.h"


#include <QMainWindow>

namespace Ui {
	class GLRender;
}

class GLRender : public QMainWindow
{
	Q_OBJECT

public:
	explicit GLRender(QWidget *parent = 0);
	virtual ~GLRender(){}

private:
	Ui::GLRender *ui;

};


#endif
