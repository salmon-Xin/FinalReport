#include "source.h"
using namespace std;

int main()
{
	int ret = 0;													//緩存區
	while (ret < 3 || ret > 8)
	{
		cout << "遊玩人數(限制為3~8人): ";
		cin >> ret;
	}
	Room room(ret);		//建立房間 並初始化 房間人數 最大回合數 分數
	//判斷是否最大回合
	while (!room.isGameEnd())
	{
		cout << " 回合 #" << room.currentRound << " 開始!\n"; //round 2出錯
		room.doRestart();							//重置玩家狀態(探險), 回合分數為0
		//判斷回合是否結束, -1為遊戲繼續
		while (room.isRoundEnd() == -1)	
		{
			room.draw();		//抽牌
			if (DEBUG && room.drawedCards.back().cardType == CardType::Monster)	
				cout << "翻開了卡片!!  卡片類型: " << room.drawedCards.back().getCardType() << "  種類: " << room.drawedCards.back().score << endl;
			else if (DEBUG && (room.drawedCards.back().cardType == CardType::Treasure || room.drawedCards.back().cardType == CardType::Treasure))
				cout << "翻開了卡片!!  卡片類型: " << room.drawedCards.back().getCardType() << "  分數: " << room.drawedCards.back().score << endl;

			//判斷是否結束, 結束的話就進行相對處裡
			if (room.isRoundEnd() != -1) { break; }
			// 取得每位冒險玩家的操作
			for (auto it = room.currentAdvanturePlayer.begin(); it != room.currentAdvanturePlayer.end();)
			{
				//詢問是否繼續
				do {
					cout << "請問玩家 #" << (*it).index << " 是否繼續探險(輸入1或0): ";
					cin >> ret;
				} while (!(ret == 1 || ret == 0));	//錯誤處理
				if (((*it).state = ret) == false) {	//將冒險玩家移動到當前回家玩家(孬種)
					room.currentBackPlayer.insert(room.currentBackPlayer.end(), *it);
					it = room.currentAdvanturePlayer.erase(it);
				}
				else {
					it++;
				}
			}
			if (DEBUG) cout << "此次回家人數: " << room.getBackNumber() << "     繼續冒險人數: " << room.getAdventureNumber() << " \n";
			// 一人回家 - 獲得神器卡, 並獨得通道上寶石
			if (room.getBackNumber() == 1) {
				if (room.currentBackPlayer.back().state == true)	//錯誤判斷
					throw "Error: 該回家的玩家沒回家, 看來他流連忘返";
				room.doAllocationScore();									//分配分數給回家的玩家並更新場上分數
				room.doMoveBackToReturn();								//將回家的玩家移到已回家
			}
			//多人回家 - 神器卡留在通道上, 平分寶石
			else if (room.getBackNumber() > 1) {
				room.doAllocationScore();									//分配分數給回家的玩家並更新場上分數
				room.doMoveBackToReturn();								//將回家的玩家移到已回家
			}
			//沒人回家 - 交給while處理
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
 * 除錯訊息
 */
void Debugmsg(int p, int score, int pcur, int ascore) {
	if (!DEBUG) return;
	cout << "玩家 #" << p
		<< "	 獲得寶石分數: " << score
		<< "	 目前總獲得分數: " << pcur;
	if (ascore)
		cout << "  獲得神器卡 分數: " << ascore;
	cout << endl;
}

/*
3~8人(探索或回家)
5回合
5張神器卡
15張寶藏卡與15張怪物卡(每種怪物卡各3張)

1.	每位玩家拿一探索卡、一張營地卡和一座帳篷
2.	將神器卡隨機放入各個回合中

1.	每一回合開始前，將回合中的神器卡洗入探索卡堆中，並開始遊戲。
2.	翻開牌堆中的第一張卡後，後詢問每一位玩家是否繼續探險
3.	寶藏卡，則將寶藏卡上的數字先平分給每個選擇"探索"玩家，剩下寶藏放在寶藏卡上。
4.	神器卡則將神器放在通道上，在神器卡上放分數(前三張神器一張5分，第四、五張為一張10分) ，只有在一位選擇回家，才能拿走神器，否則將繼續留在通道上。
5.	怪物卡，出現兩次，選擇探險的玩家，所有還沒拿回營地的寶藏和之前沒人帶走的神器將全部失去，回合結束。
6.	返回營地的玩家可以均分在通道上的寶藏，餘數的寶藏繼續放在通道上，得到的寶藏放到帳棚，寶藏到遊戲結束都不會消失。

遊戲結束條件:1.翻到兩張相同災難2.所有玩家返回營地。

遊戲五回合結束後，最多寶藏的玩家獲勝。

*/