#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <ctime>
#include <random>
#include <algorithm>

#define DEFAULT_PLAYER_NUMBER 3
#define DEFAULT_ROUND 5
#define DEBUG 1
#define ARTIFACT	{ 5,7,8,10,12 }										///所有神器的分數
#define TREASURE	{ 1,2,3,4,5,5,7,7,9,11,11,13,14,15,17 }		///所有寶石的分數

//函式宣告
void Debugmsg(int p, int score, int pcur);
void Debugmsg(int p, int score, int pcur, int ascore);

/*
 *卡片類型
 */
enum CardType : int {
	Treasure,
	Monster,
	Artifact,
};
/*
 * 怪物類型
 */
enum MonsterType : int {
	M1,
	M2,
	M3,
};
/*
 * 神器類型
 */
enum ArtifactType : int {
	A1,
	A2,
	A3,
	A4,
	A5,
};
/*
 *回合結束類型
 */
enum EndType {
	E1,	//所有玩家退出探險
	E2,	//牌堆已沒有卡牌了
	E3,	//出現了兩次的怪物
};
/*
 *玩家結構
 *TODO 轉成 class 提升資料安全性
 */
struct Player
{
	int index;							//連線用 SQL 中的玩家#數
	//bool isDisconnect = false;	//連線用
	//std::string name = "";		//連線用
	int score = 0;						//分數
	bool state = true;				//存取此回合是否繼續探險
	Player() {}
	Player(int id) {
		index = id;
	}
	void restart() {
		state = true;
	}
	void addScore(int s) {
		score += s;
	}
};

struct Card
{
	CardType cardType;
	int score;		//當卡片類型非怪物時為分數, 其餘為怪物類型代碼
	Card() {}
	Card(CardType && ct, int sc)
		: cardType(std::move(ct)) {
		score = sc;
	}
	//DEGUB用
	std::string getCardType() {
		switch (cardType)
		{
		case Treasure:
			return "寶石卡";
			break;
		case Monster:
			return "怪物卡";
			break;
		case Artifact:
			return "神器卡";
			break;
		default:
			throw "getCardType error: unknow card type.";
			break;
		}
	}
};

