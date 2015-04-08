#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"
#include <vector>
class StudentWorld;

// ACTOR
class Actor: public GraphObject{
    
public:
    Actor(int imageID, int startX, int startY, StudentWorld* world, Direction dir = none);
    StudentWorld* getWorld(){
        return m_world;
    }
    virtual void setHealth(int health){}
    virtual int getHealth(){return 0;}
    virtual bool blocksPlayer(){
        return true;
    }
    virtual void gotHurt(){}
    virtual void doSomething() = 0;
    virtual int type() = 0;
    
private:
    StudentWorld* m_world;
};


// ALIVE
class Alive : public Actor{

public:
    Alive(int imageID, int startX, int startY, StudentWorld* world,Direction dir = none);
    void setHealth(int health){
        m_health = health;
    }
    int getHealth(){
        return m_health;
    }
    virtual void doSomething() = 0;
    virtual int type() = 0;
    
private:
    int m_health;
};


// PLAYER
class Player : public Alive{
    
public:
    Player(int startX, int startY, StudentWorld* world);
    virtual void doSomething();
    virtual void gotHurt();
    int getAmmo(){
        return m_ammo;
    }
    void setAmmo(int a){
        m_ammo = a;
    }
    virtual int type(){
        return IID_PLAYER;
    }
    
    
private:
    int m_ammo;
};


// WALL
class Wall : public Actor{
    
public:
    Wall(int startX, int startY, StudentWorld* world);
    virtual void doSomething(){
        
    }
    virtual int type(){
        return IID_WALL;
    }
};


// BOULDER
class Boulder : public Alive{
    
public:
    Boulder(int startX, int startY, StudentWorld* world);
    virtual void doSomething(){
        
    }
    virtual void gotHurt(){
        setHealth(getHealth()-2);
    }
    virtual int type(){
        return IID_BOULDER;
    }
};


// BULLET
class Bullet : public Alive{
    
public:
    Bullet(int startX, int startY, StudentWorld* world, Direction dir, Actor* owner);
    virtual void doSomething();
    virtual bool blocksPlayer(){
        return false;
    }
    virtual int type(){
        return IID_BULLET;
    }
private:
    Actor* m_owner;
};

// HOLE
class Hole : public Alive{
    
public:
    Hole(int startX, int startY, StudentWorld* world);
    virtual void doSomething();
    virtual int type(){
        return IID_HOLE;
    }
};

// PICKUPABLE ITEM
class PickupableItem : public Alive{
public:
    PickupableItem (int imageID, int startX, int startY, StudentWorld* world, int scoreVal);
    virtual void doSomething();
    int getScoreValue(){
        return m_scoreValue;
    }
    virtual bool blocksPlayer(){
        return false;
    }
    virtual int type() = 0;
private:
    int m_scoreValue;
};

// JEWEL
class Jewel : public PickupableItem {
public:
    Jewel(int startX, int startY, StudentWorld* world);
    virtual int type(){
        return IID_JEWEL;
    }
};

// Extra Life
class ExtraLife : public PickupableItem {
public:
    ExtraLife(int startX, int startY, StudentWorld* world);
    virtual int type(){
        return IID_EXTRA_LIFE;
    }
};

// Restore Health
class RestoreHealth : public PickupableItem {
public:
    RestoreHealth(int startX, int startY, StudentWorld* world);
    virtual int type(){
        return IID_RESTORE_HEALTH;
    }
};

// Ammo
class Ammo : public PickupableItem {
public:
    Ammo(int startX, int startY, StudentWorld* world);
    virtual int type(){
        return IID_AMMO;
    }
};

// Exit
class Exit : public Actor{
public:
    Exit(int startX, int startY, StudentWorld* world);
    virtual void doSomething();
    virtual bool blocksPlayer(){
        return false;
    }
    virtual int type(){
        return IID_EXIT;
    }
};

// Bot
class Bot : public Alive{
public:
    Bot(int imageID, int startX, int startY, StudentWorld* world, Direction dir);
    virtual void gotHurt();
    int getTicks(){
        return m_ticks;
    }
    int getCurrentTick(){
        return m_currentTick;
    }
    void setCurrentTick(int n){
        m_currentTick = n;
    }

    virtual void doSomething() = 0;
    virtual int type() = 0;
private:
    int m_ticks;
    int m_currentTick;

};


// SnarlBot
class SnarlBot : public Bot{
public:
    SnarlBot(int startX, int startY, StudentWorld* world, Direction dir);
    virtual void doSomething();
    virtual int type(){
        return IID_SNARLBOT;
    }  
};


// KleptoBot
class KleptoBot : public Bot{
public:
    KleptoBot(int imageID, int startX, int startY, StudentWorld* world);
    virtual void doSomething();
    virtual void gotHurt();
    void dropBackGoodie();
    void move();
    int generateUniqueRandoms(std::vector<int> randDirNumbers);
    
    int getPickedGoodie(){
        return m_pickedGoodie;
    }
    void setPickedGoodie(int goo){
        m_pickedGoodie = goo;
    }
    virtual int type(){
        return IID_KLEPTOBOT;
    }

private:
    int m_distanceBeforeTurning;
    int m_walkedDistance;
    int m_pickedGoodie;
    
};

// Angry Kleptobot
class AngryKleptoBot : public KleptoBot{
public:
    AngryKleptoBot(int imageID, int startX, int startY, StudentWorld* world);
    virtual void doSomething();
    virtual int type(){
        return IID_ANGRY_KLEPTOBOT;
    }
};

// Kleptobot Factory
class KleptoBotFactory : public Actor{
public:
    KleptoBotFactory(int startX, int startY, StudentWorld* world, int kbotType);
    virtual void doSomething();
    virtual int type(){
        return IID_ROBOT_FACTORY;
    }
private:
    int m_kbotToProduce;
};



#endif // ACTOR_H_
