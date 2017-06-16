#ifndef SOUND_ENGINE_H
#define SOUND_ENGINE_H

#include <QSound>
#include <map>
#include <QString>

enum SoundEffect
{
	bouncing,
	net,
	paddleSound
};

class SoundEngine  {
private:
	std::map<SoundEffect, QSound *> sounds;
	static SoundEngine *INSTANCE;
	
	SoundEngine();
	virtual ~SoundEngine(){}

public:
	void play(const SoundEffect effect);
	static SoundEngine *getInstance();
};



#endif