struct Room
{
	int playerNumber = DEFAULT_PLAYER_NUMBER;	//房間人數
	int currentRound = 1;											//當前回合
	//int maxRound = DEFAULT_ROUND;						//最大回合數
	int currentScore = 0;											//當前回合分數(寶石)
	int currentArtifactScore = 0;											//當前回合分數(神器)
	std::vector<Card> cards;									//牌堆
	std::vector<Card> artifactcards;							//當前神器牌堆
	std::vector<Card> drawedCards;							//當前回合抽出的牌
	std::map<MonsterType, int> currentMonsters;		//當前怪物數量 MonsterType, int
	std::vector<Player> player;									//全部玩家人數
	std::vector<Player> currentAdvanturePlayer;			//當前回合探險的玩家
	std::vector<Player> currentBackPlayer;					//當前回合回家的玩家
	std::vector<Player> returnedPlayer;						//已經回家的玩家
	/*
	 * 建構子 (只會跑一次)
	 * @param pNumber	  玩家人數    預設3人
	 * @param sRoundMax 最大回合數 預設5回合
	 */
	Room(int pNumber = DEFAULT_PLAYER_NUMBER) {
		playerNumber = pNumber;				//設定玩家人數
		initPlayer();									//初始化玩家資料 網路用
		initAdvanturePlayer();						//初始化冒險玩家
		initTreasureCards();						//初始化牌堆的寶藏
		initMonsterCards();							//初始化牌堆的怪物卡
		initArtifactCards();							//初始化神器卡
		shuffle(cards.begin(), cards.end());	//牌堆洗牌
		initMonsterCount();							//初始化怪物計數
		init();												//初始化其餘設定
	}
	/*
	 * 初始化 房間人數, 回合分數, 回家人數
	 */
	void init() {
		currentScore = 0;
		currentArtifactScore = 0;
		/*		放入神器卡	*/
	}
	/*
	 * 重置寶石卡並放入牌堆
	 */
	void initTreasureCards() {
		/*		放入寶石卡	*/
		int s[15] = TREASURE;
		for (int i = 0; i < sizeof(s) / sizeof(int); i++) {
			cards.emplace_back(CardType::Treasure, s[i]);
		}
	}
	/*
	 * 重置怪物卡並放入牌堆 怪物卡x3
	 */
	void initMonsterCards() {
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				cards.emplace_back(CardType::Monster, (int)i);
	}
	/*
	 * 初始化神器卡並放入神器卡牌堆暫存
	 */
	void initArtifactCards() {
		int a[5] = ARTIFACT;
		for (auto i = 0; i < sizeof(a) / sizeof(int); i++) {
			artifactcards.emplace_back(CardType::Artifact, a[i]);
		}
		shuffle(artifactcards.begin(), artifactcards.end());	//神器卡牌堆洗牌
	}
	/*
	 * 初始化怪物計數 (每Round執行)
	 */
	void initMonsterCount() {
		currentMonsters[MonsterType::M1] = 0;
		currentMonsters[MonsterType::M2] = 0;
		currentMonsters[MonsterType::M3] = 0;
	}
	/*
	 * 初始化玩家資料 (網路用)
	 */
	void initPlayer() {
		for (int i = 0; i < playerNumber; i++) {
			player.push_back(Player(i));
		}
	}
	/*
	 * 初始化冒險玩家
	 */
	void initAdvanturePlayer() {
		currentAdvanturePlayer = player;
	}
	/*
	 * 新回合開始用
	 */
	void doRestart() {
		/*
		if (getReturnedNumber() >= 1) {	// 已經回家的玩家 移動到 當前回合探險的玩家 並重置玩家狀態
			currentAdvanturePlayer.insert(currentAdvanturePlayer.end(), returnedPlayer.begin(), returnedPlayer.end());
			returnedPlayer.clear();
			for (int i = 0; i < playerNumber; i++)
				player[i].restart();
		}*/
		returnedPlayer.clear();
		currentAdvanturePlayer = player;
		for (auto i = player.begin(); i != player.end(); i++) { (*i).restart(); };
		//加入一張神器卡
		if (artifactcards.size()) { cards.insert(cards.end(), artifactcards.begin(), artifactcards.begin() + 1); }
		shuffle(cards.begin(), cards.end());
	}
	/*
	 * 分配分數給回家的玩家
	 */
	void doAllocationScore() {
		int bNumber = getBackNumber();
		if (bNumber) {	//神器
			currentBackPlayer.back().addScore(currentArtifactScore);
			if (DEBUG) std::cout << "#" << currentBackPlayer.back().index << " 獲得神器卡 分數: " << currentArtifactScore << std::endl;
		}
		for (int i = 0, score = currentScore / bNumber; i < bNumber; i++) {
			currentBackPlayer.at(i).addScore(score);
			Debugmsg(currentBackPlayer.at(i).index, score, currentBackPlayer.at(i).score);
		}
		doUpdataScore();
		if(!DEBUG)	std::cout << "場上餘剩寶石分數: " << currentScore << std::endl;
	}
	/*
	 * 將要回家的玩家移到已回家
	 */
	void doMoveBackToReturn() {
		returnedPlayer.insert(returnedPlayer.end(), currentBackPlayer.begin(), currentBackPlayer.end());
		currentBackPlayer.clear();
	}
	/*
	 * 重置/更新回合分數
	 */
	void doUpdataScore() {
		currentScore %= getBackNumber();
	}
	/*
	 * Round 結束執行
	 * @param type 結束類型
	 */
	void doEndRound(EndType type) {
		switch (type)
		{
		case E1:		//所有人都回家
			doRemoveArtifact();										//找出神器卡並從抽出的牌堆中移除
			doPutBack();													//將抽出的牌放回牌堆
			drawedCards.clear();										//安全處理
			initMonsterCount();											//清除怪物暫存
			break;
		case E2:		//牌堆沒卡牌了 強制結束這回合
			doRemoveArtifact();										//找出神器卡並從抽出的牌堆中移除
			doPutBack();													//將抽出的牌放回牌堆
			drawedCards.clear();										//安全處理
			initMonsterCount();											//清除怪物暫存
			break;
		case E3:		//出現相同的怪物2張
			drawedCards.erase(drawedCards.end() - 1);		//移除最後重複的怪物卡
			doRemoveArtifact();										//找出神器卡並從抽出的牌堆中移除
			doPutBack();													//將抽出的牌放回牌堆
			drawedCards.clear();										//安全處理
			initMonsterCount();											//清除怪物暫存
			break;
		default:
			break;
		}
	}
	/*
	 * 移除已抽出的牌堆中的神器卡	
	 */
	void doRemoveArtifact() {
		for (auto it = drawedCards.begin(); it != drawedCards.end(); it++) {
			if ((*it).cardType == Artifact) {
				drawedCards.erase(it);
				return;
			}
		}
	}
	/*
	 將抽出的牌放回牌堆
	 */
	void doPutBack() {
		cards.insert(cards.end(), drawedCards.begin(), drawedCards.end());
	}
	/*
	 * 判斷本回合是否結束
	 * @return 回傳結束代碼, 其餘回傳 -1
	 */
	int isRoundEnd() {
		if (getAdventureNumber() == 0) {// 沒人要冒險了
			doEndRound(E1);
			return E1;
		}
		else if (getCardsnumber() == 0) {// 牌堆沒有牌了
			doEndRound(E2);
			return E2;
		}
		else if (isMonsterRepeat() != -1) { //-1 表示沒重複
			doEndRound(E3);
			return E3;
		}
		return -1;
	}
	/*
	 * 怪物重複
	 * @Return 返回重複的怪物類型, 其餘返回 -1
	 */
	int isMonsterRepeat() {
		if (currentMonsters[MonsterType::M1] >= 2) {
			if (DEBUG) std::cout << "怪物卡出現了兩次!!   種類: " << M1 << std::endl;
			return M1;
		}
		if (currentMonsters[MonsterType::M2] >= 2) {
			if (DEBUG) std::cout << "怪物卡出現了兩次!!   種類: " << M2 << std::endl;
			return M2;
		}
		if (currentMonsters[MonsterType::M3] >= 2) {
			if (DEBUG) std::cout << "怪物卡出現了兩次!!   種類: " << M3 << std::endl;
			return M3;
		}
		return -1;
	}
	/*
	 * 回合數已滿返回 true, 其餘 false
	 */
	bool isGameEnd() {
		return currentRound == DEFAULT_ROUND;
	}
	/*
	 * 返回當前冒險人數
	 */
	int getAdventureNumber() const {
		return (int)currentAdvanturePlayer.size();
	}
	/*
	 * 返回當前回家人數
	 */
	int getBackNumber() const {
		return (int)currentBackPlayer.size();
	}
	/*
	 * 返回當前已回家人數
     */
	int getReturnedNumber() const {
		return (int)returnedPlayer.size();
	}
	/*
	 * 返回當前牌堆數量
	 */
	int getCardsnumber() const {
		return (int)cards.size();
	}
	/*
	 * 抽牌
	 */
	Card draw()
	{
		auto c = cards.begin();										//牌堆第一張牌
		drawedCards.insert(drawedCards.end(), c, c+1);
		cards.erase(c);													//牌堆第一張牌刪除
		auto dc = drawedCards.back();							//取得抽出的牌
		switch (dc.cardType)											//判定卡牌類型
		{
		case Treasure:
			currentScore += dc.score;								//寶石數增加
			break;
		case Artifact:
			//currentScore += dc.score;								//寶石數增加
			break;
		case Monster:
			currentMonsters[(MonsterType)dc.score]++;		//怪物種類數增加
			break;
		default:
			throw "Drawed an unexit card.";
			break;
		}
		return dc;
	}
	/*
	 * 洗牌
	 */
	void shuffle(std::vector<Card>::iterator begin, std::vector<Card>::iterator end)
	{
		srand((unsigned int)time(0));
		std::random_shuffle(begin, end); //打亂元素
	}
};

/*
 * 遊戲規則 移植到客戶端
 */

 /*
 下一次的開始
 若此次出現的神器未被拿走，則移出遊戲
 (若神器未出現則保留在牌組中即可)
 將下一張神器卡洗入牌組，翻轉神殿卡
 剩餘的寶石卡及災難卡也洗回牌庫
 所有玩家回到探險中，重複上述流程

 */