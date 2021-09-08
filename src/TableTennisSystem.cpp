#include "TableTennisSystem.h"
#include "PaddleDrawer.h"
#include "Defines.h"
#include "BallLauncher.h"
#include "CCanvas.h"
#include "Shader.h"

TableTennisSystem::TableTennisSystem(CCanvas *canvas, Shader *text, Shader *shader)
	:ball(new EasyLauncher()),
	table(),
	scoreBoard(),
	score(canvas,text,shader)
{
	oppPaddle = new OpponentPaddle(&ball);
	ball.setScore(&score);
	ball.setOppPaddle(oppPaddle);
}

void TableTennisSystem::update(const double tau)
{
	if(!score.finish())
	{
		scoreBoard.update(tau);
		ball.update(tau);
		table.update(tau);
		oppPaddle->update(tau);
	}
	else
	{
		ball.resetPlayerReady();
	}

	score.update();
}

void TableTennisSystem::draw(unsigned int shaderId)
{
	if(!score.finish())
	{
		ball.draw(shaderId);
		table.draw(shaderId);
		oppPaddle->draw(shaderId);
		PaddleDrawer::getDrawer()->draw(shaderId);
	}
	else
	{
		oppPaddle->draw(shaderId);
		PaddleDrawer::getDrawer()->draw(shaderId);
		table.draw(shaderId);
	}
	//scoreBoard.draw(shaderId);
	score.draw();
}