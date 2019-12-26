#include<iostream>
#include<string>
#include<vector>
#include<map>
#include <ctime>
using namespace std;
int  RandomNumber(int range,int min)
{
	srand((unsigned int)time(NULL));
	int x = rand() % range + min;
	return x;
}
int  draw(vector<string>vec)//抽排
{
	int x = RandomNumber(vec.size(), 0);
	cout << "card " << x << "	:" << vec.at(x) << endl;
	return x; 
}

int main()
{
	/*string s = "哈囉"; s+="a"; cout << s << endl;
	cout<<s.find("囉", 0)<<endl;
	cout << s.find("b", 0) << endl;*/
	vector<string> card;
	int Player, Round = 5;
	cout << "遊戲規則:\nbalabalabala...\n\n";//補遊戲規則
	cout << "請輸入遊玩人數(限制為3~8人):"; cin >> Player;
	int *PlayerScore = new int[Player];//玩家分數(動態配置)
	/**PlayerScore = 0;//0分開始計分*/
	for (int i = 0; i < Player; i++)
		PlayerScore[i] = 0;//0分開始計分
	map<string, int> JewelScore, Gift;
	string str; int i = -1;
	for (char ch = 'a'; ch < 'a' + 15; ch++)
	{
		string s; s += ch;//把字元轉字串
		str = "寶藏卡" + s;//cout << str << endl;
		card.push_back(str);//放入寶藏卡
		int x = RandomNumber(20, 1);/*亂數寶藏卡分數1~20*/
		JewelScore[str] = x; i++;
		cout << i << endl;
		if ((ch - 'a') % 3 == 0)
		{
			card.push_back("神器卡"); i++;
			cout << ".."<<i << endl;
		}//放入五張神器卡
	}
	for (int i = 1; i <= 3; i++)
	{//怪物卡
		card.push_back("Monster a");
		card.push_back("Monster b");
		card.push_back("Monster c");
		card.push_back("Monster d");
		card.push_back("Monster e");
	}
	//以上為牌堆中的東西
	/*int i = card.size();
	while (i--)//抽牌樣本
	{
		card.erase(card.begin() + draw(card));
	}*/
	while (Round--)
	{	
		cout << "------Round" << 5 - Round << "------\n";
		bool EndOfTurn = false, *Adventure = new bool[Player];//存取此回合是否繼續探險(動態配置)
		*Adventure = true;
		int AdventureCount = Player, Jewel = 0, GiftScore = 0;//Gift表神器卡,計算回家人數及寶物分數
		int *PlayerRoundScore = new int[Player];//玩家回合分數(動態配置)
		/**PlayerRoundScore = 0;//0分開始計分*/
		for(int i=0;i<Player;i++)
			PlayerRoundScore[i] = 0;//0分開始計分
		map<string, int> Monster;//紀錄場上怪物卡張數
		Monster["Monster a"] = 0;
		Monster["Monster b"] = 0;
		Monster["Monster c"] = 0;
		Monster["Monster d"] = 0;
		Monster["Monster e"] = 0;
		vector<string> PassagewayCard;//通道上的卡

		while (!EndOfTurn) //回合尚未結束
		{
			int HomeCount = 0;
			bool *end= new bool[Player];//結束回合
			*end = false;
			string GetCard=card[draw(card)];
			PassagewayCard.push_back(GetCard);
			if (GetCard.find("寶藏卡",0)<=10) { 
				Jewel += JewelScore[GetCard]; 
				cout << "寶石:" << Jewel << endl;
			}//判斷前三個字為寶藏卡
			else if (GetCard == "神器卡") { 
				Gift[GetCard] = 5; 
				GiftScore += Gift[GetCard];
				cout << "神器卡:" << GiftScore << endl;
			}//補前三張神器一張5分，第四、五張為一張10分,map寫
			else{
				Monster[GetCard]++;
				cout << GetCard << "數量:" << Monster[GetCard];
			}
			system("pause");
			if (Monster[GetCard] == 2) {
				/*兩張怪物卡，通道崩壞，丟棄通道上所有卡片，回合結束*/
				break;
			}
			for (int i = 0; i < Player; i++)//玩家操作part
			{
				if(Adventure[i] == false)continue;
				else {
					int t;
					do {
						cout << "請問玩家" << i + 1 << "是否繼續探險(輸入1或0)";
						cin >> t;
					} while (!(t == 1 || t == 0));
					Adventure[i] = t;
					if (Adventure[i] == false) {
						HomeCount++;//回合回家人數加一
						AdventureCount--;//回合冒險家減一
					}
				}

			}
			cout << "此次回家人數:" << HomeCount <<",繼續冒險人數:"<< AdventureCount << endl;
			if (HomeCount==1) {
				/*獲得神器卡，獨得通道上寶石*/
				for (int i = 0; i < Player; i++)
				{
					//if (end[i] == true)continue;
					if (end[i] == false && Adventure[i] == false)
					{
						PlayerScore[i] += (Jewel + GiftScore);
						cout << "玩家" << i + 1 << "獲得寶石分數:" << Jewel << ",獲得神器卡分數:"
							<< GiftScore << ",此回合獲得分數:" << PlayerScore[i] << endl;
						Jewel = 0;
						//card.erase(card.begin() + );//移除神器卡
						end[i] = true;
						break;
					}
				}

			}
			else {
				/*神器卡留在通道上，平分寶石*/
				for (int i = 0; i < Player; i++)
				{
					if (end[i] == false && Adventure[i] == false)
					{
						PlayerScore[i] += (Jewel / HomeCount);
						cout << "玩家" << i + 1 << "獲得寶石分數:" << Jewel / HomeCount << ",此回合獲得分數:" << PlayerScore[i] << endl;
						end[i] = true;//break;
					}
					/*else {
						PlayerRoundScore[i] = (Jewel % HomeCount) / AdventureCount;//留在通道上的寶石平分給繼續探險者
						cout << "玩家" << i + 1 << "目前回合獲得寶石分數:" << PlayerRoundScore[i] << endl;
					}*/
				}
				if (HomeCount != 0)Jewel %= HomeCount;//留在場上的寶石
				if (AdventureCount == 0)break;
				cout << "目前場上寶石:" << Jewel << ",場上冒險家各分得:" << Jewel / AdventureCount << "分" << endl;
			}//未完成:大家都回家,抽完的卡片刪掉的問題,已回家的玩家移除此回合的問題
		}
		

	}
	system("pause");
	return 0;
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