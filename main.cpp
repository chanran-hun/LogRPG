#include <iostream>
#include <algorithm>    //max, shuffle, clamp
#include <vector>
#include <random>
#include <string>
#include <limits>
using namespace std;

namespace Balance {
    constexpr double PLAYER_CRIT  = 0.20;
    constexpr double PLAYER_DODGE = 0.10;
    constexpr double MONSTER_CRIT = 0.20;
    constexpr double MONSTER_DODGE= 0.10;
    constexpr int MONSTER_HP_SCALE = 3;
    constexpr int MONSTER_ATK_DIV = 2;
    constexpr int MONSTER_DEF_DIV = 3;
}

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

bool rollChance(mt19937& rng, double p){
    p = clamp(p, 0.0, 1.0); //안전장치
    bernoulli_distribution d(p);
    return d(rng);
}

string rewardText(int rewardIndex) {
    switch (rewardIndex){
        case 0: return "공격력 +1";
        case 1: return "방어력 +1";
        case 2: return "최대 체력 +5";
        default: return "알 수 없는 보상";
    }
        
}

void applyReward(Player& p, int rewardIndex){
    switch (rewardIndex) {
        case 0: p.increaseAtk(); break;
        case 1: p.increaseDef(); break;
        case 2: p.increaseMaxHp(); break;
        default: break;
    }
        
}

void chooseReward(Player& p, mt19937& rng){
    // 1) 보상 후보 인덱스 목록
    vector<int> idx = {0,1,2};

    // 2) 인덱스 랜덤하게 섞기
    shuffle(idx.begin(), idx.end() ,rng);
    
    // 3) 섞인것 중 앞 두가지 선택
    int pick1 = idx[0];
    int pick2 = idx[1]; 

    // 4) 화면에 선택 목록 추가
    cout << "===보상을 선택하세요===\n";
    cout << "1) " << rewardText(pick1) << "\n";
    cout << "2) " << rewardText(pick2) << "\n> ";

    // 5) 사용자 입력 받기
    int choice = 0;
    cin >> choice;

    //잘못된 입력의 경우
    if(cin.fail()){
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "숫자만 입력해 주세요. 기본값으로 1번을 선택합니다.\n";
        choice = 1;
    }

    // 6) choice를 실제 보상 인덱스로 변환
    int chosen = 0;
    if (choice == 1) {
        chosen = pick1;
    } else if (choice == 2 ){
        chosen = pick2;
    } else {
        cout << "잘못된 번호입니다. 기본값으로 1번 선택지를 선택하겠습니다\n"; 
        chosen = pick1;
    }

    // 7) chosen에 따라 효과 적용
    applyReward(p,chosen);

    cout << "[STAT] ATK:" << p.getAtk() << " DEF:" << p.getDef() << " HP:" << p.getHp() << endl;
}

void battle(Player &p, Monster &m, mt19937& rng){
    while(p.isAlive() && m.isAlive()){
        cout << "----\n";
        if(rollChance(rng, Balance::MONSTER_DODGE)){
            cout << "💨 " << m.getName() << "이(가) " << p.getName() << "의 공격을 회피했습니다!" << endl;
        } else {
            bool critP = rollChance(rng, Balance::PLAYER_CRIT);
            int dmgP = max(1,p.getAtk()-m.getDef());
            if(critP)dmgP *= 2;
                
            cout << (critP ? "★ 치명타! " : "") << p.getName() << "이(가) " << dmgP << "의 피해를 입혔습니다." << endl;
            m.takeDamage(dmgP);
        }
        

        if(m.isAlive()){
            if(rollChance(rng, Balance::PLAYER_DODGE)){
                cout << "💨 " << p.getName() << "이(가) " << m.getName() << "의 공격을 회피했습니다!" << endl;
            } else {
                bool critM = rollChance(rng, Balance::MONSTER_CRIT);
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
        chooseReward(p,rng);
    } else {
        cout << "패배하셨습니다." << endl;
    }
}

struct MonsterTemplate{
    string name;
    int baseHp;
    int baseAtk;
    int baseDef;
};

vector<MonsterTemplate> earlyMonsters = {
    {"슬라임", 25, 6, 1},
    {"들쥐", 18, 7, 0}
};

vector<MonsterTemplate> midMonsters = {
    {"고블린", 30, 9, 2},
    {"늑대", 26, 11, 1}
};

vector<MonsterTemplate> lateMonsters = {
    {"오크", 40, 13, 3},
    {"스켈레톤", 34, 12, 4}
};

vector<MonsterTemplate> bossMonsters = {
    {"케렐라", 50, 15, 5},
    {"케바풀", 55, 8, 10}
};

Monster makeMonster(int stage, mt19937& rng){
    // 1)stage에 따라 풀 고르기
    const vector<MonsterTemplate>* pool;
    if(stage <= 3) pool = &earlyMonsters;
    else if (stage <= 6) pool = &midMonsters;
    else pool = &lateMonsters;

    if(stage % 5 == 0) pool = &bossMonsters;

    // 2)풀에서 랜덤으로 하나 뽑기
    uniform_int_distribution<int> dist(0, (int)pool->size() - 1);
    const MonsterTemplate& t = (*pool)[dist(rng)];

    // 3)스테이지 보정
    int hp = t.baseHp + stage * Balance::MONSTER_HP_SCALE;
    int atk = t.baseAtk + stage / Balance::MONSTER_ATK_DIV;
    int def = t.baseDef + stage / Balance::MONSTER_DEF_DIV;

    // 4) Monster 생성해서 반환
    return Monster(t.name, hp, atk, def);
}

int main(){ 
    mt19937 rng(random_device{}());

    Player p = Player("Hero", 60, 14, 4);

    for(int stage = 1; stage <= 10; stage++){
        cout << "\n==== STAGE " << stage << " ====\n";

        Monster m = makeMonster(stage, rng);
        battle(p, m, rng);
        if(!p.isAlive()) break;
    }

    cout << "\n=== 게임종료 ===\n";

    return 0; 
}