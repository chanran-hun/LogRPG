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
//체력바 표시
string makeHpBar(int current, int maxHp, int barWidth = 12){
    //0으로 나누기 방지
    if(maxHp <= 0) return "[------------]";
    //current값 보정
    current = clamp(current, 0, maxHp);
    int filled = (current * barWidth) / maxHp;
    //살아 있을 때 0칸이 되는것 방지
    if (current > 0 && filled == 0) filled = 1;
    string bar = "[";
    for (int i = 0; i < barWidth; i++){
        bar += (i < filled ? "█" : "-");
    }
    bar += "]";
    return bar;
}
//멤버들간의 밸런스를 맞추기 위한 체력 성장, 치명타율, 방어력 성장, 공격력 성장 등의 데이터 상수들
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
//상점 품목
namespace Shop {
    constexpr int POTION_HEAL = 20;     //포션의 회복량
    constexpr int POTION_PRICE = 15;    //포션의 가격
}
//플레이어
class Player{ 
    string name; //플레이어의 캐릭터 이름
    int level;   //플레이어 레벨
    int hp;      //플레이어 hp
    int maxHp;   //플레이어 최대체력
    int atk;     //플레이어 공격력
    int def;     //플레이어 방어력
    int exp;     //플레이어 경험치
    int gold;    //플레이어 골드 보유량
public: 
    Player(string name, int maxHp, int atk, int def): 
    name(name), 
    level(1),
    hp(maxHp), 
    maxHp(maxHp), 
    atk(atk), 
    def(def),
    exp(0),
    gold(0)
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

