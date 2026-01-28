#include <iostream>
#include <algorithm>    //max, shuffle, clamp
#include <vector>
#include <random>
#include <string>
#include <limits>
#include <thread>
#include <chrono>
using namespace std;

bool DEBUG_MODE = true;
constexpr int BOSS_INTRO_DELAY_MS = 1000;
constexpr int BATTLE_DELAY_MS = 1000;
//타자치는 느낌 삽입
void typePrint(const string& s, int msPerChar){
    if (DEBUG_MODE) {
        cout << s;
        return;
    }
    for(char c : s){
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(msPerChar));
    }
}
//전투중 지연 삽입
void battleDelay(){
    if (!DEBUG_MODE)
        this_thread::sleep_for(chrono::milliseconds(BATTLE_DELAY_MS));
}

namespace Balance {
    constexpr double PLAYER_CRIT  = 0.20;
    constexpr double PLAYER_DODGE = 0.10;
    constexpr double MONSTER_CRIT = 0.20;
    constexpr double MONSTER_DODGE= 0.10;
    constexpr int MONSTER_HP_SCALE = 3;
    constexpr int MONSTER_ATK_DIV = 2;
    constexpr int MONSTER_DEF_DIV = 3;
    constexpr int BOSSMONSTER_HP_SCALE = 4;
    constexpr int BOSSMONSTER_ATK_DIV = 2;
    constexpr int BOSSMONSTER_DEF_DIV = 2;
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

void chooseReward(Player& p, mt19937& rng, int rewardCount){
    // 1) 보상 후보 인덱스 목록
    vector<int> idx = {0,1,2};
    // 2) 인덱스 랜덤하게 섞기
    shuffle(idx.begin(), idx.end() ,rng);
    // 3) 화면에 선택 목록 추가
    if(rewardCount==3){
        typePrint("\n✨ 보스 전용 보상을 선택해주세요 ✨\n", 25);
    } else {
        typePrint("\n보상을 선택해주세요\n", 20);
    }
    for (int i = 0; i < rewardCount; i++){
        cout << i+1 << ")" << rewardText(idx[i]) << "\n";
    }
    cout << "\n>";
    // 4) 사용자 입력 받기
    int choice = 0;
    cin >> choice;
    // 4)-1 입력 실패 처리
    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "숫자만 입력해주세요. 1번을 선택합니다.\n";
        choice = 1;
    }
    // 5) 사용자 입력 처리
    int chosen = 0;
    if(choice >= 1 && choice <= rewardCount){
        chosen = idx[choice-1];
    } else {
        cout << "잘못된 입력입니다. 1번을 선택합니다.\n";
        chosen = idx[0];
    }
    // 7) chosen에 따라 효과 적용
    applyReward(p,chosen);

    cout << "[STAT] ATK:" << p.getAtk() << " DEF:" << p.getDef() << " HP:" << p.getHp() << endl;
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
    {"고전파", 50, 15, 5},
    {"전승의 종소리", 55, 8, 10}
};

Monster makeMonster(int stage, mt19937& rng){
    // 1)stage에 따라 풀 고르기
    const vector<MonsterTemplate>* pool;
    if(stage >= 5 && stage % 5 == 0 && !bossMonsters.empty()) {
        pool = &bossMonsters;
    } else if(stage <= 3) {
        pool = &earlyMonsters;
    } else if (stage <= 6) {
        pool = &midMonsters;
    } else {
        pool = &lateMonsters;
    }
    

    // 2)풀에서 랜덤으로 하나 뽑기
    uniform_int_distribution<int> dist(0, (int)pool->size() - 1);
    const MonsterTemplate& t = (*pool)[dist(rng)];

    // 3)스테이지 보정
    int hp = 0, atk = 0, def = 0;
    if(pool == &bossMonsters){
        hp = t.baseHp + stage * Balance::BOSSMONSTER_HP_SCALE;
        atk = t.baseAtk + stage / Balance::BOSSMONSTER_ATK_DIV;
        def = t.baseDef + stage / Balance::BOSSMONSTER_DEF_DIV;
    } else {
        hp = t.baseHp + stage * Balance::MONSTER_HP_SCALE;
        atk = t.baseAtk + stage / Balance::MONSTER_ATK_DIV;
        def = t.baseDef + stage / Balance::MONSTER_DEF_DIV;
    }
    
    // 4) Monster 생성해서 반환
    if(DEBUG_MODE){
        cout << "[DEBUG] stage " << stage << " -> " << t.name << (pool == &bossMonsters ? " (Boss)\n" : "\n");
    }
    
    return Monster(t.name, hp, atk, def);
}
//보스 판별
bool isBossStage(int stage){
    return stage >= 5 && stage % 5 == 0;
}
//보스 몬스터 전용 인트로
void showBossIntro(int stage, const Monster& m){
    cout << "==============================\n";
    this_thread::sleep_for(chrono::milliseconds(BOSS_INTRO_DELAY_MS));
    cout << "        ⚠️  BOSS 등장! ⚠️\n";
    this_thread::sleep_for(chrono::milliseconds(BOSS_INTRO_DELAY_MS));
    cout << "         STAGE " << stage << "\n";
    this_thread::sleep_for(chrono::milliseconds(BOSS_INTRO_DELAY_MS));
    cout << "      " << m.getName() << " 출현!\n";
    this_thread::sleep_for(chrono::milliseconds(BOSS_INTRO_DELAY_MS));
    cout << "==============================\n";
    if(DEBUG_MODE){
        cout << "HP: " << m.getHp() << "  ATK: " << m.getAtk() << "  DEF: " << m.getDef() << "\n\n";
    } else {
        cout << "\n";
    }
}
//전투진행
void battle(Player &p, Monster &m, mt19937& rng, int stage){
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
        battleDelay();

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
            battleDelay();
        }
        cout << "[HP] " << p.getName() << ": " << p.getHp() << " / " << m.getName() << ": " << m.getHp() << endl;
    }
    //전투 종료 이후
    if(p.isAlive()){
        cout << "승리하셨습니다." << endl;
        //경험치 증가
        int reward = 10; 
        cout << "경험치 +" << reward << "!" << endl;
        p.gainEXP(reward);
        //보스 특별 보상
        if(isBossStage(stage)){
            typePrint("🎁 보스 보상! 더 많은 선택지 중 하나를 고르세요!\n", 25);
            chooseReward(p,rng,3);
        } else {
            chooseReward(p,rng,2);
        }
    } else {
        cout << "패배하셨습니다." << endl;
    }
}

int main(){ 
    mt19937 rng(random_device{}());

    Player p = Player("Hero", 60, 14, 4);

    cout << "모드를 선택하세요:\n";
    cout << "1) 플레이 모드\n";
    cout << "2) 개발 모드\n>" ;

    int mode;
    cin >> mode;

    if(mode == 2){
        DEBUG_MODE = true;
        cout << "[개발 모드 활성화]\n";
    } else {
        DEBUG_MODE = false;
        cout << "[플레이 모드 활성화]\n";
    }

    for(int stage = 1; stage <= 10; stage++){
        
        Monster m = makeMonster(stage, rng);
        //보스 전용 인트로
        if(isBossStage(stage)){
            showBossIntro(stage,m);
        } else {
            cout << "\n==== STAGE " << stage << " ====\n";
        }

        battle(p, m, rng, stage);
        if(!p.isAlive()) break;
    }

    cout << "\n=== 게임종료 ===\n";

    return 0; 
}