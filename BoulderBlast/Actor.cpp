#include "Actor.h"

#include "StudentWorld.h"
#include <iostream>
#include <cstdlib>
using namespace std;

// ACTOR
Actor::Actor(int imageID, int startX, int startY, StudentWorld *world, Direction dir):GraphObject(imageID, startX, startY, dir)
{
    m_world = world;
    setVisible(true);
}


//ALIVE
Alive::Alive(int imageID, int startX, int startY, StudentWorld* world,Direction dir) : Actor(imageID, startX, startY, world, dir)
{
    
}

// PLAYER
Player::Player(int startX, int startY, StudentWorld* world) : Alive(IID_PLAYER, startX, startY, world, right)
{
    setHealth(20);
    m_ammo = 20;
    setDirection(right);
}

void Player::doSomething()
{
    if(getHealth() == 0){
        return;
    }else{
        int ch;
        if(getWorld()->getKey(ch)){
            switch (ch) {
                case KEY_PRESS_ESCAPE:
                {
                    setHealth(0);
                    break;
                }
                    
                case KEY_PRESS_SPACE:
                {
                    if(m_ammo > 0){
                        Bullet *b = new Bullet(getX(), getY(), getWorld(), getDirection(), this);
                        getWorld()->addActorToVector(b);
                        m_ammo--;
                        getWorld()->playSound(SOUND_PLAYER_FIRE);
                    }
                    break;
                }
                    
                case KEY_PRESS_LEFT:
                {
                    setDirection(left);
                    if(getWorld()->canPlayerMove(getX()-1,getY()))
                        moveTo(getX()-1, getY());
                    break;
                }
                
                case KEY_PRESS_RIGHT:
                {
                    setDirection(right);
                    if(getWorld()->canPlayerMove(getX()+1,getY()))
                        moveTo(getX()+1, getY());
                    break;
                }

                case KEY_PRESS_UP:
                {
                    setDirection(up);
                    if(getWorld()->canPlayerMove(getX(),getY()+1))
                        moveTo(getX(), getY()+1);
                    break;
                }

                case KEY_PRESS_DOWN:
                {
                    setDirection(down);
                    if(getWorld()->canPlayerMove(getX(),getY()-1))
                        moveTo(getX(), getY()-1);
                    break;
                }
                    
                default:
                {
                    break;
                }
            }
        }
    }
}

void Player::gotHurt(){

    setHealth(getHealth()-2);
    
    if(getHealth() > 0){
        getWorld()->playSound(SOUND_PLAYER_IMPACT);
    }else{
        getWorld()->playSound(SOUND_PLAYER_DIE);
    }
        
};

// WALL
Wall::Wall(int startX, int startY, StudentWorld* world) : Actor(IID_WALL, startX, startY, world, none)
{
    
}


// Boulder
Boulder::Boulder(int startX, int startY, StudentWorld* world) : Alive(IID_BOULDER, startX, startY, world, none)
{
    setHealth(10);
}


// Bullet
Bullet::Bullet(int startX, int startY, StudentWorld* world, Direction dir, Actor* owner) : Alive(IID_BULLET, startX, startY, world, dir)
{
    setHealth(1);
    m_owner = owner;
}

void Bullet::doSomething()
{
    if(getHealth() == 0) return;
    else{
        getWorld()->canBulletMove(getX(), getY(), m_owner);
        switch (getDirection()) {
            case up:
                if(getWorld()->canBulletMove(getX(), getY()+1, m_owner)){
                    moveTo(getX(), getY()+1);
                }else setHealth(0);
                
                break;
            case down:
                if(getWorld()->canBulletMove(getX(), getY()-1, m_owner)){
                    moveTo(getX(), getY()-1);
                }else setHealth(0);

                break;
            case left:
                if(getWorld()->canBulletMove(getX()-1, getY(), m_owner)){
                    moveTo(getX()-1, getY());
                }else setHealth(0);

                break;
            case right:
                if(getWorld()->canBulletMove(getX()+1, getY(), m_owner)){
                    moveTo(getX()+1, getY());
                }else setHealth(0);

                break;
            default:
                break;
        }
    }
}

// Hole
Hole::Hole(int startX, int startY, StudentWorld* world) : Alive(IID_HOLE, startX, startY, world, none)
{
    setHealth(1);
}

void Hole::doSomething()
{
    if(getHealth() == 0) return;
    else{
        if(getWorld()->isBoulderAboveHole(this)){
            setHealth(0);
        }
    }
}