    int getMaxHp()const{
        return maxHp;
    }
    //골드 량 반환
    int getGold()const{
        return gold;
    }
    //골드 획득
    void gainGold(int amount){
        //음수값 방지
        if( amount > 0){
            gold += amount;
        }
    }
    //골드 사용
    bool spendGold(int amount){
        //음수값 방지
        if(amount <= 0) return true;
        if(amount > gold){
            return false;
        }
        gold -= amount;
        return true;
    }
    //데미지를 받을 때 
    void takeDamage(int dmg){
        hp -= dmg;
        if(hp < 0) hp = 0;
    }
    //체력 회복
    void heal(int amount){
        if(amount <= 0) return;
        hp = min(maxHp, hp + amount);
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
    //총 상태 출력
    void printSummary()const{
        cout << "\n=== PLAYER STATUS ===\n";
        cout << "이름: " << name << "\n";
        cout << "LV : " << level << "\n";
        cout << "HP : " << hp << " / " << maxHp << " " << makeHpBar(hp, maxHp) << "\n";
        cout << "ATK: " << atk << "  DEF: " << def << "\n";
        cout << "EXP: " << exp << " / 20\n";
        cout << "GOLD: " << gold << "\n";
        cout << "=====================\n";
    }
}; 
//몬스터
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
    //몬스터의 최대 체력 반환
    int getMaxHp()const{
        return maxHp;
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
    //rewardCount값 범위 이탈 방지
    rewardCount = clamp(rewardCount,1,(int)idx.size());
    if(rewardCount==3){
        typePrint("\n✨ 보스 전용 보상을 선택해주세요 ✨\n", 25);
    } else {
        typePrint("\n보상을 선택해주세요\n", 20);
    }
    for (int i = 0; i < rewardCount; i++){
        cout << i+1 << ") " << rewardText(idx[i]) << "\n";
        battleDelay();
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
//메뉴출력
int stageMenuInput(){
    //선택지 제시
    cout << "\n다음 행동을 선택하세요:\n";
    cout << "1) 다음 스테이지로\n";
    cout << "2) 상점\n";
    cout << "3) 상태 보기\n";
    cout << "0) 게임 종료\n> ";
    //선택 입력
    int choice;
    cin >> choice;
    //입력 실패시
    if (cin.fail()){
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "숫자만 입력해주세요. 기본값으로 1번(다음 스테이지)을 선택합니다.\n";
        return 1;
    }
    //선택 반환
    return choice;
}
//전투진행
void battle(Player &p, Monster &m, mt19937& rng, int stage){
    //1) 턴 카운트하기
    int turn = 1;
    while(p.isAlive() && m.isAlive()){
        //2) 보스인지, 그리고 특정 턴인지 체크하기
        bool isBoss = isBossStage(stage);
        bool isPowerTurn = isBoss && (turn % 3 == 0);
        //방어턴 변수도 만들기
        bool isGuardTurn = isBoss && (turn % 2 == 0);
        //전투시작
        cout << "----\n";
        if(rollChance(rng, Balance::MONSTER_DODGE)){
            cout << "💨 " << m.getName() << "이(가) " << p.getName() << "의 공격을 회피했습니다!" << endl;
        } else {
            bool critP = rollChance(rng, Balance::PLAYER_CRIT);
            //방어턴일 때 방어력 조정
            int effectiveDef = m.getDef();
            if(isGuardTurn){
                effectiveDef += 3;
                typePrint("🛡️ 보스가 방어 자세를 취합니다! 방어력이 상승했습니다!\n", 20);
            }
            int dmgP = max(1,p.getAtk()-effectiveDef);
            if(critP)dmgP *= 2;    
            cout << (critP ? "★ 치명타! " : "") << p.getName() << "이(가) " << dmgP << "의 피해를 입혔습니다." << endl;
            m.takeDamage(dmgP);
        }
        battleDelay();
        //몬스터 턴
        if(m.isAlive()){
            if(rollChance(rng, Balance::PLAYER_DODGE)){
                cout << "💨 " << p.getName() << "이(가) " << m.getName() << "의 공격을 회피했습니다!" << endl;
            } else {
                bool critM = rollChance(rng, Balance::MONSTER_CRIT);
                int dmgM = max(1, m.getAtk()-p.getDef());
                //3) 조건 만족할 때 데미지 계산 식 바꾸기
                if(isPowerTurn){
                    dmgM = dmgM * 2;
                    typePrint("💥 보스가 강력한 공격을 준비합니다!\n", 20);
                }
                //치명타 적용
                if(critM)dmgM *= 2;
                
                cout << (critM ? "★ 치명타! " : "") << m.getName() << "이(가) " << dmgM << "의 피해를 입혔습니다." << endl;
                p.takeDamage(dmgM);
            }
            battleDelay();
        }
        //4) 턴 증가 시키기
        turn++;
        //체력바와 체력 상태 출력
        cout << "[HP] " << p.getName() << ": " << p.getHp() << " / " << p.getMaxHp() << " " << makeHpBar(p.getHp(), p.getMaxHp()) << '\n';
        cout << "[HP] " << m.getName() << ": " << m.getHp() << " / " << m.getMaxHp() << " " << makeHpBar(m.getHp(), m.getMaxHp()) << '\n';
    }
    //전투 종료 이후
    if(p.isAlive()){
        cout << "승리하셨습니다." << endl;
        //경험치 증가
        int reward = 10; 
        cout << "경험치 +" << reward << "!" << endl;
        p.gainEXP(reward);
        //골드 증가
        int goldReward = isBossStage(stage) ? 30 : 10;
        p.gainGold(goldReward);
        cout << "골드 +" << goldReward << " (보유: " << p.getGold() << ")\n";
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
        //스테이지 클리어 후 메뉴
        while(true){
            //선택지 제시 + 선택
            int sel = stageMenuInput();
            //선택지 적용
            if(sel == 1){           //다음 스테이지 진행
                break;  
            } else if(sel == 2){    //상점으로 이동
                cout << "\n[상점] 준비중입니다! (다음 단계에서 구현할게요)\n";
            } else if(sel == 3){    //상태 표시
                p.printSummary();
            } else if(sel == 0){    //게임 종료
                cout << "\n게임을 종료합니다.\n";
                return 0;
            } else {
                cout << "잘못된 입력입니다. 다시 선택해주세요.\n";
            }
        }
    }

    cout << "\n=== 게임종료 ===\n";

    return 0; 
}