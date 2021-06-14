#include <iostream>
#include <ctime>
#include <Windows.h>
using namespace std;

#define item_size 5 // 물건의 개수
#define knapsack_weight 10 // 배낭에 넣을 수 있는 총 무게
#define weight_max 10 // 한 아이템 무게 최대 값
#define value_max 50 // 한 아이템 가치 최대 값

#define CHECK_TIME_START __int64 freq, start, end; if (QueryPerformanceFrequency((_LARGE_INTEGER*)&freq)) {QueryPerformanceCounter((_LARGE_INTEGER*)&start);
#define CHECK_TIME_END(a,b) QueryPerformanceCounter((_LARGE_INTEGER*)&end); a=(float)((double)(end - start)/freq*1000); b=TRUE; } else b=FALSE;

struct object {
	int weight;
	int price;
};

int P[item_size + 1][knapsack_weight] = { 0 }; // 전체 무게가 w가 넘지 않도록 i번째 까지의 항목 중 얻어진 최고의 이익 P[i][w]
object it[item_size+1];

void initItem() {
	srand(time(NULL));
	for (int i = 1; i <= item_size; i++) {
		it[i].weight = rand() % weight_max + 1;
		it[i].price = rand() % value_max + 1;
		//cout << i << "의 무게 : " << it[i].weight << ", 가치 : " << it[i].price << endl;
	}
}

int maximum(int a, int b) {
	return (a < b) ? b : a;
}

void Knapsack(object item[]) {
	for (int i = 0; i <= knapsack_weight; i++) {
		if (i >= item[1].weight)
			P[1][i] = item[1].price;
	}
	for (int i = 1; i <= item_size; i++) {
		for (int j = 0; j <= knapsack_weight; j++) {
			if (j >= item[i].weight) {
				P[i][j] = maximum(P[i - 1][j], item[i].price + P[i - 1][j - item[i].weight]); // recursive property
			}
			else {
				P[i][j] = P[i - 1][j];
			}
		}
	}
}

int main() {
	float Time;
	BOOL err;

	initItem();
	CHECK_TIME_START;
	srand((unsigned int)time(NULL));
	Knapsack(it);
	CHECK_TIME_END(Time, err);
	cout << "item : " << item_size << ", knapsack max weight : " << knapsack_weight << endl;
	
	for (int i = 1; i <= item_size; i++) {
		for (int j = 0; j <= knapsack_weight; j++) {
			cout << P[i][j] << " ";
		}
		cout << endl;
	}
	cout << "DP value : " << P[item_size][knapsack_weight] << endl;
	cout << "DP 수행 시간 : " << Time << "ms" << endl;
	return 0;
}