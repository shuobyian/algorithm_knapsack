#include <iostream>
#include <vector>
#include <math.h>
#include <ctime>
#include <queue>
#include <algorithm>
#include <random>
#include <Windows.h>
using namespace std;

#define student_num 1781 // �й� �� 4�ڸ�
#define item_size 10 // 5,10,15,20,25 ������ ����
#define weight_max 10 // �� ������ ���� �ִ� ��
#define value_max 50 // �� ������ ��ġ �ִ� ��
#define pop_size 20 // ������ ����
#define knapsack_weight 20 // �賶 �ִ� ���� ����
#define crossover_rate 0.85f // ���� ����
#define mutation_rate student_num*0.00001f // �������� ����(�й�)
#define max_epoch 100000 // �ִ� generation(epoch) ��

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
	int fitness; // ������
	int totalWeight; // ������ ��ü ����
	vector<int> chromosome; // ������ ����
};
int item[2][item_size]; // �������� ���Կ� ����
int totalFitness = 0; // �������� ����
void initItem() {
	srand(time(NULL));
	for (int i = 0; i < item_size; i++) {
		item[0][i] = rand() % weight_max + 1; // GA item ���� �Է�
		item[1][i] = rand() % value_max + 1;
		it[i].weight = item[0][i]; // DP item ���� �Է�
		it[i].price = item[1][i];
	}

} // item�� ����, ���� �ʱ�ȭ
vector<population> generation; // ���� ����
void Decode() {
	population p;
	for (int i = 0; i < pop_size; i++) {
		for (int j = 0; j < item_size; j++) {
			p.chromosome.push_back(rand() % 2); // 0 �Ǵ� 1
		}
		generation.push_back(p);
		p.chromosome.clear();
	}
} // ������ ���� �ʱ�ȭ (encoding ����)
void fitness() {
	for (int i = 0; i < pop_size; i++) {
		int fit = 0, weight = 0; // ������, ��ü ���� ���� �ʱ�ȭ
		for (int j = 0; j < item_size; j++) {
			if (generation[i].chromosome[j] == 1) {
				weight += item[0][j];
				fit += item[1][j];
			}
		}
		if (weight > knapsack_weight) fit = 0; // �ִ� ���� ���� �ʰ� �� ������ 0
		generation[i].totalWeight = weight;
		generation[i].fitness = fit;
		totalFitness += fit;
	}
} // ������ ��� (��ġ�� ��, ������)
population& selection() {
	double slice = randFloat() * totalFitness;
	double total = 0.0;
	int selected = 0;
	for (int i = 0; i < generation.size(); i++) {
		total += (double)generation[i].fitness / totalFitness;
		if (total > slice) {
			selected = i;
			break;
		} // ���õ� index ����
	}
	return generation[selected];
} // �귿 ����
void crossover(const vector<int> &parent1, const vector<int> &parent2,
	vector<int> &baby1, vector<int> &baby2)
{
	if (randFloat() > crossover_rate || parent1 == parent2) { // ���� ���� �ݿ�
		baby1 = parent1;
		baby2 = parent2;
		return;
	}
	int cp = randInt(0, item_size - 1); // ���� ���� ����
	for (int i = 0; i < cp; ++i)
	{
		baby1.push_back(parent1[i]);
		baby2.push_back(parent2[i]);
	} // ���� ���� ������ �״��
	for (int i = cp; i < parent1.size(); ++i)
	{
		baby1.push_back(parent2[i]);
		baby2.push_back(parent1[i]);
	} // �����Ͽ� ����
} // ����
void mutation(vector<int> &baby) {
	for (int i = 0; i < baby.size(); i++) {
		if (randFloat() < mutation_rate) // �������� ���� �ݿ�
			baby[i] = !baby[i];
	} // �����ϰ� ��Ʈ ����
} // ��������

vector<population> epoch() {
	int newBabies = 0;
	vector<population> babyGenomes;
	while (newBabies < pop_size) { // ������ ������ ������ �ÿ� break
		population parent1 = selection(); // �θ� ����
		population parent2 = selection();

		population baby1, baby2;
		crossover(parent1.chromosome, parent2.chromosome, baby1.chromosome, baby2.chromosome);
		// �ڽ� ������ ����
		mutation(baby1.chromosome);
		mutation(baby2.chromosome);
		// �ڽ� ������ ��������
		babyGenomes.push_back(baby1);
		babyGenomes.push_back(baby2);
		baby1.chromosome.clear();
		baby2.chromosome.clear();
		// ���ο� ���뿡 �ڽ� ������ push
		newBabies += 2;
	}
	return babyGenomes;
} // �� ���� ����

int main() {
	LARGE_INTEGER Frequency;	//�ð��Լ�
	LARGE_INTEGER BeginTime;
	LARGE_INTEGER Endtime;

	__int64 elapsed;
	double duringtime;

	QueryPerformanceFrequency(&Frequency);

	srand(unsigned(time(NULL)));
	initItem(); // ������ ���� ����
	cout << "item : " << item_size << ", knapsack max weight : " << knapsack_weight << endl;
	
	QueryPerformanceCounter(&BeginTime);
	Knapsack(it);
	QueryPerformanceCounter(&Endtime);
	elapsed = Endtime.QuadPart - BeginTime.QuadPart;
	duringtime = (double)elapsed / (double)Frequency.QuadPart;
	int DP_result = P[item_size][knapsack_weight];
	cout << "\nDP value : " << DP_result << endl;
	cout << "DP ���� �ð� : " << duringtime << endl;

	int temp = 0;
	int geneN = 0, ep = 0; 
	QueryPerformanceCounter(&BeginTime);
	Decode(); // 0���� ������ ����
	for (int i = 0; i <= max_epoch; i++) {
		fitness();
		cout << i << "����\n";
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
		generation = epoch(); // ������� ���ο� ���� ����
		cout << endl;
	}
	QueryPerformanceCounter(&Endtime);
	elapsed = Endtime.QuadPart - BeginTime.QuadPart;
	duringtime = (double)elapsed / (double)Frequency.QuadPart;
	cout << "\nGeneration : " << ep << "����\nGene bit : ";
	for (int k = 0; k < generation[geneN].chromosome.size(); k++)
		cout << generation[geneN].chromosome[k];
	cout << "\nGA value : " << temp << endl;
	cout << "GA ���� �ð� : " << duringtime << endl;
	return 0;
}