PickupableItem::PickupableItem (int imageID, int startX, int startY, StudentWorld* world, int scoreVal) : Alive(imageID, startX, startY, world, none)
{
    setHealth(1);
    m_scoreValue = scoreVal;
}

void PickupableItem::doSomething()
{
    if(getHealth() == 0) return;
    else{
        if(getWorld()->isPlayerAbovePickupableItem(this)){
            getWorld()->playSound(SOUND_GOT_GOODIE);
            setHealth(0);
        }
    }
}

// JEWEL
Jewel::Jewel(int startX, int startY, StudentWorld* world) : PickupableItem (IID_JEWEL, startX, startY, world, 50)
{

}

// Extra Life
ExtraLife::ExtraLife(int startX, int startY, StudentWorld* world) : PickupableItem (IID_EXTRA_LIFE, startX, startY, world, 1000)
{

}

// Restore Health
RestoreHealth::RestoreHealth(int startX, int startY, StudentWorld* world) : PickupableItem (IID_RESTORE_HEALTH, startX, startY, world, 500)
{

}

// Ammo
Ammo::Ammo(int startX, int startY, StudentWorld* world) : PickupableItem (IID_AMMO, startX, startY, world, 100)
{
    
}

// Exit
Exit::Exit(int startX, int startY, StudentWorld* world) : Actor(IID_EXIT, startX, startY, world, none)
{
    setVisible(false);
}

void Exit::doSomething()
{
    if(getWorld()->collectedAllJewels()){
        if(!isVisible()){
            setVisible(true);
            getWorld()->playSound(SOUND_REVEAL_EXIT);
        }
    }
    
    if(isVisible()){
        if(getWorld()->isPlayerAboveExit(this)){
            getWorld()->playSound(SOUND_FINISHED_LEVEL);
        }
    }
}

// Bot
Bot::Bot(int imageID, int startX, int startY, StudentWorld* world, Direction dir) : Alive(imageID, startX, startY, world, dir)
{
    m_currentTick = 1;
    
    m_ticks = (28-getWorld()->getLevel()) / 4;
    if(m_ticks < 3)
        m_ticks = 3;
}

void Bot::gotHurt()
{
    setHealth(getHealth()-2);
    if(getHealth() > 0)
        getWorld()->playSound(SOUND_ROBOT_IMPACT);
    else{
        getWorld()->playSound(SOUND_ROBOT_DIE);
    }
}


// SnarlBot
SnarlBot::SnarlBot(int startX, int startY, StudentWorld* world, Direction dir) : Bot(IID_SNARLBOT, startX, startY, world, dir)
{
    setHealth(10);
}

void SnarlBot::doSomething()
{
    if(getHealth() <= 0) return;
    else if(getCurrentTick() == getTicks()){
        if(getWorld()->shouldBotFireBullet(this)){
            Bullet *b = new Bullet(getX(), getY(), getWorld(), getDirection(), this);
            getWorld()->addActorToVector(b);
            getWorld()->playSound(SOUND_ENEMY_FIRE);
        }else{
                switch (getDirection()) {
                    case up:
                        if(getWorld()->canBotMove(getX(), getY()+1))
                            moveTo(getX(), getY()+1);
                        else
                            setDirection(down);
                        break;
                    case down:
                        if(getWorld()->canBotMove(getX(), getY()-1))
                            moveTo(getX(), getY()-1);
                        else
                            setDirection(up);
                        break;
                    case left:
                        if(getWorld()->canBotMove(getX()-1, getY()))
                            moveTo(getX()-1, getY());
                        else
                            setDirection(right);
                        break;
                    case right:
                        if(getWorld()->canBotMove(getX()+1, getY()))
                            moveTo(getX()+1, getY());
                        else
                            setDirection(left);
                        break;
                    default:
                        break;
                }
            
        }
        
        setCurrentTick(1);
    }else{
        setCurrentTick(getCurrentTick()+1);
    }
}


// Kleptobot
KleptoBot::KleptoBot(int imageID, int startX, int startY, StudentWorld* world) : Bot(imageID, startX, startY, world, right)
{
    setHealth(5);
    
    m_walkedDistance = 0;
    m_distanceBeforeTurning = (rand() % 6) + 1;
    m_pickedGoodie = 0;
    
}

