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
    
    void increaseAtk(int amount = 1){
        atk += amount;
        cout << "공격력이 +" << amount << " 증가했습니다!\n";
    }

    void increaseDef(int amount = 1){
        def += amount;
        cout << "방어력이 +" << amount << " 증가했습니다!\n";
    }

    void increaseMaxHp(int amount = 5){
        maxHp += amount;
        hp = maxHp;
        cout << "최대 체력이 +" << amount << " 증가하고 체력이 회복되었습니다!\n";
    }

    void gainEXP(int amount){
        exp += amount;
        while(exp >= 20){
            level++;
            exp -= 20;
            atk += 1;
            maxHp += 5;
            hp = maxHp;
            cout << "\n=== 레벨 업! LV." << level << " ===\n" << endl;
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
            cout << "💨 " << m.getName() << "이(가) " << p.getName() << "의 공격을 회피했습니다!" << endl;
        } else {
            bool critP = isCritical();
            int dmgP = max(1,p.getAtk()-m.getDef());
            if(critP)dmgP *= 2;
                
            cout << (critP ? "★ 치명타! " : "") << p.getName() << "이(가) " << dmgP << "의 피해를 입혔습니다." << endl;
            m.takeDamage(dmgP);
        }
        

        if(m.isAlive()){
            if(isDodge()){
                cout << "💨 " << p.getName() << "이(가) " << m.getName() << "의 공격을 회피했습니다!" << endl;
            } else {
                bool critM = isCritical();
                int dmgM = max(1, m.getAtk()-p.getDef());
                if(critM)dmgM *= 2;
                
                cout << (critM ? "★ 치명타! " : "") << m.getName() << "이(가) " << dmgM << "의 피해를 입혔습니다." << endl;
                p.takeDamage(dmgM);
            }
            
        }
        cout << "[HP] " << p.getName() << ": " << p.getHp() << " / " << m.getName() << ": " << m.getHp() << endl;
    }

    if(p.isAlive()){
        cout << "승리하셨습니다." << endl;

        int reward = 10; 
        cout << "경험치 +" << reward << "!" << endl;
        p.gainEXP(reward);
    } else {
        cout << "패배하셨습니다." << endl;
    }
}

function rewardText(int rewardIndex) -> string:
    switch rewardIndex:
        case 0: return "공격력 +1";
        case 1: return "방어력 +1";
        case 2: return "최대 체력 +5";

void chooseReward(Player& p){
    // 1) 보상 후보 인덱스 목록
    idx = [0,1,2];

    // 2) 인덱스 랜덤하게 섞기
    shuffle(idx);
    
    // 3) 섞인것 중 앞 두가지 선택
    pick1 = idx[0];
    pick2 = idx[1]; 

    // 4) 화면에 선택 목록 추가
    print "보상을 선택하세요";
    print "1) " + rewardText(pick1);
    print "2) " + rewardText(pick2);

    // 5) 사용자 입력 받기
    input choice;

    // 6) choice를 실제 보상 인덱스로 변환
    if choice == 1:
        chosen = pick1;
    else if choice == 2:
        chosen = pick2;
    else :
        (잘못입력 처리: 다시 입력받거나 기본값)

    // 7) chosen에 따라 효과 적용
    applyReward(p,chosen);


    cout << "\n보상을 선택하세요:\n";
    cout << "1) 공격력 +1\n";
    cout << "2) 방어력 +1\n";
    cout << "3) 최대 체력 +5\n";
    cout << "> ";

    int choice;
    cin >> choice;

    switch(choice){
        case 1: p.increaseAtk(); break;
        case 2: p.increaseDef(); break;
        case 3: p.increaseMaxHp(); break;
        default:
            cout << "잘못된 선택입니다. 기본 보상으로 공격력 +1을 받습니다.\n";
            p.increaseAtk();
    }

    cout << "[STAT] ATK:" << p.getAtk() << " DEF:" << p.getDef() << " HP:" << p.getHp() << endl;
}

Monster makeMonster(int stage){
    int baseHp = 30 + 5*stage;
    int baseAtk = 6 + stage*2;
    int baseDef = 2 + stage/2;

    return Monster("슬라임", baseHp, baseAtk, baseDef);
}

int main(){ 
    srand(time(nullptr));

    Player p = Player("Hero", 60, 14, 4);

    int stage = 1;

    while(p.isAlive()){
        cout << "\n==== STAGE " << stage << " ====\n";

        Monster m = makeMonster(stage);
        battle(p,m);

        if(!p.isAlive()) break;

        chooseReward(p);
        stage++;
    }

    cout << "\n=== 게임종료 ===\n";
    cout << "클리어한 스테이지: " << stage-1 << endl;

    return 0; 
}