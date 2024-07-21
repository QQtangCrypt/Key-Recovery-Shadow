/*
* This is the key recovery for Shadow32
* This code is used to exact the information from the 12-round differential distinguisher, other distinguishers are similar.
*/
#include<fstream>
#include <iostream>
#include <time.h>
#include <math.h>
#include<string.h>

#include<iostream>
#include<cstdlib>
#include<cstdint>
#include<cmath>
#include<random>
#include<time.h>
#include<string.h>
using namespace std;
#define ROR(x,t) ((x>>t)|(x<<(8-t)))
#define ROL(x,t) ((x<<t)|(x>>(8-t)))
#define ROUND 16
#define BlockSize 8
/*
#define diff 1
#define diffpos1 0
#define diffpos2 6
#define keypos1 1
#define keypos2 5
#define keypos3 7
*/
#define diff 16
#define difftt 2
#define diffpos1 1
#define diffpos2 7
#define keypos1 0
#define keypos2 2
#define keypos3 6
//////////////////////////////////////////////// random number generator
uint32_t p = 0xffff;
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> dis(0, p - 1);
////////////////////////////////////////////////
static const unsigned char Bdiff[8] = { 0,0,0,0,0,1,0,0 };
static const unsigned char c0[16] = { 0 };
static const unsigned char c1[16] = { 0 };
static const unsigned char c2[16] = { 0 };
static const unsigned char c3[16] = { 0 };
static const unsigned char c4[16] = { 0 };
static const unsigned char P[64] = { 48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,4,5,6,7,8,9,10,11,12,13,14,15,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,0,1,2,3,16,17,18,19 };
static const unsigned char pk0[8] = { 0,1,2,3,8,9,10,11 };
static const unsigned char pk1[8] = { 4,5,6,7,12,13,14,15 };
static const unsigned char pk2[8] = { 16,17,18,19,24,25,26,27 };
static const unsigned char pk3[8] = { 20,21,22,23,28,29,30,31 };
static const unsigned char Pos[8][9] = { 1,0,2,3,0,0,0,0,0, 0,1,2,7,0,0,0,0,0, 0,1,6,0,1,2,3,5,7, 6,0,5,7,0,0,0,0,0, 0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0, 0,0,0,00,0,0,0,0 };

//Dai ma wen ti  cong kai shi dao jie wei jian cha yi xia
//unsigned char 0--2^8-2

void ToBinary(unsigned char v, unsigned char* B, int length)
{
    int i;
    for (i = 0; i < length; i++)
    {
        B[i] = 0;
    }
    for (i = 0; i < length; i++)
    {
        B[length - 1 - i] = v % 2;
        v = int(v / 2);
    }

}
void ToBinaryU(int v, unsigned char* B, int length)
{
    int i;
    for (i = 0; i < length; i++)
    {
        B[i] = 0;
    }
    for (i = 0; i < length; i++)
    {
        B[length - 1 - i] = v % 2;
        v = int(v / 2);
    }

}




void Extract(int t,int Kelength)
{
    int* CountKey = new int[65536];
    int NK = 4096;
    int temp;
    int sum_10_0, sum_10_1;
    int sum_9_0, sum_9_1;
    int sum_8_0, sum_8_1;
    int sum_7_0, sum_7_1;
    int sum_6_0, sum_6_1;
    int sum_5_0, sum_5_1;
    int sum_4_0, sum_4_1;
    int sum_3_0, sum_3_1;
    int sum_11_0, sum_11_1;
    int sum_12_0, sum_12_1;
    int sum_13_0, sum_13_1;
    int sum_14_0, sum_14_1;
    unsigned char key[64] = { 0 };
    int sumtest[64] = { 0 };
    //int t;
    //t = 13;



    ifstream fw;
    fw.open("Result.txt");
    for (int y = 0; y < NK; y++)
    {
        fw >> temp;
        ToBinaryU(temp, key, Kelength);
        if (key[t] == 0)
        {
            sumtest[t] = sumtest[t] + 1;
        }


    }
    fw.close();
    ofstream fout;
    string name;
    int tt;
    name = "00_out_result.txt";
    //name[0] = name[0] + 1;
    if (t < 10)
    {
        for (int x = 0; x < t; x++)
        {
            name[1] = name[1] + 1;
        }
    }
    if (t >= 10&&t<20)
    {
        tt = t - 10;
        name[0] = name[0] + 1;
        for (int x = 0; x < tt; x++)
        {
            name[1] = name[1] + 1;
        }
    }
    if (t >=20)
    {
        tt = t - 20;
        name[0] = name[0] + 2;
        for (int x = 0; x < tt; x++)
        {
            name[1] = name[1] + 1;
        }
    }


    fout.open(name);
    fout << "===============================================================" << endl;
    fout << "************The " << t << " -th bit************" << endl;
    fout << "###############################################################" << endl;
    fout << "The " << t << " -th, sum of equal to zero = " << sumtest[t] << endl;
    fout << "The " << t << " -th, rate of zero = " << double(1.0 * sumtest[t] / NK) << endl;
    fout << "===============================================================" << endl;
    fout.close();
    printf("===============================================================\n");
    printf("************The %d-th bit************\n", t);
    printf("###############################################################\n");
    printf("The %d-th, sum of equal to zero=%d\n", t, sumtest[t]);
    printf("The %d-th, rate of zero=%f\n", t, double(1.0*sumtest[t]/NK));
    printf("===============================================================\n");
    //printf("sum_9_0=%d,sum_10_0=%d\n",sum_9_0, sum_10_0);
    //printf("Sub=sum_9_0=%d,sum_10_0=%d\n",NK-sum_9_0, NK-sum_10_0);




}

int main()
{
    int index, Klength;
    Klength = 16;
    //index =17;
    for (index = 0; index < Klength; index++)
    {
        Extract(index, Klength);
        printf("\n");
    }
    
    

    return 0;
}