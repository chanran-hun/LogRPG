#include <iostream> 
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
    
    void takeDamage(int dmg){
        hp -= dmg;
        cout << "맞았어요... 상태창 켜보세요" << endl;
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
};

int main(){ 
    Player test = Player("test", 30, 5, 3); 
    test.printStatus();
    test.takeDamage(1);
    test.printStatus();
    test.gainEXP(20);
    test.takeDamage(35);
    test.printStatus();
    return 0; 
}