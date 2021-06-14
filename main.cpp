#include <iostream>
#include <vector>
#include <math.h>
#include <ctime>
#include <queue>
#include <algorithm>
#include <random>
#include <Windows.h>
using namespace std;

#define student_num 1781 // 학번 뒤 4자리
#define item_size 10 // 5,10,15,20,25 아이템 개수
#define weight_max 10 // 한 아이템 무게 최대 값
#define value_max 50 // 한 아이템 가치 최대 값
#define pop_size 20 // 유전자 개수
#define knapsack_weight 20 // 배낭 최대 수용 무게
#define crossover_rate 0.85f // 교차 비율
#define mutation_rate student_num*0.00001f // 돌연변이 비율(학번)
#define max_epoch 100000 // 최대 generation(epoch) 수

int randInt(int x, int y) { return rand() % (y - x + 1) + x; }
double randFloat() { return (rand()) / (RAND_MAX + 1.0); }

struct object {
	int weight;
	int price;
};
int P[item_size + 1][knapsack_weight] = { 0 };
object it[item_size];

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
				P[i][j] = maximum(P[i - 1][j], item[i].price + P[i - 1][j - item[i].weight]);
			}
			else {
				P[i][j] = P[i - 1][j];
			}
		}
	}
}

struct population {
	int fitness; // 적응도
	int totalWeight; // 유전자 전체 무게
	vector<int> chromosome; // 유전자 정보
};
int item[2][item_size]; // 아이템의 무게와 가격
int totalFitness = 0; // 적응도의 총합
void initItem() {
	srand(time(NULL));
	for (int i = 0; i < item_size; i++) {
		item[0][i] = rand() % weight_max + 1; // GA item 정보 입력
		item[1][i] = rand() % value_max + 1;
		it[i].weight = item[0][i]; // DP item 정보 입력
		it[i].price = item[1][i];
	}

} // item의 무게, 가격 초기화
vector<population> generation; // 세대 저장
void Decode() {
	population p;
	for (int i = 0; i < pop_size; i++) {
		for (int j = 0; j < item_size; j++) {
			p.chromosome.push_back(rand() % 2); // 0 또는 1
		}
		generation.push_back(p);
		p.chromosome.clear();
	}
} // 유전자 정보 초기화 (encoding 과정)
void fitness() {
	for (int i = 0; i < pop_size; i++) {
		int fit = 0, weight = 0; // 적응도, 전체 무게 변수 초기화
		for (int j = 0; j < item_size; j++) {
			if (generation[i].chromosome[j] == 1) {
				weight += item[0][j];
				fit += item[1][j];
			}
		}
		if (weight > knapsack_weight) fit = 0; // 최대 수용 무게 초과 시 적응도 0
		generation[i].totalWeight = weight;
		generation[i].fitness = fit;
		totalFitness += fit;
	}
} // 적응도 계산 (가치가 즉, 적응도)
population& selection() {
	double slice = randFloat() * totalFitness;
	double total = 0.0;
	int selected = 0;
	for (int i = 0; i < generation.size(); i++) {
		total += (double)generation[i].fitness / totalFitness;
		if (total > slice) {
			selected = i;
			break;
		} // 선택된 index 저장
	}
	return generation[selected];
} // 룰렛 선택
void crossover(const vector<int> &parent1, const vector<int> &parent2,
	vector<int> &baby1, vector<int> &baby2)
{
	if (randFloat() > crossover_rate || parent1 == parent2) { // 교차 비율 반영
		baby1 = parent1;
		baby2 = parent2;
		return;
	}
	int cp = randInt(0, item_size - 1); // 교차 지점 선택
	for (int i = 0; i < cp; ++i)
	{
		baby1.push_back(parent1[i]);
		baby2.push_back(parent2[i]);
	} // 교차 지점 전까지 그대로
	for (int i = cp; i < parent1.size(); ++i)
	{
		baby1.push_back(parent2[i]);
		baby2.push_back(parent1[i]);
	} // 교차하여 저장
} // 교차
void mutation(vector<int> &baby) {
	for (int i = 0; i < baby.size(); i++) {
		if (randFloat() < mutation_rate) // 돌연변이 비율 반영
			baby[i] = !baby[i];
	} // 랜덤하게 비트 반전
} // 돌연변이

vector<population> epoch() {
	int newBabies = 0;
	vector<population> babyGenomes;
	while (newBabies < pop_size) { // 유전자 개수에 도달할 시에 break
		population parent1 = selection(); // 부모 선택
		population parent2 = selection();

		population baby1, baby2;
		crossover(parent1.chromosome, parent2.chromosome, baby1.chromosome, baby2.chromosome);
		// 자식 유전자 교차
		mutation(baby1.chromosome);
		mutation(baby2.chromosome);
		// 자식 유전자 돌연변이
		babyGenomes.push_back(baby1);
		babyGenomes.push_back(baby2);
		baby1.chromosome.clear();
		baby2.chromosome.clear();
		// 새로운 세대에 자식 유전자 push
		newBabies += 2;
	}
	return babyGenomes;
} // 한 세대 실행

int main() {
	LARGE_INTEGER Frequency;	//시간함수
	LARGE_INTEGER BeginTime;
	LARGE_INTEGER Endtime;

	__int64 elapsed;
	double duringtime;

	QueryPerformanceFrequency(&Frequency);

	srand(unsigned(time(NULL)));
	initItem(); // 아이템 정보 생성
	cout << "item : " << item_size << ", knapsack max weight : " << knapsack_weight << endl;
	
	QueryPerformanceCounter(&BeginTime);
	Knapsack(it);
	QueryPerformanceCounter(&Endtime);
	elapsed = Endtime.QuadPart - BeginTime.QuadPart;
	duringtime = (double)elapsed / (double)Frequency.QuadPart;
	int DP_result = P[item_size][knapsack_weight];
	cout << "\nDP value : " << DP_result << endl;
	cout << "DP 수행 시간 : " << duringtime << endl;

	int temp = 0;
	int geneN = 0, ep = 0; 
	QueryPerformanceCounter(&BeginTime);
	Decode(); // 0세대 유전자 생성
	for (int i = 0; i <= max_epoch; i++) {
		fitness();
		cout << i << "세대\n";
		for (int m = 0; m < pop_size; m++) {
			for (int n = 0; n < item_size; n++) {
				cout << generation[m].chromosome[n];
			}
			//cout << "\nweight : " << generation[m].totalWeight << ", value : " << generation[m].fitness << endl;
			cout << endl;
			if (generation[m].fitness == DP_result) {
				temp = generation[m].fitness;
				geneN = m;
				ep = i;
				break;
			}
		}
		if (temp == DP_result)
			break;
		generation = epoch(); // 만들어진 새로운 세대 저장
		cout << endl;
	}
	QueryPerformanceCounter(&Endtime);
	elapsed = Endtime.QuadPart - BeginTime.QuadPart;
	duringtime = (double)elapsed / (double)Frequency.QuadPart;
	cout << "\nGeneration : " << ep << "세대\nGene bit : ";
	for (int k = 0; k < generation[geneN].chromosome.size(); k++)
		cout << generation[geneN].chromosome[k];
	cout << "\nGA value : " << temp << endl;
	cout << "GA 수행 시간 : " << duringtime << endl;
	return 0;
}