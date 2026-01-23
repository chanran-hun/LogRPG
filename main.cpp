#include <iostream> 
#include <cstdlib>  //rand, srand
#include <ctime>    //time
#include <algorithm>    //max
using namespace std;

class Player{ 
    string name; 
    int level; 
    int hp; 
    int maxHp; 
    int atk; 
    int def; 
    int exp; 
public: 
    Player(string name, int maxHp, int atk, int def): 
    name(name), 
    level(1),
    hp(maxHp), 
    maxHp(maxHp), 
    atk(atk), 
    def(def),
    exp(0)
    {} 
    
    int getAtk()const{
        return atk;
    }

    int getDef()const{
        return def;
    }

    string getName()const{
        return name;
    }

    int getHp()const{
        return hp;
    }

    void takeDamage(int dmg){
        hp -= dmg;
        if(hp < 0) hp = 0;
    }

    bool isAlive()const{
        return hp > 0;
    }
    
    void gainEXP(int amount){
        exp += amount;
        if(exp >= 20){
            level++;
            exp -= 20;
            atk += 1;
            maxHp += 5;
            hp = maxHp;
        }
    }
    
    void printStatus(){ 
        cout << "이름: " << name << " 체력: " << hp << " 공격력: " << atk << " 경험치: " << exp << endl; 
        if(isAlive()){
            cout << "살아계시는군요" << endl;
        } else {
            cout << "ㅉㅉ" << endl;
        }
    }
}; 

class Monster{ 
    string name;  
    int hp; 
    int maxHp; 
    int atk; 
    int def;
public: 
    Monster(string name, int maxHp, int atk, int def): 
    name(name), 
    hp(maxHp), 
    maxHp(maxHp), 
    atk(atk), 
    def(def)
    {} 

    int getDef()const{
        return def;
    }

    int getAtk()const{
        return atk;
    }

    string getName()const{
        return name;
    }

    int getHp()const{
        return hp;
    }

    bool isAlive()const{
        return hp > 0;
    }

    void takeDamage(int dmg){
        hp = max(0, hp - dmg);
    }
};

bool isCritical(){
    return rand() % 100 < 20;
}

bool isDodge(){
    return rand() % 100 < 10;
}

void battle(Player &p, Monster &m){
    while(p.isAlive() && m.isAlive()){
        if(isDodge()){
            cout << "💨 " << m.getName() << "이(가) 회피했습니다!" << endl;
        } else {
            bool crit = isCritical();
            int dmg = max(1,p.getAtk()-m.getDef());
            if(crit)dmg *= 2;
                
            cout << (crit ? "★ 치명타! " : "") << p.getName() << "이(가) " << dmg << "의 피해를 입혔습니다." << endl;
            m.takeDamage(dmg);
        }
        

        if(m.isAlive()){
            if(isDodge()){
                cout << "💨 " << p.getName() << "이(가) 회피했습니다!" << endl;
            } else {
                bool critM = isCritical();
                int dmg = max(1, m.getAtk()-p.getDef());
                if(critM)dmg *= 2;
                
                cout << (critM ? "★ 치명타! " : "") << m.getName() << "이(가) " << dmg << "의 피해를 입혔습니다." << endl;
                p.takeDamage(dmg);
            }
            
        }
        cout << "[HP] " << p.getName() << ": " << p.getHp() << " / " << m.getName() << ": " << m.getHp() << endl;
    }

    if(p.isAlive()){
        cout << "승리하셨습니다." << endl;
    } else {
        cout << "패배하셨습니다." << endl;
    }
}

int main(){ 
    Player test = Player("test", 30, 5, 1); 
    Monster test1 = Monster("test1", 10, 4, 0);

    srand(time(nullptr));

    battle(test, test1);

    return 0; 
}