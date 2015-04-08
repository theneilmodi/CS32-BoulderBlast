#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
	 : GameWorld(assetDir)
	{
	}
    
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    virtual ~StudentWorld();

    void addActorToVector(Actor* a);
    
    // bot functions
    bool isKleptoBotOnLocation(int x, int y);
    Actor* isKleptoBotOnGoodie(KleptoBot* kbot);
    bool shouldBotFireBullet(Bot* bot);
    bool canBotMove(int x, int y);
    
    bool isPlayerAboveExit(Exit* e);
    
    bool collectedAllJewels();
    
    bool isPlayerAbovePickupableItem (PickupableItem* p);
    
    bool canBulletMove(int x, int y, Actor* owner);
    
    bool isBoulderAboveHole(Hole* h);
    
    bool canBoulderMove(int x, int y);
    
    bool canPlayerMove(int x, int y);
    
private:
    std::vector<Actor*> m_actors;
    Player *m_player;
    int m_bonus = 1000;
    int m_totalJewels = 0;
    int m_collectedJewels = 0;
    bool m_levelComplete = false;
    
    void updateDisplayText();
    void removeDeadActors();
    Actor* isActorOnLocation(int x, int y);

};

#endif // STUDENTWORLD_H_
