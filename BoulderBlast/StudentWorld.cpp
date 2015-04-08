#include "StudentWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

///////////////// PURE VIRTUAL /////////////////////////

int StudentWorld::init()
{
    m_levelComplete = false;
    
    if(getLevel() > 99) return GWSTATUS_PLAYER_WON;
    
    ostringstream oss;
    if(getLevel() < 10) oss << "level0" << getLevel() << ".dat";
    else oss << "level" << getLevel() << ".dat";
    string curLevel = oss.str();
    
    Level lev(assetDirectory());
    Level::LoadResult result = lev.loadLevel(curLevel);
    
    if(result == Level::load_fail_file_not_found)
        return GWSTATUS_PLAYER_WON;
    
    if (result == Level:: load_fail_bad_format)
        return GWSTATUS_LEVEL_ERROR;
    
    for(int x = 0; x<VIEW_WIDTH; x++){
        for(int y = 0; y<VIEW_HEIGHT; y++){
            
            Level::MazeEntry item = lev.getContentsOf(x, y);
            
            switch (item) {
                case Level::player:
                    m_player = new Player(x,y,this);
                    break;
                case Level::wall:
                    m_actors.push_back(new Wall(x,y,this));
                    break;
                case Level::boulder:
                    m_actors.push_back(new Boulder(x,y,this));
                    break;
                case Level::hole:
                    m_actors.push_back(new Hole(x,y,this));
                    break;
                case Level::jewel:
                    m_actors.push_back(new Jewel(x,y,this));
                    m_totalJewels++;
                    break;
                case Level::extra_life:
                    m_actors.push_back(new ExtraLife(x,y,this));
                    break;
                case Level::restore_health:
                    m_actors.push_back(new RestoreHealth(x,y,this));
                    break;
                case Level::ammo:
                    m_actors.push_back(new Ammo(x,y,this));
                    break;
                case Level::exit:
                    m_actors.push_back(new Exit(x,y,this));
                    break;
                case Level::horiz_snarlbot:
                    m_actors.push_back(new SnarlBot(x,y,this,Actor::right));
                    break;
                case Level::vert_snarlbot:
                    m_actors.push_back(new SnarlBot(x,y,this,Actor::down));
                    break;
                case Level::kleptobot_factory:
                    m_actors.push_back(new KleptoBotFactory(x,y,this,IID_KLEPTOBOT));
                    break;
                case Level::angry_kleptobot_factory:
                    m_actors.push_back(new KleptoBotFactory(x,y,this,IID_ANGRY_KLEPTOBOT));
                    break;
                default:
                    break;
            }
            
        }
    }
    
    return GWSTATUS_CONTINUE_GAME;
    
}

