#include "SoundEngine.h"
#include "Defines.h"


SoundEngine *SoundEngine::INSTANCE = NULL;

SoundEngine *SoundEngine::getInstance() {	
	if(SoundEngine::INSTANCE == NULL) {
		SoundEngine::INSTANCE = new SoundEngine();	
	}

	return SoundEngine::INSTANCE;
}

SoundEngine::SoundEngine()
{
	QString soundPath(Path::projectRoot().c_str());
	soundPath+="\\Sounds\\";

	sounds[bouncing] = new QSound(soundPath + "bounce.wav");
	sounds[net] = new QSound(soundPath + "net.wav");
	sounds[paddleSound] = new QSound(soundPath + "bounce.wav");
}


void SoundEngine::play(const SoundEffect effect)
{
	sounds[effect]->play();
}
