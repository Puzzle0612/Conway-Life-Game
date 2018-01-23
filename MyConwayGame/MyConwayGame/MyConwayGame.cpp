// MyConwayGame.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<stdlib.h>
#include<iostream>
#include<fstream>
#include<string>
#include<time.h>

using namespace std;
//定义两个全局数组变量存放上一代和下一代的信息
#define ROW 50
#define COL 100
int thisGen[ROW][COL];
int nextGen[ROW][COL];

//读原始状态文件，存入数组
int readData() 
{
	int i, j;
	ifstream in;
	in.open("input_50x100");
	for (i = 0; i < 50; i++)
		for (j = 0; j < 100; j++)
			in >> thisGen[i][j];
	return 0;
}

//写结果文件，存入数组
int writeData()
{
	int i, j;
	ofstream out("output_50x100");
	for (i = 0; i <= 49; i++)
	{
		for (j = 0; j <= 99; j++)
			out << thisGen[i][j] << " ";
		out << endl;
	}
	return 0;
}

//对于周围元素不满8个的元素做边界检查
bool checkBound(int i, int j)
{
	if (i >= 0 && i <= 49 && j >= 0 && j <= 99)
		return 1;
	else
		return 0;
}

//统计八字格中的存活元素个数
int countLives(int i,int j)
{
	int count=0;
	int m, n;
	for (m = i - 1; m <= i + 1; m++)
		for (n = j - 1; n <= j + 1; n++)
			if (checkBound(i,j))
				if (thisGen[m][n] == 1)
					count++;
	if (thisGen[i][j] == 1)
		count--;
	return count;
}

//更新一代
int updateNext()
{
	int i = 0;
	int j = 0;
	int count;
	for(i=0;i<=49;i++)
		for (j = 0; j <= 99; j++)
		{
			count = countLives(i, j);
			if (thisGen[i][j] == 1)
				if (count < 2 || count>3)
					nextGen[i][j] = 0;
				else
					nextGen[i][j] = 1;
			else
				if (count == 3)
					nextGen[i][j] = 1;
				else
					nextGen[i][j] = 0;
		}
	return 1;
}

//主函数入口
int main(int argc,char* argv[])
{
	//设定计时器
	clock_t start, finish;
	double time_use;
	int i, j;
	readData();
	start = clock();
	//多次迭代
	for (int time = 0; time <= atoi(argv[1]); time++)
	{
		updateNext();
		for (i = 0; i <= 49; i++)
			for (j = 0; j <= 99; j++)
				thisGen[i][j] = nextGen[i][j];
	}
	finish = clock();
	time_use = (double)(finish - start) / CLOCKS_PER_SEC;
	//打印时间
	cout << "\nThe program takes about " << time_use << " seconds!" << endl;
	writeData();
    return 0;
}