int StudentWorld::move()
{
    updateDisplayText();
    
    m_player->doSomething();
    
    for(int i = 0; i<m_actors.size(); i++){
        m_actors[i]->doSomething();
        
        if(m_player->getHealth() <= 0){
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
        
        if(m_levelComplete){
            return GWSTATUS_FINISHED_LEVEL;
        }
    }
    
    removeDeadActors();
    
    if(m_bonus > 0) m_bonus--;
    
    if(m_player->getHealth() <= 0){
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    
    if(m_levelComplete){
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_player;
    
    while(!m_actors.empty()){
        delete m_actors[0];
        m_actors.erase(m_actors.begin());
    }
}


//////////////////////////////////////////////

StudentWorld::~StudentWorld()
{
    delete m_player;
    
    while(!m_actors.empty()){
        delete m_actors[0];
        m_actors.erase(m_actors.begin());
    }
}

void StudentWorld::addActorToVector(Actor* a)
{
    m_actors.push_back(a);
}

bool StudentWorld::isKleptoBotOnLocation(int x, int y)
{
    for(int i = 0; i<m_actors.size(); i++){
        if(m_actors[i]->type() == IID_KLEPTOBOT || m_actors[i]->type() == IID_ANGRY_KLEPTOBOT)
            if(m_actors[i]->getX() == x && m_actors[i]->getY() == y)
                return true;
    }
    
    return false;
}


Actor* StudentWorld::isKleptoBotOnGoodie(KleptoBot* kbot)
{
    for(int i = 0; i<m_actors.size(); i++){
        if(m_actors[i]->type() == IID_EXTRA_LIFE || m_actors[i]->type() == IID_RESTORE_HEALTH || m_actors[i]->type() == IID_AMMO)
            if(m_actors[i]->getX() == kbot->getX() && m_actors[i]->getY() == kbot->getY())
            {
                return m_actors[i];
            }
    }
    
    return nullptr;
}

bool StudentWorld::shouldBotFireBullet(Bot* bot)
{
    if((m_player->getY() == bot->getY()))
    {
        if(bot->getDirection() == Actor::left && m_player->getX() < bot->getX()) {
            for(int x = bot->getX()-1; x>=m_player->getX(); x--){
                Actor* temp = isActorOnLocation(x,bot->getY());
                if(temp != nullptr && (temp->type() == IID_BOULDER || temp->type() == IID_WALL || temp->type() == IID_SNARLBOT || temp->type() == IID_KLEPTOBOT || temp->type() == IID_ANGRY_KLEPTOBOT || temp->type() == IID_ROBOT_FACTORY))
                {
                    return false;
                }
            }
            return true;
        }else if(bot->getDirection() == Actor::right && m_player->getX() > bot->getX()){
            for(int x = bot->getX()+1; x<=m_player->getX(); x++){
                Actor* temp = isActorOnLocation(x,bot->getY());
                    if(temp != nullptr && (temp->type() == IID_BOULDER || temp->type() == IID_WALL || temp->type() == IID_SNARLBOT || temp->type() == IID_KLEPTOBOT || temp->type() == IID_ANGRY_KLEPTOBOT || temp->type() == IID_ROBOT_FACTORY))
                    {
                        return false;
                    }
            }
            return true;
        }
        
        return false;
    }
    
    else if((m_player->getX() == bot->getX()))
    {
        if(bot->getDirection() == Actor::down && m_player->getY() < bot->getY()) {
            for(int y = bot->getY()-1; y>=m_player->getY(); y--){
                Actor* temp = isActorOnLocation(bot->getX(),y);
                if(temp != nullptr && (temp->type() == IID_BOULDER || temp->type() == IID_WALL || temp->type() == IID_SNARLBOT || temp->type() == IID_KLEPTOBOT || temp->type() == IID_ANGRY_KLEPTOBOT || temp->type() == IID_ROBOT_FACTORY))
                {
                    return false;
                }
            }
            return true;
        }else if(bot->getDirection() == Actor::up && m_player->getY() > bot->getY()){
            for(int y = bot->getY()+1; y<=m_player->getY(); y++){
                Actor* temp = isActorOnLocation(bot->getX(),y);
                if(temp != nullptr && (temp->type() == IID_BOULDER || temp->type() == IID_WALL || temp->type() == IID_SNARLBOT || temp->type() == IID_KLEPTOBOT || temp->type() == IID_ANGRY_KLEPTOBOT || temp->type() == IID_ROBOT_FACTORY))
                {
                    return false;
                }
            }
            return true;
        }
        
        return false;
    }

    
    return false;
}

bool StudentWorld::canBotMove(int x, int y)
{
    
    if(m_player->getX() == x && m_player->getY() == y){
        return false;
    }
    
    for(int i = 0; i<m_actors.size(); i++){
        if(m_actors[i]->getX() == x && m_actors[i]->getY() == y)
        {
            if(m_actors[i]->type() == IID_BOULDER || m_actors[i]->type() == IID_WALL || m_actors[i]->type() == IID_HOLE || m_actors[i]->type() == IID_SNARLBOT || m_actors[i]->type() == IID_KLEPTOBOT || m_actors[i]->type() == IID_KLEPTOBOT || m_actors[i]->type() == IID_ROBOT_FACTORY)
            {
                return false;
            }
        }
    }
    
    return true;
}

bool StudentWorld::isPlayerAboveExit(Exit* e)
{
    if(e->isVisible() && m_player->getX() == e->getX() && m_player->getY() == e->getY())
    {
        increaseScore(m_bonus + 2000);
        m_levelComplete = true;
        return true;
    }
    
    return false;
}

bool StudentWorld::collectedAllJewels()
{
    return m_totalJewels == m_collectedJewels;
}

bool StudentWorld::isPlayerAbovePickupableItem(PickupableItem* p)
{
    if(m_player->getX() == p->getX() && m_player->getY() == p->getY()){

        increaseScore(p->getScoreValue());

        if(p->type() == IID_JEWEL)
        {
            m_collectedJewels++;
        }
        else if(p->type() == IID_EXTRA_LIFE)
        {
            incLives();
        }
        else if(p->type() == IID_RESTORE_HEALTH)
        {
            m_player->setHealth(20);
        }
        else if(p->type() == IID_AMMO)
        {
            m_player->setAmmo(m_player->getAmmo() + 20);
        }
        
        return true;
    }

    return false;
}

bool StudentWorld::canBulletMove(int x, int y, Actor* owner)
{
    // PLAYER
    if(owner->type() != IID_PLAYER){
        if(m_player->getX() == x && m_player->getY() == y)
        {
            m_player->gotHurt();
            return false;
        }
    }

    bool returnValue = true;
    // ACTORS
    for(int i = 0; i<m_actors.size(); i++){
        if(m_actors[i]->getX() == x && m_actors[i]->getY() == y)
        {
            // hit walls and robot factories, do nothing
            if(m_actors[i]->type() == IID_WALL){
                returnValue = false;
            }
            
            if(m_actors[i]->type() == IID_ROBOT_FACTORY){
                returnValue = false;
            }
            
            if (m_actors[i]->type() == IID_BOULDER){
                m_actors[i]->gotHurt();
                returnValue = false;
            }
            
            if(owner->type() != IID_SNARLBOT && m_actors[i]->type() == IID_SNARLBOT){
                m_actors[i]->gotHurt();
                if(m_actors[i]->getHealth() <= 0 && owner->type() == IID_PLAYER) increaseScore(100);
                returnValue = false;
            }
            
            if(m_actors[i]->type() == IID_KLEPTOBOT){
                m_actors[i]->gotHurt();
                if(m_actors[i]->getHealth() <= 0 && owner->type() == IID_PLAYER) increaseScore(10);
                returnValue = false;
            }
            
            if(owner->type() != IID_ANGRY_KLEPTOBOT && m_actors[i]->type() == IID_ANGRY_KLEPTOBOT){
                m_actors[i]->gotHurt();
                if(m_actors[i]->getHealth() <= 0 && owner->type() == IID_PLAYER) increaseScore(20);
                returnValue = false;
            }
            
        }
        
        
    }
    
    //if true, go through stuff
    return returnValue;
    
}

bool StudentWorld::isBoulderAboveHole(Hole* h)
{
    for(int i = 0; i<m_actors.size(); i++){
          
        if(m_actors[i]->type() == IID_BOULDER && m_actors[i]->getX() == h->getX() && m_actors[i]->getY() == h->getY())
        {
            m_actors[i]->setHealth(0);
            return true;
        }
    }
    return false;
}

bool StudentWorld::canBoulderMove(int x, int y)
{
    Actor *actor = isActorOnLocation(x, y);

    if(actor != nullptr){
        if(actor->type() == IID_HOLE) return true;
        else return false;
    }

    return true;
}

bool StudentWorld::canPlayerMove(int x, int y)
{
    bool blocksPlayer = false;
    
     for(int i = 0; i<m_actors.size(); i++){
    
        if(m_actors[i]->getX() == x && m_actors[i]->getY() == y){
            if(m_actors[i]->type() == IID_BOULDER){
                
                Actor::Direction pDirection = m_player->getDirection();
                switch (pDirection) {
                    case Actor::up:
                        if(canBoulderMove(x, y+1))
                            m_actors[i]->moveTo(x, y+1);
                        break;
                    case Actor::down:
                        if(canBoulderMove(x, y-1))
                            m_actors[i]->moveTo(x, y-1);
                        break;
                    case Actor::left:
                        if(canBoulderMove(x-1, y))
                            m_actors[i]->moveTo(x-1, y);
                        break;
                    case Actor::right:
                        if(canBoulderMove(x+1, y))
                            m_actors[i]->moveTo(x+1, y);
                        break;
                    default:
                        break;
                }
            }
            
         }
    
        // All other actors
        if(m_actors[i]->getX() == x && m_actors[i]->getY() == y)
        {
            if(m_actors[i]->blocksPlayer()) blocksPlayer = true;
        }
     }
    
    if(blocksPlayer) return false;
    
    return true;
}

///////////////   PRIVATE FUNCTIONS   ////////////////////////

void StudentWorld::updateDisplayText()
{
    
    ostringstream oss;
    
    int score = getScore();
    oss.fill('0');
    oss << "Score: " << setw(7) << score;
    oss << "  ";
    
    int level = getLevel();
    oss.fill('0');
    oss << "Level: " << setw(2) << level;
    oss << "  ";
    
    int lives = getLives();
    oss.fill(' ');
    oss << "Lives: " << setw(2) << lives;
    oss << "  ";
    
    int health =  ((double)(m_player->getHealth())/20) * 100;
    oss.fill(' ');
    oss << "Health: " << setw(3) << health;
    oss << "%" << "  ";
    
    int ammo = m_player->getAmmo();
    oss.fill(' ');
    oss << "Ammo: " << setw(3) << ammo;
    oss << "  ";
    
    int bonus = m_bonus;
    oss.fill(' ');
    oss << "Bonus: " << setw(4) << bonus;
    oss << "  ";
    
    setGameStatText(oss.str());
}

void StudentWorld::removeDeadActors()
{
    for(int i = 0; i<m_actors.size(); i++){
        Alive *currentActor = dynamic_cast<Alive*>(m_actors[i]);
        
        if(currentActor != nullptr && currentActor->getHealth() <= 0){
            currentActor->setVisible(false);
            delete m_actors[i];
            m_actors.erase(m_actors.begin()+i);
            continue;
        }
    }
}


Actor* StudentWorld::isActorOnLocation(int x, int y)
{
    for(int i = 0; i<m_actors.size(); i++){
        if(m_actors[i]->getX() == x && m_actors[i]->getY() == y)
            return m_actors[i];
    }
    
    return nullptr;
}

