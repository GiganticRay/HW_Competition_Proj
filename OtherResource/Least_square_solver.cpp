#include<iostream>
#include<iomanip>
#include<math.h>
#include<vector>
#include<algorithm>
#define m 19 //n<m
#define n 2

using namespace std;

void Least_squar_unit_cost(double Cpu_Mem_cost[m][n], double Solid_cost[m], double Unit_cost[n])
{
	/*线性消元变量声明*/ 
	double x[n]; 
	double mm[n];
	double T[n], BT = 0;
	double temp = 0;
	double t;
	int tap1 = 0, tap2 = 0;
	/*数据预处理*/ 
	double ATA[n][n];
	double ATy[n];
	for(int i = 0; i < n; i++)
	{
		ATy[i] = 0;
		for(int j = 0 ; j < n ; j++)
		{
			ATA[i][j] = 0;
		}
	}
	/*
	cout<<"超定方程组："<<endl;
	for(int i = 0; i < m; i++)
	{
		cout << Cpu_Mem_cost[i][0] << "x";
		cout.setf(ios::showpos);
		cout << Cpu_Mem_cost[i][1] << "=" << resetiosflags(ios::showpos) << Solid_cost[i] << endl;
	}
	*/
	for(int i = 0; i < n; i++)//计算ATA[n][n]
	{
		for(int j = 0; j < n; j++)
		{
			for(int k = 0; k < m; k++)
			{
				ATA[i][j] += Cpu_Mem_cost[k][i] * Cpu_Mem_cost[k][j];
			}
		}
	}
	/*
	cout << endl << "ATA[" << n << "][" << n << "]=" <<endl;
	for(int i = 0; i < n; i++)
	{
		for(int j = 0; j < n; j++)
		{
			cout << setw(15) << ATA[i][j];
		}
		cout << endl;
	}
	*/
	for(int i = 0 ; i < n; i++)//利用A[m][n]和b[m]来计算ATy[n]
	{
			for(int j = 0; j < m; j++)
			{
				ATy[i] += Cpu_Mem_cost[j][i] * Solid_cost[j];
				
			}
	}
	/*
	cout << endl << "ATy[" << n << "]=" << endl;
	for(int i = 0; i < n; i++)//输出ATy[n]
	{
		cout << setw(15) << ATy[i];
	}
	*/
	/*最小二乘*/ 
	int i, j, s;
	for (s = 0;s < n; s++)
	{
		t = ATA[s][s];
		for (int Unit_cost = s; Unit_cost < n; Unit_cost++)
		{
			if(fabs(ATA[Unit_cost][s]) > t)
			{
				t = ATA[Unit_cost][s];
				tap1 = Unit_cost;
				tap2 = s;
			}
		}
		if ( t == 0)
		{
			cout << "失败" << endl;
			break;
		}
		if ( tap1 != tap2)//换行
		{
			for( int i = s ; i < n ; i++)
			{
				T[i] = ATA[tap1][i];
				ATA[tap1][i] = ATA[s][i];
				ATA[s][i] = T[i];
			}
			BT = ATy[tap1];//给B换行
			ATy[tap1] = ATy[s];
			ATy[s] = BT;
		}
		for(i = s + 1; i < n; i++)//消元
		{
			mm[i] = ATA[i][s] / ATA[s][s];
			for(j = s; j < n; j++)
			{
				ATA[i][j] = ATA[i][j] - mm[i] * ATA[s][j];
			}
			ATy[i] = ATy[i] - mm[i] * ATy[s];
		}
	}
	x[n-1] = ATy[n-1] / ATA[n-1][n-1];//回代步
	for (i = (n - 1) -1; i >= 0; i--)
	{
		for ( j = i + 1; j < n; j++)
		{
			temp += ATA[i][j] * x[j];
		}
		x[i] = (ATy[i] - temp) / ATA[i][i];
        temp = 0;
	}
	for(i = 0; i < n; i++)
	{
		Unit_cost[i] = x[i];
	}
	
}