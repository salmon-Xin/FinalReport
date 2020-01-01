#include "source.h"
using namespace std;

int main()
{
	int ret = 0;													//�w�s��
	while (ret < 3 || ret > 8)
	{
		cout << "�C���H��(���3~8�H): ";
		cin >> ret;
	}
	Room room(ret);		//�إߩж� �ê�l�� �ж��H�� �̤j�^�X�� ����
	//�P�_�O�_�̤j�^�X
	while (!room.isGameEnd())
	{
		cout << " �^�X #" << room.currentRound << " �}�l!\n"; //round 2�X��
		room.doRestart();							//���m���a���A(���I), �^�X���Ƭ�0
		//�P�_�^�X�O�_����, -1���C���~��
		while (room.isRoundEnd() == -1)	
		{
			room.draw();		//��P
			if (DEBUG && room.drawedCards.back().cardType == CardType::Monster)	
				cout << "½�}�F�d��!!  �d������: " << room.drawedCards.back().getCardType() << "  ����: " << room.drawedCards.back().score << endl;
			else if (DEBUG && (room.drawedCards.back().cardType == CardType::Treasure || room.drawedCards.back().cardType == CardType::Treasure))
				cout << "½�}�F�d��!!  �d������: " << room.drawedCards.back().getCardType() << "  ����: " << room.drawedCards.back().score << endl;

			//�P�_�O�_����, �������ܴN�i��۹�B��
			if (room.isRoundEnd() != -1) { break; }
			// ���o�C��_�I���a���ާ@
			for (auto it = room.currentAdvanturePlayer.begin(); it != room.currentAdvanturePlayer.end();)
			{
				//�߰ݬO�_�~��
				do {
					cout << "�аݪ��a #" << (*it).index << " �O�_�~���I(��J1��0): ";
					cin >> ret;
				} while (!(ret == 1 || ret == 0));	//���~�B�z
				if (((*it).state = ret) == false) {	//�N�_�I���a���ʨ��e�^�a���a(�e��)
					room.currentBackPlayer.insert(room.currentBackPlayer.end(), *it);
					it = room.currentAdvanturePlayer.erase(it);
				}
				else {
					it++;
				}
			}
			if (DEBUG) cout << "�����^�a�H��: " << room.getBackNumber() << "     �~��_�I�H��: " << room.getAdventureNumber() << " \n";
			// �@�H�^�a - ��o�����d, �ÿW�o�q�D�W�_��
			if (room.getBackNumber() == 1) {
				if (room.currentBackPlayer.back().state == true)	//���~�P�_
					throw "Error: �Ӧ^�a�����a�S�^�a, �ݨӥL�y�s�Ѫ�";
				room.doAllocationScore();									//���t���Ƶ��^�a�����a�ç�s���W����
				room.doMoveBackToReturn();								//�N�^�a�����a����w�^�a
			}
			//�h�H�^�a - �����d�d�b�q�D�W, �����_��
			else if (room.getBackNumber() > 1) {
				room.doAllocationScore();									//���t���Ƶ��^�a�����a�ç�s���W����
				room.doMoveBackToReturn();								//�N�^�a�����a����w�^�a
			}
			//�S�H�^�a - �浹while�B�z
			else {
				continue;
			}
		}
		room.currentRound++;
	}
	room.finalResults();
	system("pause");
	return 0;
}

void Debugmsg(int p, int score, int pcur) {
	if (!DEBUG) return;
	Debugmsg(p, score, pcur, 0);
}
/*
 * �����T��
 */
void Debugmsg(int p, int score, int pcur, int ascore) {
	if (!DEBUG) return;
	cout << "���a #" << p
		<< "	 ��o�_�ۤ���: " << score
		<< "	 �ثe�`��o����: " << pcur;
	if (ascore)
		cout << "  ��o�����d ����: " << ascore;
	cout << endl;
}

/*
3~8�H(�����Φ^�a)
5�^�X
5�i�����d
15�i�_�åd�P15�i�Ǫ��d(�C�ةǪ��d�U3�i)

1.	�C�쪱�a���@�����d�B�@�i��a�d�M�@�y�b�O
2.	�N�����d�H����J�U�Ӧ^�X��

1.	�C�@�^�X�}�l�e�A�N�^�X���������d�~�J�����d�襤�A�ö}�l�C���C
2.	½�}�P�襤���Ĥ@�i�d��A��߰ݨC�@�쪱�a�O�_�~���I
3.	�_�åd�A�h�N�_�åd�W���Ʀr���������C�ӿ��"����"���a�A�ѤU�_�é�b�_�åd�W�C
4.	�����d�h�N������b�q�D�W�A�b�����d�W�����(�e�T�i�����@�i5���A�ĥ|�B���i���@�i10��) �A�u���b�@���ܦ^�a�A�~�ள�������A�_�h�N�~��d�b�q�D�W�C
5.	�Ǫ��d�A�X�{�⦸�A��ܱ��I�����a�A�Ҧ��٨S���^��a���_�éM���e�S�H�a���������N�������h�A�^�X�����C
6.	��^��a�����a�i�H�����b�q�D�W���_�áA�l�ƪ��_���~���b�q�D�W�A�o�쪺�_�é��b�סA�_�è�C�����������|�����C

�C����������:1.½���i�ۦP�a��2.�Ҧ����a��^��a�C

�C�����^�X������A�̦h�_�ê����a��ӡC

*/