void KleptoBot::doSomething()
{
    if(getHealth() <= 0){
        return;
    }else if(getCurrentTick() == getTicks()){
        // does not include jewels
        Actor* goodie = getWorld()->isKleptoBotOnGoodie(this);
        
        //pick up goodie, does not incldue jewels
        if(goodie != nullptr && m_pickedGoodie == 0){
            int random = (rand() % 10) + 1;
            if(random == 1){
                m_pickedGoodie = goodie->type();
                goodie->setHealth(0);
                getWorld()->playSound(SOUND_ROBOT_MUNCH);
            }else{
                move();
            }
        }else{
            move();
        }

        setCurrentTick(1);
    }else{
        setCurrentTick(getCurrentTick()+1);
    }
    
}

void KleptoBot::gotHurt()
{
    Bot::gotHurt();
    
    if(getHealth() <= 0){
        if(m_pickedGoodie != 0){
            dropBackGoodie();
        }
    }
}

void KleptoBot::dropBackGoodie()
{
    if(m_pickedGoodie == IID_RESTORE_HEALTH){
        RestoreHealth *droppedGoodie = new RestoreHealth(getX(),getY(),getWorld());
        getWorld()->addActorToVector(droppedGoodie);
    }else if(m_pickedGoodie == IID_EXTRA_LIFE){
        ExtraLife *droppedGoodie = new ExtraLife(getX(),getY(),getWorld());
        getWorld()->addActorToVector(droppedGoodie);
    }else if(m_pickedGoodie == IID_AMMO){
        Ammo *droppedGoodie = new Ammo(getX(),getY(),getWorld());
        getWorld()->addActorToVector(droppedGoodie);
    }
}

void KleptoBot::move()
{
    bool foundObstruction = false;
    if(m_walkedDistance != m_distanceBeforeTurning){

        switch (getDirection()) {
            case up:
                if(getWorld()->canBotMove(getX(), getY()+1)){
                    moveTo(getX(), getY()+1);
                    m_walkedDistance++;
                    return;
                } else foundObstruction = true;
                break;
            case down:
                if(getWorld()->canBotMove(getX(), getY()-1)){
                    moveTo(getX(), getY()-1);
                    m_walkedDistance++;
                    return;
                } else foundObstruction = true;
                break;
            case left:
                if(getWorld()->canBotMove(getX()-1, getY())){
                    moveTo(getX()-1, getY());
                    m_walkedDistance++;
                    return;
                } else foundObstruction = true;
                break;
            case right:
                if(getWorld()->canBotMove(getX()+1, getY())){
                    moveTo(getX()+1, getY());
                    m_walkedDistance++;
                    return;
                } else foundObstruction = true;
                break;
            default:
                break;
        }
        
    }
    
    if(m_walkedDistance == m_distanceBeforeTurning || foundObstruction)
    {
        
        m_distanceBeforeTurning = (rand() % 6) + 1;
        m_walkedDistance = 0;
        int randomDirNum = (rand() % 4) + 1;
        std::vector<int> randomVector;
        
        int firstLookedDirNum = randomDirNum;
        bool finallyMoved = false;
        
        for(int check = 0; check < 4; check++){
            
            //up
            if(randomDirNum == 1) {
                if(getWorld()->canBotMove(getX(), getY()+1)){
                    setDirection(up);
                    moveTo(getX(), getY()+1);
                    finallyMoved = true;
                    break;
                } else{
                    randomVector.push_back(randomDirNum);
                    randomDirNum = generateUniqueRandoms(randomVector);
                }
            }
            
            // down
            else if(randomDirNum == 2) {
                if(getWorld()->canBotMove(getX(), getY()-1)){
                    setDirection(down);
                    moveTo(getX(), getY()-1);
                    finallyMoved = true;
                    break;
                } else{
                    randomVector.push_back(randomDirNum);
                    randomDirNum = generateUniqueRandoms(randomVector);
                }
            }
            
            // left
            else if(randomDirNum == 3){
                if(getWorld()->canBotMove(getX()-1, getY())){
                    setDirection(left);
                    moveTo(getX()-1, getY());
                    finallyMoved = true;
                    break;
                } else{
                    randomVector.push_back(randomDirNum);
                    randomDirNum = generateUniqueRandoms(randomVector);
                }
            }
            
            // right
            else if(randomDirNum == 4){
                if(getWorld()->canBotMove(getX()+1, getY())){
                    setDirection(right);
                    moveTo(getX()+1, getY());
                    finallyMoved = true;
                    break;
                } else{
                    randomVector.push_back(randomDirNum);
                    randomDirNum = generateUniqueRandoms(randomVector);
                }
            }
        }
        
        
        if(finallyMoved){
            m_walkedDistance++;
        }else{
            switch (firstLookedDirNum) {
                case 1:
                    setDirection(up);
                    break;
                case 2:
                    setDirection(down);
                    break;
                case 3:
                    setDirection(left);
                    break;
                case 4:
                    setDirection(right);
                    break;
                default:
                    break;
            }
        }
        
    } // END SECOND IF

}


