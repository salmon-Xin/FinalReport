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
	Room room(ret, DEFAULT_ROUND);		//�إߩж� �ê�l�� �ж��H�� �̤j�^�X�� ����
	//�P�_�O�_�̤j�^�X
	while (!room.isGameEnd())
	{
		cout << "------Round" << room.currentRound << "------\n"; //round 2�X��
		room.doRestart();							//���m���a���A(���I), ��e�^�X���Ƭ�0  <------- error
		//�P�_�^�X�O�_����, -1���C���~��
		while (room.isRoundEnd() == -1)	
		{
			//��P, �ç��쪺�P��J�� drawedCards, �M��P�_�O�_�X�{����ۦP���Ǫ�
			room.draw();
			if (DEBUG)
				cout << "½�}�F�d��!!  �d������: " << room.drawedCards.back().getCardType() << "  ����/����: " << room.drawedCards.back().score << endl;
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
					it =room.currentAdvanturePlayer.erase(it);
				}
				else {
					it++;
				}
			}
			if (DEBUG)
				cout << "�����^�a�H��: " << room.getBackNumber() << "   �~��_�I�H��: " << room.getAdventureNumber() << " \n";
			//�P�_�O�_����, �������ܴN�i��۹�B��
			if (room.isRoundEnd() != -1) {
				break;
			}
			// �@�H�^�a - ��o�����d, �ÿW�o�q�D�W�_��
			if (room.getBackNumber() == 1) {
				if (room.currentBackPlayer.back().state == true)	//���~�P�_
					throw "Error: �Ӧ^�a�����a�S�^�a, �ݨӥL�y�s�Ѫ�";
				(*room.currentBackPlayer.begin()).addScore(room.currentScore);
				Debugmsg(room.currentBackPlayer.begin()->index, room.currentScore, room.currentBackPlayer.begin()->score); //TODO ����
				room.currentScore = 0;
				room.doMoveBackToReturn();								//�N�^�a�����a����w�^�a
				continue;
			}
			//�h�H�^�a - �����d�d�b�q�D�W, �����_��
			else if (room.getBackNumber() > 1) {
				int returnCount = room.getBackNumber();			//���F�Ĳv�h�@���ܼ��x�s ��^�H��
				int	 score = room.currentScore / returnCount;		//�^�a���a��o����
				room.currentScore %= returnCount;						//���W�Ѿl�_�ۤ���
				for (int i = 0; i < room.getBackNumber(); i++) {					//�����^�a���a����
					room.currentBackPlayer.at(i).addScore(score);
					Debugmsg(room.currentBackPlayer.at(i).index, score, room.currentBackPlayer.at(i).score);
				}
				room.doMoveBackToReturn();								//�N�^�a�����a����w�^�a
				cout << "���W�l���_�ۤ���: " << room.currentScore << endl;
			}
			//�S�H�^�a - �浹while�B�z
			else {
				continue;
			}
		}
		room.currentRound++;
	}
	system("pause");
	return 0;
}

void Debugmsg(int p, int score, int pcur) {
	Debugmsg(p, score, pcur, 0);
}
/*
 * �����T��
 */
void Debugmsg(int p, int score, int pcur, int ascore) {
	if (!DEBUG) return;
	cout << "���a: " << p
		<< "	 ��o�_�ۤ���: " << score
		<< "	 �ثe����: " << pcur;
	if (ascore)
		cout << "  ��o�����d����:" << ascore;
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