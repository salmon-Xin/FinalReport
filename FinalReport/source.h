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
#define ARTIFACT	{ 5,7,8,10,12 }										///�Ҧ�����������
#define TREASURE	{ 1,2,3,4,5,5,7,7,9,11,11,13,14,15,17 }		///�Ҧ��_�۪�����

void Debugmsg(int p, int score, int pcur);
void Debugmsg(int p, int score, int pcur, int ascore);

/*
 *�d������
 */
enum CardType : int {
	Treasure,
	Monster,
	Artifact,
};
/*
 * �Ǫ�����
 */
enum MonsterType : int {
	M1,
	M2,
	M3,
};
/*
 * ��������
 */
enum ArtifactType : int {
	A1,
	A2,
	A3,
	A4,
	A5,
};
/*
 *�^�X��������
 */
enum EndType {
	E1,	//�Ҧ����a�h�X���I
	E2,	//�P��w�S���d�P�F
	E3,	//�X�{�F�⦸���Ǫ�
};
/*
 *���a���c
 *TODO �ন class ���ɸ�Ʀw����
 */
struct Player
{
	int index;							//�s�u�� SQL �������a#��
	//bool isDisconnect = false;	//�s�u��
	//std::string name = "";		//�s�u��
	int score = 0;						//����
	bool state = true;				//�s�����^�X�O�_�~���I
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
	int score;		//��d�������D�Ǫ��ɬ�����, ��l���Ǫ������N�X
	Card() {}
	Card(CardType && ct, int sc)
		: cardType(std::move(ct)) {
		score = sc;
	}
	//DEGUB��
	std::string getCardType() {
		switch (cardType)
		{
		case Treasure:
			return "�_�ۥd";
			break;
		case Monster:
			return "�Ǫ��d";
			break;
		case Artifact:
			return "�����d";
			break;
		default:
			throw "getCardType error: unknow card type.";
			break;
		}
	}
};