int KleptoBot::generateUniqueRandoms(vector<int> randDirNumbers)
{
    if(randDirNumbers.size() == 4){
        return -1;
    }
    
    bool foundARandom = false;
    while (!foundARandom){
        int somethingSimilar = 0;
        int tempRand = (rand() % 4) + 1;
        
        for(int i = 0; i<randDirNumbers.size(); i++){
            if(tempRand == randDirNumbers[i])
                somethingSimilar++;
        }
        
        if(somethingSimilar == 0){
            foundARandom = true;
            return tempRand;
        }
    }
    
    return -1;
}

// Angry Kleptobot
AngryKleptoBot::AngryKleptoBot(int imageID, int startX, int startY, StudentWorld* world) : KleptoBot(imageID, startX, startY, world)
{
    setHealth(8);
}

void AngryKleptoBot::doSomething()
{

    if(getHealth() <= 0){
        return;
    }else if(getCurrentTick() == getTicks()){
        
        // fire bullet
        if(getWorld()->shouldBotFireBullet(this)){
            Bullet *b = new Bullet(getX(), getY(), getWorld(), getDirection(), this);
            getWorld()->addActorToVector(b);
            getWorld()->playSound(SOUND_ENEMY_FIRE);
        
        //pick up goodie, does not include jewels
        }else if(getWorld()->isKleptoBotOnGoodie(this) != nullptr && getPickedGoodie() == 0){
            int random = (rand() % 10) + 1;
            if(random == 1){
                setPickedGoodie(getWorld()->isKleptoBotOnGoodie(this)->type());
                getWorld()->isKleptoBotOnGoodie(this)->setHealth(0);
                getWorld()->playSound(SOUND_ROBOT_MUNCH);
            }else{
                move();
            }
        }else{
            move();
        }
        
        setCurrentTick(1);
    }else{
        setCurrentTick(getCurrentTick()+1);
    }
    
}

// KBOT Factory
KleptoBotFactory::KleptoBotFactory(int startX, int startY, StudentWorld* world, int kbotType) : Actor(IID_ROBOT_FACTORY, startX, startY, world, none)
{
    m_kbotToProduce = kbotType;
}

void KleptoBotFactory::doSomething()
{
    int x = getX();
    int y = getY();
    int kbots = 0;
    
    //look up
    int i = 1;
    while((y-i)>=0 && i<=3){
        if(getWorld()->isKleptoBotOnLocation(x, y-i))
        {
            kbots++;
        }
        i++;
    }
    
    //look down
    i = 1;
    while((y+i)<VIEW_HEIGHT && i<=3){
        if(getWorld()->isKleptoBotOnLocation(x, y+i))
        {
            kbots++;
        }
        i++;
    }
    
    //look left
    i = 1;
    while((x-i)>=0 && i<=3){
        if(getWorld()->isKleptoBotOnLocation(x-i, y))
        {
            kbots++;
        }
        i++;
    }

    
    //look right
    i = 1;
    while((x+i)<VIEW_WIDTH && i<=3){
        if(getWorld()->isKleptoBotOnLocation(x+i, y))
        {
            kbots++;
        }
        i++;
    }
    

    if(kbots < 3 && getWorld()->isKleptoBotOnLocation(getX(), getY()) == false){
        int randomNum = (rand() % 50) + 1;
        if(randomNum == 1){
            if(m_kbotToProduce == IID_KLEPTOBOT){
                KleptoBot *kbot = new KleptoBot(IID_KLEPTOBOT,x, y, getWorld());
                getWorld()->addActorToVector(kbot);
                getWorld()->playSound(SOUND_ROBOT_BORN);
            }else if(m_kbotToProduce == IID_ANGRY_KLEPTOBOT){
                AngryKleptoBot *kbot = new AngryKleptoBot(IID_ANGRY_KLEPTOBOT,x, y, getWorld());
                getWorld()->addActorToVector(kbot);
                getWorld()->playSound(SOUND_ROBOT_BORN);
            }
        }
    }
}