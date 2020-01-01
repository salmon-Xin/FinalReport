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

//�禡�ŧi
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
	M4,
	M5,
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
	/*
	 *	�غc�l
	 * @param ct �d������
	 * @param sc �d�������ƩΤl����
	 */
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
	//int maxRound = DEFAULT_ROUND;						//�̤j�^�X��
	int currentScore = 0;											//��e�^�X����(�_��)
	int currentArtifactScore = 0;											//��e�^�X����(����)
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
	Room(int pNumber = DEFAULT_PLAYER_NUMBER) {
		playerNumber = pNumber;				//�]�w���a�H��
		initPlayer();									//��l�ƪ��a��� ������
		initAdvanturePlayer();						//��l�ƫ_�I���a
		initTreasureCards();						//��l�ƵP�諸�_��
		initMonsterCards();							//��l�ƵP�諸�Ǫ��d
		initArtifactCards();							//��l�Ư����d
		shuffle(cards.begin(), cards.end());	//�P��~�P
		initMonsterCount();							//��l�ƩǪ��p��
		//init();												//��l�ƨ�l�]�w
	}
	/*
	 * ��l�� �ж��H��, �^�X����, �^�a�H��
	 */
	void init() {
		currentScore = 0;
		currentArtifactScore = 0;
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
		for (int i = 0; i < 5; i++)
			for (int j = 0; j < 3; j++)
				cards.emplace_back(CardType::Monster, (int)i);
	}
	/*
	 * ��l�Ư����d�é�J�����d�P��Ȧs
	 */
	void initArtifactCards() {
		int a[5] = ARTIFACT;
		for (auto i = 0; i < sizeof(a) / sizeof(int); i++) {
			artifactcards.emplace_back(CardType::Artifact, a[i]);
		}
		shuffle(artifactcards.begin(), artifactcards.end());	//�����d�P��~�P
	}
	/*
	 * ��l�ƩǪ��p�� (�CRound����)
	 */
	void initMonsterCount() {
		currentMonsters[MonsterType::M1] = 0;
		currentMonsters[MonsterType::M2] = 0;
		currentMonsters[MonsterType::M3] = 0;
		currentMonsters[MonsterType::M4] = 0;
		currentMonsters[MonsterType::M5] = 0;
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
	 * �s�^�X�}�l�� �[�J�s���H�������d
	 */
	void doRestart() {
		currentAdvanturePlayer.insert(currentAdvanturePlayer.end(), returnedPlayer.begin(), returnedPlayer.end());
		std::sort(currentAdvanturePlayer.begin(), currentAdvanturePlayer.end() - 1, compareByIndex);
		returnedPlayer.clear();
		for (auto i = player.begin(); i != player.end(); i++) { (*i).restart(); };
		//�[�J�@�i�����d
		if (artifactcards.size()) { cards.insert(cards.end(), artifactcards.begin(), artifactcards.begin() + 1); }
		artifactcards.erase(artifactcards.begin());
		shuffle(cards.begin(), cards.end());
	}
	/*
	 * ���t���Ƶ��^�a�����a
	 */
	void doAllocationScore() {
		int bNumber = getBackNumber();
		if (bNumber == 1) {	//����
			currentBackPlayer.back().addScore(currentArtifactScore);
			currentArtifactScore = 0;
		}
		for (int i = 0, score = currentScore / bNumber; i < bNumber; i++) {
			currentBackPlayer.at(i).addScore(score);
			if (currentArtifactScore > 0 && bNumber == 1) {
				Debugmsg(currentBackPlayer.at(i).index, score, currentBackPlayer.at(i).score, currentArtifactScore);
			}
			else
				Debugmsg(currentBackPlayer.at(i).index, score, currentBackPlayer.at(i).score);
		}
		doUpdataScore();
		if (DEBUG)	std::cout << "���W�l���_�ۤ���: " << currentScore << std::endl;
	}
	/*
	 * �N�n�^�a�����a����w�^�a
	 */
	void doMoveBackToReturn() {
		returnedPlayer.insert(returnedPlayer.end(), currentBackPlayer.begin(), currentBackPlayer.end());
		currentBackPlayer.clear();
	}
	/*
	 * ���m/��s�^�X����
	 */
	void doUpdataScore() {
		currentScore %= getBackNumber();
	}
	/*
	 * Round ��������
	 * @param type ��������
	 */
	void doEndRound(EndType type) {
		switch (type)
		{
		case E1:		//�Ҧ��H���^�a
			doRemoveArtifact();										//��X�����d�ñq��X���P�襤����
			doPutBack();													//�N��X���P��^�P��
			drawedCards.clear();										//�w���B�z
			initMonsterCount();											//�M���Ǫ��Ȧs
			break;
		case E2:		//�P��S�d�P�F �j����o�^�X	error ���i��X�{
			exit(-1);
			break;
		case E3:		//�X�{�ۦP���Ǫ�2�i
			drawedCards.erase(drawedCards.end() - 1);		//�����̫᭫�ƪ��Ǫ��d
			doRemoveArtifact();										//��X�����d�ñq��X���P�襤����
			doPutBack();													//�N��X���P��^�P��
			drawedCards.clear();										//�w���B�z
			initMonsterCount();											//�M���Ǫ��Ȧs
			break;
		default:
			break;
		}
	}
	/*
	 * �����w��X���P�襤�������d
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
	 �N��X���P��^�P��
	 */
	void doPutBack() {
		cards.insert(cards.end(), drawedCards.begin(), drawedCards.end());
	}
	/*
	 * �P�_���^�X�O�_����
	 * @return �^�ǵ����N�X, ��l�^�� -1
	 */
	int isRoundEnd() {
		if (getAdventureNumber() == 0) {// �S�H�n�_�I�F
			doEndRound(E1);
			return E1;
		}
		else if (getCardsnumber() == 0) {// �P��S���P�F
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
			if (DEBUG) std::cout << "�Ǫ��d�X�{�F�⦸!!   ����: " << M1 << std::endl;
			return M1;
		}
		if (currentMonsters[MonsterType::M2] >= 2) {
			if (DEBUG) std::cout << "�Ǫ��d�X�{�F�⦸!!   ����: " << M2 << std::endl;
			return M2;
		}
		if (currentMonsters[MonsterType::M3] >= 2) {
			if (DEBUG) std::cout << "�Ǫ��d�X�{�F�⦸!!   ����: " << M3 << std::endl;
			return M3;
		}
		if (currentMonsters[MonsterType::M4] >= 2) {
			if (DEBUG) std::cout << "�Ǫ��d�X�{�F�⦸!!   ����: " << M4 << std::endl;
			return M3;
		}
		if (currentMonsters[MonsterType::M5] >= 2) {
			if (DEBUG) std::cout << "�Ǫ��d�X�{�F�⦸!!   ����: " << M5 << std::endl;
			return M3;
		}
		return -1;
	}
	/*
	 * �^�X�Ƥw����^ true, ��l false
	 */
	bool isGameEnd() {
		return currentRound == DEFAULT_ROUND + 1;
	}
	/*
	 *	��^�`�C���H��
	 */
	int getPlayerNumber() const {
		return (int)player.size();
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
	int getCardsnumber() const {
		return (int)cards.size();
	}
	/*
	 * ��P
	 */
	Card draw()
	{
		auto c = cards.begin();										//�P��Ĥ@�i�P
		drawedCards.insert(drawedCards.end(), c, c + 1);
		cards.erase(c);													//�P��Ĥ@�i�P�R��
		auto dc = drawedCards.back();							//���o��X���P
		switch (dc.cardType)											//�P�w�d�P����
		{
		case Treasure:
			currentScore += dc.score;								//�_�ۼƼW�[
			break;
		case Artifact:
			currentArtifactScore += dc.score;
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
	void shuffle(std::vector<Card>::iterator begin, std::vector<Card>::iterator end)
	{
		srand((unsigned int)time(0));
		std::random_shuffle(begin, end); //���ä���
	}
	/*
	 * �����Ӫ��a���N��
	 * @param a �n��������a
	 * @param b �n��������a
	 * @return ��^ a ���a�N���O�_�C�� b ���a
	 */
	static bool compareByIndex(Player &a, Player &b) {
		return (a.index < b.index);
	}
	/*
	 *	�����Ӫ��a������
	 * @param a �n��������a
	 * @param b �n��������a
	 * @return ��^ a ���a���ƬO�_���� b ���a
	 */
	static bool compareByScore(Player &a, Player &b) {
		return (a.score > b.score);
	}
	/*
	 *	�o��̫ᦨ�Z
	 */
	void finalResults() {
		if (!DEBUG)	return;
		std::sort(currentAdvanturePlayer.begin(), currentAdvanturePlayer.end(), compareByScore);
		for (int i = 0; i < getAdventureNumber(); i++) {
			std::cout << "��" << i + 1 << "�W   ���a#" << currentAdvanturePlayer.at(i).index << "  �̫�o��: " << currentAdvanturePlayer.at(i).score << std::endl;
		}
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