struct Room
{
	int playerNumber = DEFAULT_PLAYER_NUMBER;	//�ж��H��
	int currentRound = 1;											//��e�^�X
	int maxRound = DEFAULT_ROUND;						//�̤j�^�X��
	int currentScore = 0;											//��e�^�X����(�_��)
	std::vector<Card> cards;									//�P��
	std::vector<Card> artifactcards;							//��e�����P��
	std::vector<Card> drawedCards;							//��e�^�X��X���P
	std::map<MonsterType, int> currentMonsters;		//��e�Ǫ��ƶq MonsterType, int
	std::vector<Player> player;									//�������a�H��
	std::vector<Player> currentAdvanturePlayer;			//��e�^�X���I�����a
	std::vector<Player> currentBackPlayer;					//��e�^�X�^�a�����a
	std::vector<Player> returnedPlayer;						//�w�g�^�a�����a
	/*
	 * �غc�l (�u�|�]�@��)
	 * @param pNumber	  ���a�H��    �w�]3�H
	 * @param sRoundMax �̤j�^�X�� �w�]5�^�X
	 */
	Room(int pNumber = DEFAULT_PLAYER_NUMBER, int sRoundMax = DEFAULT_ROUND) {
		maxRound = sRoundMax;				//�]�w�̤j�^�X��
		playerNumber = pNumber;				//�]�w���a�H��
		initPlayer();									//��l�ƪ��a��� ������
		initAdvanturePlayer();						//��l�ƫ_�I���a
		initTreasureCards();						//��l�ƵP�諸�_��
		initMonsterCards();							//��l�ƵP�諸�Ǫ��d
		shuffle();	//�~�P
		initMonsterCount();							//��l�ƩǪ��p��
		init();												//��l�ƨ�l�]�w
	}
	/*
	 * ��l�� �ж��H��, �^�X����, �^�a�H��
	 */
	void init() {
		currentScore = 0;
		/*		��J�����d	*/
	}
	/*
	 * ���m�_�ۥd�é�J�P��
	 */
	void initTreasureCards() {
		/*		��J�_�ۥd	*/
		int s[15] = TREASURE;
		for (int i = 0; i < sizeof(s) / sizeof(int); i++) {
			cards.emplace_back(CardType::Treasure, s[i]);
		}
	}
	/*
	 * ���m�Ǫ��d�é�J�P�� �Ǫ��dx3
	 */
	void initMonsterCards() {
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				cards.emplace_back(CardType::Monster, (int)i);
	}
	/*
	 * ���m�_�ۥd�é�J�P��
	 */
	void initArtifactCards() {
		/*		��J�����d	*/
		int a[5] = ARTIFACT;
		for (auto i = 0; i < sizeof(a) / sizeof(int); i++) {
			artifactcards.emplace_back(CardType::Artifact, a[i]);
		}
	}
	/*
	 * ��l�ƩǪ��p��
	 */
	void initMonsterCount() {
		currentMonsters[MonsterType::M1] = 0;
		currentMonsters[MonsterType::M2] = 0;
		currentMonsters[MonsterType::M3] = 0;
	}
	/*
	 * ��l�ƪ��a��� (������)
	 */
	void initPlayer() {
		for (int i = 0; i < playerNumber; i++) {
			player.push_back(Player(i));
		}
	}
	/*
	 * ��l�ƫ_�I���a
	 */
	void initAdvanturePlayer() {
		currentAdvanturePlayer = player;
	}
	/*
	 * ���m�^�X��
	 */
	void doRestart() {
		if (getReturnedNumber() >= 1) {	// �w�g�^�a�����a ���ʨ� ��e�^�X���I�����a �í��m���a���A
			currentAdvanturePlayer.insert(currentAdvanturePlayer.end(), returnedPlayer.begin(), returnedPlayer.end());
			returnedPlayer.clear();
			for (int i = 0; i < playerNumber; i++)
				player[i].restart();
		}
	}
	/*
	 * �N�n�^�a�����a����w�^�a
	 */
	void doMoveBackToReturn() {
		returnedPlayer.insert(returnedPlayer.end(), currentBackPlayer.begin(), currentBackPlayer.end());
		currentBackPlayer.clear();
	}
	/*
	 * ��������
	 * @param type ��������
	 */
	void doEndRound(EndType type) {
		switch (type)
		{
		case E1:		//�Ҧ��H���^�a
			break;
		case E2:		//�P��S�d�P�F
			break;
		case E3:		//�X�{�ۦP���Ǫ�2�i
			drawedCards.erase(drawedCards.end() - 1);										//�����̫᭫�ƪ��Ǫ��d
			for (auto it = drawedCards.begin(); it != drawedCards.end(); it++) {	//��X�����d�ñq��X���P�襤����
				if ((*it).cardType == Artifact) {
					drawedCards.erase(it);
					break;
				}
			}

			cards.insert(cards.end(), drawedCards.begin(), drawedCards.end());	//�N��X���P��^�P��
			drawedCards.clear();
			break;
		default:
			//throw "Error: unknow EndType: " + type;
			break;
		}
	}
	/*
	 * �P�_���^�X�O�_����
	 * @return �^�ǵ����N�X, ��l�^�� -1
	 */
	int isRoundEnd() {
		if (getAdventureNumber() == 0) {
			doEndRound(E1);
			return E1;
		}
		else if (getCardsnumber() == 0) {
			doEndRound(E2);
			return E2;
		}
		else if (isMonsterRepeat() != -1) { //-1 ��ܨS����
			doEndRound(E3);
			return E3;
		}
		return -1;
	}
	/*
	 * �Ǫ�����
	 * @Return ��^���ƪ��Ǫ�����, ��l��^ -1
	 */
	int isMonsterRepeat() {
		if (currentMonsters[MonsterType::M1] >= 2) {
			return M1;
		}
		if (currentMonsters[MonsterType::M2] >= 2) {
			return M2;
		}
		if (currentMonsters[MonsterType::M3] >= 2) {
			return M3;
		}
		return -1;
	}
	/*
	 * �^�X�Ƥw����^ true, ��l false
	 */
	bool isGameEnd() {
		return currentRound == maxRound;
	}
	/*
	 * ��^��e�_�I�H��
	 */
	int getAdventureNumber() const {
		return (int)currentAdvanturePlayer.size();
	}
	/*
	 * ��^��e�^�a�H��
	 */
	int getBackNumber() const {
		return (int)currentBackPlayer.size();
	}
	/*
	 * ��^��e�w�^�a�H��
     */
	int getReturnedNumber() const {
		return (int)returnedPlayer.size();
	}
	/*
	 * ��^��e�P��ƶq
	 */
	size_t getCardsnumber() const {
		return cards.size();
	}
	/*
	 * ��P
	 */
	Card draw()
	{
		auto c = cards.begin();										//�P��Ĥ@�i�P
		drawedCards.insert(drawedCards.end(), c, c+1);
		cards.erase(c);													//�P��Ĥ@�i�P�R��
		auto dc = drawedCards.back();							//���o��X���P
		switch (dc.cardType)											//�P�w�d�P����
		{
		case Treasure:
			currentScore += dc.score;								//�_�ۼƼW�[
			break;
		case Artifact:
			break;
		case Monster:
			currentMonsters[(MonsterType)dc.score]++;		//�Ǫ������ƼW�[
			break;
		default:
			throw "Drawed an unexit card.";
			break;
		}
		return dc;
	}
	/*
	 * �~�P
	 */
	void shuffle()
	{
		srand((unsigned int)time(0));
		std::random_shuffle(cards.begin(), cards.end()); //���ä��� //myrandom
	}
};

/*
 * �C���W�h ���Ө�Ȥ��
 */

 /*
 �U�@�����}�l
 �Y�����X�{���������Q�����A�h���X�C��
 (�Y�������X�{�h�O�d�b�P�դ��Y�i)
 �N�U�@�i�����d�~�J�P�աA½�௫���d
 �Ѿl���_�ۥd�Ψa���d�]�~�^�P�w
 �Ҧ����a�^�챴�I���A���ƤW�z�y�{

 */