/*
* This is the key recovery for Shadow32
* This code is the key guessing using the 11-round differential distinguisher with the difference (0x00,0x04,0x00,0x04)
* Key guessing using other distinguishers are similar
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
using namespace std;
#define ROR(x,t) ((x>>t)|(x<<(8-t)))
#define ROL(x,t) ((x<<t)|(x>>(8-t)))
#define ROUND 16
#define BlockSize 8

#define diff 2
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
//static const unsigned char P[64] = { 48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,4,5,6,7,8,9,10,11,12,13,14,15,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,0,1,2,3,16,17,18,19 };
static const unsigned char P[64] = { 56,57,58,59,16,17,18,19,20,21,22,23,24,25,26,27,60,61,62,63,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };
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

unsigned char ToInteger(unsigned char* B, int length)
{
    unsigned char out;
    int i, t;
    t = 1;
    out = 0;
    for (i = 0; i < length; i++)
    {
        out = out + t * B[length - 1 - i];
        t = t * 2;
    }
    return out;

}
void Shadow_Enc_One_Round(unsigned char& L0, unsigned char& R0, unsigned char& L1, unsigned char& R1, unsigned char k0, unsigned char k1, unsigned char k2, unsigned char k3)
{
    unsigned char r_1_L0, r_7_L0, r_2_L0, r_1_L1, r_7_L1, r_2_L1;
    unsigned char temp0, temp1;
    r_1_L0 = ROL(L0, 1);
    r_2_L0 = ROL(L0, 2);
    r_7_L0 = ROL(L0, 7);
    //printf("%d %d\n", L0, R0);
    temp0 = (r_1_L0 & r_7_L0) ^ R0 ^ r_2_L0 ^ k0;
    //printf("temp0=%d\n", temp0);
    R0 = L0;
    L0 = temp0;
    //printf("Part 1\n");
    //printf("%d %d\n", L0, R0);
    r_1_L1 = ROL(L1, 1);
    r_2_L1 = ROL(L1, 2);
    r_7_L1 = ROL(L1, 7);
    temp1 = (r_1_L1 & r_7_L1) ^ R1 ^ r_2_L1 ^ k1;
    R1 = L1;
    L1 = temp1;
    //printf("middle=L0=%d,R0=%d,L1=%d,R1=%d\n", L0, R0, L1, R1);
    //printf("Part 2\n");
    //printf("%d %d\n", L1, R1);

    r_1_L0 = ROL(L0, 1);
    r_2_L0 = ROL(L0, 2);
    r_7_L0 = ROL(L0, 7);
    R0 = (r_1_L0 & r_7_L0) ^ R0 ^ r_2_L0 ^ k2;
    r_1_L1 = ROL(L1, 1);
    r_2_L1 = ROL(L1, 2);
    r_7_L1 = ROL(L1, 7);
    R1 = (r_1_L1 & r_7_L1) ^ R1 ^ r_2_L1 ^ k3;
    temp0 = L0;
    L0 = L1;
    L1 = temp0;
    //printf("Part 1\n");
    //printf("%d %d\n", L0, R0);
    //printf("Part 2\n");
    //printf("%d %d\n", L1, R1);

}

void Shadow_Dnc_One_Round(unsigned char& L0, unsigned char& R0, unsigned char& L1, unsigned char& R1, unsigned char k0, unsigned char k1, unsigned char k2, unsigned char k3)
{
    unsigned char r_1_L0, r_7_L0, r_2_L0, r_1_L1, r_7_L1, r_2_L1;
    unsigned char temp0, temp1, temp;
    r_1_L0 = ROL(L0, 1);
    r_2_L0 = ROL(L0, 2);
    r_7_L0 = ROL(L0, 7);

    r_1_L1 = ROL(L1, 1);
    r_2_L1 = ROL(L1, 2);
    r_7_L1 = ROL(L1, 7);

    temp0 = (r_1_L0 & r_7_L0) ^ R1 ^ r_2_L0 ^ k3;
    temp1 = (r_1_L1 & r_7_L1) ^ R0 ^ r_2_L1 ^ k2;

    //printf("temp0=%d\n", temp0);
    temp = L0;
    L0 = L1;
    R0 = temp1;
    //printf("Part 1\n");
    //printf("%d %d\n", L0, R0);
    L1 = temp;
    R1 = temp0;
    //printf("middle=L0=%d,R0=%d,L1=%d,R1=%d\n", L0, R0, L1, R1);
    //printf("Part 2\n");
    //printf("%d %d\n", L1, R1);

    r_1_L0 = ROL(R0, 1);
    r_2_L0 = ROL(R0, 2);
    r_7_L0 = ROL(R0, 7);

    r_1_L1 = ROL(R1, 1);
    r_2_L1 = ROL(R1, 2);
    r_7_L1 = ROL(R1, 7);
    temp0 = (r_1_L0 & r_7_L0) ^ L0 ^ r_2_L0 ^ k0;
    temp1 = (r_1_L1 & r_7_L1) ^ L1 ^ r_2_L1 ^ k1;
    L0 = R0;
    L1 = R1;
    R0 = temp0;
    R1 = temp1;

    //printf("Part 1\n");
    //printf("%d %d\n", L0, R0);
    //printf("Part 2\n");
    //printf("%d %d\n", L1, R1);

}
void Shadow_Dnc_One_Round_Half(unsigned char& L0, unsigned char& R0, unsigned char& L1, unsigned char& R1, unsigned char k0, unsigned char k1, unsigned char k2, unsigned char k3)
{
    unsigned char r_1_L0, r_7_L0, r_2_L0, r_1_L1, r_7_L1, r_2_L1;
    unsigned char temp0, temp1, temp;
    r_1_L0 = ROL(L0, 1);
    r_2_L0 = ROL(L0, 2);
    r_7_L0 = ROL(L0, 7);

    r_1_L1 = ROL(L1, 1);
    r_2_L1 = ROL(L1, 2);
    r_7_L1 = ROL(L1, 7);

    temp0 = (r_1_L0 & r_7_L0) ^ R1 ^ r_2_L0 ^ k3;
    temp1 = (r_1_L1 & r_7_L1) ^ R0 ^ r_2_L1 ^ k2;

    //printf("temp0=%d\n", temp0);
    temp = L0;
    L0 = L1;
    R0 = temp1;
    //printf("Part 1\n");
    //printf("%d %d\n", L0, R0);
    L1 = temp;
    R1 = temp0;
    //printf("middle=L0=%d,R0=%d,L1=%d,R1=%d\n", L0, R0, L1, R1);
    //printf("Part 2\n");
    //printf("%d %d\n", L1, R1);



    //printf("Part 1\n");
    //printf("%d %d\n", L0, R0);
    //printf("Part 2\n");
    //printf("%d %d\n", L1, R1);

}

void KeySchedule(unsigned long long mk, unsigned char* k0, unsigned char* k1, unsigned char* k2, unsigned char* k3)
{
    int round, i;
    unsigned char K[64] = { 0 };
    unsigned char AC[64] = { 0 };//the key after add round constant
    unsigned char NX[64] = { 0 };//the key after NX operation
    //K[64] also represent the key after the permutation
    unsigned char key0[8] = { 0 };
    unsigned char key1[8] = { 0 };
    unsigned char key2[8] = { 0 };
    unsigned char key3[8] = { 0 };
    ToBinaryU(mk, K, 64);

    //printf("master key=ll%\n", mk);
    for (round = 0; round < ROUND; round++)
    {

        //printf("Round=%d\n", round);
        //AddRoundconstant
        for (i = 0; i < 64; i++)
        {
            if (i == 3)
            {
                AC[i] = K[i] ^ c0[round];
            }
            else if (i == 4)
            {
                AC[i] = K[i] ^ c1[round];
            }
            else if (i == 5)
            {
                AC[i] = K[i] ^ c2[round];
            }
            else if (i == 6)
            {
                AC[i] = K[i] ^ c3[round];
            }
            else if (i == 7)
            {
                AC[i] = K[i] ^ c4[round];
            }
            else
            {
                AC[i] = K[i];
            }
        }
        //printf("AddConstant\n");
        //for (i = 0; i < 64; i++)
       // {
            //printf("%d", AC[i]);
       // }
        //printf("\n");
        //NX operation
        NX[56] = AC[56] & (AC[56] ^ AC[62]);
        NX[57] = AC[57] & (AC[57] ^ AC[63]);
        NX[58] = AC[58] & (AC[58] ^ AC[56] ^ AC[62]);
        NX[59] = AC[59] & (AC[59] ^ AC[57] ^ AC[63]);
        NX[60] = AC[60] & (AC[60] ^ AC[58] ^ AC[56] ^ AC[62]);
        NX[61] = AC[61] & (AC[61] ^ AC[59] ^ AC[57] ^ AC[63]);
        NX[62] = AC[62] & (AC[62] ^ AC[60] ^ AC[58] ^ AC[56] ^ AC[62]);
        NX[63] = AC[63] & (AC[63] ^ AC[61] ^ AC[59] ^ AC[57] ^ AC[63]);
        for (i = 0; i < 56; i++)
        {
            NX[i] = AC[i];
        }
        //printf("NX\n");
        //for (i = 0; i < 64; i++)
        //{
           // printf("%d", NX[i]);
        //}
        //printf("\n");
        //Permutation
        for (i = 0; i < 64; i++)
        {
            K[i] = NX[P[i]];
        }
        //printf("Permutation\n");
        //for (i = 0; i < 64; i++)
        //{
            //printf("%d", K[i]);
       // }
        //printf("\n");
        //Give the value to subkey
        for (i = 0; i < 8; i++)
        {
            key0[i] = K[pk0[i]];
            key1[i] = K[pk1[i]];
            key2[i] = K[pk2[i]];
            key3[i] = K[pk3[i]];
        }
        k0[round] = ToInteger(key0, 8);
        k1[round] = ToInteger(key1, 8);
        k2[round] = ToInteger(key2, 8);
        k3[round] = ToInteger(key3, 8);
        //printf("k0=%d,k1=%d,k2=%d,k3=%d\n", k0[round], k1[round], k2[round], k3[round]);

    }
}

void Shadow_Enc(unsigned char L0in, unsigned char R0in, unsigned char L1in, unsigned char R1in, unsigned char& L0out, unsigned char& R0out, unsigned char& L1out, unsigned char& R1out, unsigned long long mk)
{
    int r, i;
    unsigned char k0[ROUND] = { 0 };
    unsigned char k1[ROUND] = { 0 };
    unsigned char k2[ROUND] = { 0 };
    unsigned char k3[ROUND] = { 0 };
    KeySchedule(mk, k0, k1, k2, k3);

    //printf("msaterkey=%x\n", mk);
    //for (i = 0; i < ROUND; i++)
    //{
        //printf("Round=%d,k0=%d,k1=%d,k2=%d,k3=%d\n", i, k0[i], k1[i], k2[i], k3[i]);
    //}
    //printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    for (r = 0; r < ROUND; r++)
    {
        Shadow_Enc_One_Round(L0in, R0in, L1in, R1in, k2[ROUND-1-r], k3[ROUND - 1 - r], k0[ROUND - 1 - r], k1[ROUND - 1 - r]);

        //Shadow_Enc_One_Round(L0in, R0in, L1in, R1in, k0[r], k1[r], k2[r], k3[r]);
        //printf("Round=%d,L0=%d,R0=%d,L1=%d,R1=%d\n",r, L0in, R0in, L1in, R1in);
        //printf("Round=%d,k0=%d,k1=%d,k2=%d,k3=%d\n", r, k2[ROUND - 1 - r], k3[ROUND - 1 - r], k0[ROUND - 1 - r], k1[ROUND - 1 - r]);
    }
    //printf("***********************************************\n");
    L0out = L0in;
    R0out = R0in;
    L1out = L1in;
    R1out = R1in;

}
void CollectPhase(int& count, int N, unsigned long long mk, unsigned char dL0in, unsigned char dR0in, unsigned char dL1in, unsigned char dR1in, int* cL0, int* cLL0, int* cR0, int* cRR0, int* cL1, int* cLL1, int* cR1, int* cRR1)
{

    int n;
    //dL0--dR1 is the output difference after 16 round
    unsigned char L0, LL0, R0, RR0, L1, LL1, R1, RR1, L0_16, LL0_16, R0_16, RR0_16, L1_16, LL1_16, R1_16, RR1_16, mL0and_15, mLL0and_15, mL1and_15, mLL1and_15, mL0and_0, mLL0and_0, mL1and_0, mLL1and_0, dL0_16, dL1_16, dR0_16, dR1_16, dmL0and_15, dmL1and_15, dmR0_15, dmR1_15;
    unsigned char BdL0_16[8] = { 0 };
    unsigned char BdL1_16[8] = { 0 };
    unsigned char BdR0_16[8] = { 0 };
    unsigned char BdR1_16[8] = { 0 };
    unsigned char BdL0_0[8] = { 0 };
    unsigned char BdL1_0[8] = { 0 };
    unsigned char BdR0_0[8] = { 0 };
    unsigned char BdR1_0[8] = { 0 };
    unsigned char BLL0[8] = { 0 };
    unsigned char BLL1[8] = { 0 };
    unsigned char BRR0[8] = { 0 };
    unsigned char BRR1[8] = { 0 };
    unsigned char BL0[8] = { 0 };
    unsigned char BL1[8] = { 0 };
    unsigned char BR0[8] = { 0 };
    unsigned char BR1[8] = { 0 };
    unsigned char BLL0_16[8] = { 0 };
    unsigned char BLL1_16[8] = { 0 };
    unsigned char BRR0_16[8] = { 0 };
    unsigned char BRR1_16[8] = { 0 };
    unsigned char BL0_16[8] = { 0 };
    unsigned char BL1_16[8] = { 0 };
    unsigned char BR0_16[8] = { 0 };
    unsigned char BR1_16[8] = { 0 };
    unsigned char BdmR0_15[8] = { 0 };
    unsigned char BdmR1_15[8] = { 0 };
    unsigned char ran1, ran2, rans, dL0_0, dL1_0, dR0_0, dR1_0, dLL0_0, dLL1_0, dRR0_0, dRR1_1;
    int ssss;
    ssss = 0;
    unsigned char dmL0and_0, dmL1and_0, dR0_1, dR1_1, dmR0_1, dmR1_1;
    count = 0;
    for (n = 0; n < N; n++)
    {
        //the input of random
        //printf("Start the encryption!\n");
        L0 = dis(gen);
        R0 = dis(gen);
        L1 = dis(gen);
        R1 = dis(gen);
        //input XOR with input difference
        LL0 = L0 ^ diff;
        RR0 = R0 ^ 8;
        LL1 = L1 ^ diff;
        RR1 = R1 ^ 8;
        ran1 = dis(gen) % 2;
        ran2 = dis(gen) % 2;
        rans = ran1 * 4 + ran2 * 1;
        RR0 = RR0 ^ rans;
        ran1 = dis(gen) % 2;
        ran2 = dis(gen) % 2;
        rans = ran1 * 4+ ran2 * 1;
        RR1 = RR1 ^ rans;

        dL0_0 = L0 ^ LL0;
        dL1_0 = L1 ^ LL1;
        dR0_0 = R0 ^ RR0;
        dR1_0 = R1 ^ RR1;

        ToBinary(L0, BL0, 8);
        ToBinary(R0, BR0, 8);
        ToBinary(L1, BL1, 8);
        ToBinary(R1, BR1, 8);

        ToBinary(LL0, BLL0, 8);
        ToBinary(RR0, BRR0, 8);
        ToBinary(LL1, BLL1, 8);
        ToBinary(RR1, BRR1, 8);

        ToBinary(dL0_0, BdL0_0, 8);
        ToBinary(dR0_0, BdR0_0, 8);
        ToBinary(dL1_0, BdL1_0, 8);
        ToBinary(dR1_0, BdR1_0, 8);

        //The difference in plaintext is no problem
        /*
        printf("\n");
        printf("The %d-th pair of plaintext!\n",n);
        printf("L0=");
        for (int m = 0; m < BlockSize; m++)
        {
            printf("%d", BL0[m]);
        }
        printf(";  R0=");
        for (int m = 0; m < BlockSize; m++)
        {
            printf("%d", BR0[m]);
        }
        printf(";  L1=");
        for (int m = 0; m < BlockSize; m++)
        {
            printf("%d", BL1[m]);
        }
        printf(";  R1=");
        for (int m = 0; m < BlockSize; m++)
        {
            printf("%d", BR1[m]);
        }
        printf("\n");
        printf("LL0=");
        for (int m = 0; m < BlockSize; m++)
        {
            printf("%d", BLL0[m]);
        }
        printf("; RR0=");
        for (int m = 0; m < BlockSize; m++)
        {
            printf("%d", BRR0[m]);
        }
        printf("; LL1=");
        for (int m = 0; m < BlockSize; m++)
        {
            printf("%d", BLL1[m]);
        }
        printf("; RR1=");
        for (int m = 0; m < BlockSize; m++)
        {
            printf("%d", BRR1[m]);
        }
        printf("\n");
        */

        mL0and_0 = (ROL(L0, 1) & (ROL(L0, 7)));
        mLL0and_0 = (ROL(LL0, 1) & (ROL(LL0, 7)));
        mL1and_0 = (ROL(L1, 1) & (ROL(L1, 7)));
        mLL1and_0 = (ROL(LL1, 1) & (ROL(LL1, 7)));
        dmL0and_0 = mL0and_0 ^ mLL0and_0;
        dmL1and_0 = mL1and_0 ^ mLL1and_0;
        dmR0_1 = dR0_0 ^ (ROL(dL0_0, 2)) ^ dmL0and_0;
        dmR1_1 = dR1_0 ^ (ROL(dL1_0, 2)) ^ dmL1and_0;
        /*
        printf("mL0and=%d,mLL0and=%d,dmL0and=%d\n", mL0and_0, mLL0and_0, dmL0and_0);
        printf("mL1and=%d,mLL1and=%d,dmL1and=%d\n", mL1and_0, mLL1and_0, dmL1and_0);
        printf("dmR0=%d,dmR1=%d\n", dmR0_1, dmR1_1);
        */
        if ((dmR0_1 == 0) && (dmR1_1 == 0))
        {

            //kan xia yi lun hou de L0,L1 shi fou deng yu LL0,LL1
            ssss = ssss + 1;
            Shadow_Enc(L0, R0, L1, R1, L0_16, R0_16, L1_16, R1_16, mk);
            //printf("+++++++++++++++++++++++++++++++++++++++++++++++++\n");
            //printf("The encryption for LL,RR\n");
            //printf("PLL0=%d,PRR0=%d,PLL1=%d,PRR1=%d\n", LL0, RR0, LL1, RR1);
            Shadow_Enc(LL0, RR0, LL1, RR1, LL0_16, RR0_16, LL1_16, RR1_16, mk);
            //printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
            //The following we add the limits on cipher

            ToBinary(L0_16, BL0_16, 8);
            ToBinary(R0_16, BR0_16, 8);
            ToBinary(L1_16, BL1_16, 8);
            ToBinary(R1_16, BR1_16, 8);

            ToBinary(LL0_16, BLL0_16, 8);
            ToBinary(RR0_16, BRR0_16, 8);
            ToBinary(LL1_16, BLL1_16, 8);
            ToBinary(RR1_16, BRR1_16, 8);
            dL0_16 = L0_16 ^ LL0_16;
            dL1_16 = L1_16 ^ LL1_16;
            dR0_16 = R0_16 ^ RR0_16;
            dR1_16 = R1_16 ^ RR1_16;

            ToBinary(dR0_16, BdR0_16, 8);
            ToBinary(dR1_16, BdR1_16, 8);
            ToBinary(dL0_16, BdL0_16, 8);
            ToBinary(dL1_16, BdL1_16, 8);

            mL0and_15 = (ROL(L1_16, 1) & (ROL(L1_16, 7)));
            mLL0and_15 = (ROL(LL1_16, 1) & (ROL(LL1_16, 7)));
            mL1and_15 = (ROL(L0_16, 1) & (ROL(L0_16, 7)));
            mLL1and_15 = (ROL(LL0_16, 1) & (ROL(LL0_16, 7)));
            dmL0and_15 = mL0and_15 ^ mLL0and_15;
            dmL1and_15 = mL1and_15 ^ mLL1and_15;
            dmR0_15 = dR0_16 ^ (ROL(dL1_16, 2)) ^ dmL0and_15;
            dmR1_15 = dR1_16 ^ (ROL(dL0_16, 2)) ^ dmL1and_15;

            /*
            printf("\n");
            printf("The %d-th pair of ciphertext!\n", n);
            printf("L0_16=");
            for (int m = 0; m < BlockSize; m++)
            {
                printf("%d", BL0_16[m]);
            }
            printf(";  R0_16=");
            for (int m = 0; m < BlockSize; m++)
            {
                printf("%d", BR0_16[m]);
            }
            printf(";  L1_16=");
            for (int m = 0; m < BlockSize; m++)
            {
                printf("%d", BL1_16[m]);
            }
            printf(";  R1_16=");
            for (int m = 0; m < BlockSize; m++)
            {
                printf("%d", BR1_16[m]);
            }
            printf("\n");
            printf("LL0_16=");
            for (int m = 0; m < BlockSize; m++)
            {
                printf("%d", BLL0_16[m]);
            }
            printf("; RR0_16=");
            for (int m = 0; m < BlockSize; m++)
            {
                printf("%d", BRR0_16[m]);
            }
            printf("; LL1_16=");
            for (int m = 0; m < BlockSize; m++)
            {
                printf("%d", BLL1_16[m]);
            }
            printf("; RR1_16=");
            for (int m = 0; m < BlockSize; m++)
            {
                printf("%d", BRR1_16[m]);
            }
            printf("\n");


            printf("mL0and=%d,mLL0and=%d,dmL0and=%d\n", mL0and_15, mLL0and_15, dmL0and_15);
            printf("mL1and=%d,mLL1and=%d,dmL1and=%d\n", mL1and_15, mLL1and_15, dmL1and_15);
            printf("dmR0=%d,dmR1=%d\n", dmR0_15, dmR1_15);
            */

            ToBinary(dmR0_15, BdmR0_15, 8);
            ToBinary(dmR1_15, BdmR1_15, 8);

            /*
            printf("\n");
            printf("The %d-th pair of ciphertext!\n", n);
            printf("L0=");
            for (int m = 0; m < BlockSize; m++)
            {
                printf("%d", BL1_16[m]^ BLL1_16[m]);
            }
            printf(";  R0=");
            for (int m = 0; m < BlockSize; m++)
            {
                printf("%d", BdmR0_15[m]);
            }
            printf(";  L1=");
            for (int m = 0; m < BlockSize; m++)
            {
                printf("%d", BL0_16[m]^ BLL0_16[m]);
            }
            printf(";  R1=");
            for (int m = 0; m < BlockSize; m++)
            {
                printf("%d", BdmR1_15[m]);
            }
            printf("\n");
            */
            //L0 is input LL0 is output
            //if ((BdL1_16[Pos[difftt][0]] == 0) && (BdL1_16[Pos[difftt][1]] == 0) && (BdL1_16[Pos[difftt][2]] == 0) && (BdmR0_15[Pos[difftt][3]] == 0) && (BdmR0_15[Pos[difftt][4]] == 0) && (BdmR0_15[Pos[difftt][5]] == 0) && (BdmR0_15[Pos[difftt][6]] == 0) && (BdmR0_15[Pos[difftt][7]] == 0) && (BdmR0_15[Pos[difftt][8]] == 0))
            if ((BdmR0_15[7] == 0) && (BdmR1_15[7] == 0))
            {
                //if ((BdL0_16[Pos[difftt][0]] == 0) && (BdL0_16[Pos[difftt][1]] == 0) && (BdL0_16[Pos[difftt][2]] == 0) && (BdmR1_15[Pos[difftt][3]] == 0) && (BdmR1_15[Pos[difftt][4]] == 0) && (BdmR1_15[Pos[difftt][5]] == 0) && (BdmR1_15[Pos[difftt][6]] == 0) && (BdmR1_15[Pos[difftt][7]] == 0) && (BdmR1_15[Pos[difftt][8]] == 0))
                if (1)
                {
                    //printf("The %d-th pair\n", count);
                    //printf("Leave above!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
                    //printf("\n");
                    cL0[count] = L0_16;
                    cLL0[count] = LL0_16;
                    cR0[count] = R0_16;
                    cRR0[count] = RR0_16;
                    cL1[count] = L1_16;
                    cLL1[count] = LL1_16;
                    cR1[count] = R1_16;
                    cRR1[count] = RR1_16;
                    count = count + 1;
                }
            }

        }



        //printf("\n");
        //printf("+++++++++++++++++++++++++++++++++++++++++++++++++\n");
        //



    }
    printf("N=%d,ssss=%d,count=%d\n", N, ssss, count);
}

unsigned char C0bit(int index, unsigned char L0, unsigned char R0, unsigned char L1, unsigned char R1)
{
    unsigned char temp;
    unsigned char BL0[8] = { 0 };
    unsigned char BR0[8] = { 0 };
    unsigned char BL1[8] = { 0 };
    unsigned char BR1[8] = { 0 };
    ToBinary(L0, BL0, 8);
    ToBinary(R0, BR0, 8);
    ToBinary(L1, BL1, 8);
    ToBinary(R1, BR1, 8);
    temp = BR1[index] ^ (BL0[(index + 1) % BlockSize] & BL0[(index + 7) % BlockSize]) ^ BL0[(index + 2) % BlockSize];
    return temp;
}

unsigned char C1bit(int index, unsigned char L0, unsigned char R0, unsigned char L1, unsigned char R1)
{
    unsigned char temp;
    unsigned char BL0[8] = { 0 };
    unsigned char BR0[8] = { 0 };
    unsigned char BL1[8] = { 0 };
    unsigned char BR1[8] = { 0 };
    ToBinary(L0, BL0, 8);
    ToBinary(R0, BR0, 8);
    ToBinary(L1, BL1, 8);
    ToBinary(R1, BR1, 8);
    temp = BR0[index] ^ (BL1[(index + 1) % BlockSize] & BL1[(index + 7) % BlockSize]) ^ BL1[(index + 2) % BlockSize];
    return temp;
}

//ki_15 is the round key in 15-th round,k3_15=Bk3_15[index] is one bit key
unsigned char mL0_14_bit(int index, unsigned char L0, unsigned char R0, unsigned char L1, unsigned char R1, unsigned char k3_15)
{
    //k3_15 is the third key in 15-th round
    unsigned char temp;
    unsigned char BL0[8] = { 0 };
    unsigned char BR0[8] = { 0 };
    unsigned char BL1[8] = { 0 };
    unsigned char BR1[8] = { 0 };
    ToBinary(L0, BL0, 8);
    ToBinary(R0, BR0, 8);
    ToBinary(L1, BL1, 8);
    ToBinary(R1, BR1, 8);

    temp = C0bit(index, L0, R0, L1, R1);
    temp = temp ^ k3_15;
    return temp;
}

unsigned char mL1_14_bit(int index, unsigned char L0, unsigned char R0, unsigned char L1, unsigned char R1, unsigned char k2_15)
{
    unsigned char temp;
    unsigned char BL0[8] = { 0 };
    unsigned char BR0[8] = { 0 };
    unsigned char BL1[8] = { 0 };
    unsigned char BR1[8] = { 0 };
    unsigned char Bk0_15[8] = { 0 };
    unsigned char Bk1_15[8] = { 0 };
    unsigned char Bk2_15[8] = { 0 };
    unsigned char Bk3_15[8] = { 0 };
    ToBinary(L0, BL0, 8);
    ToBinary(R0, BR0, 8);
    ToBinary(L1, BL1, 8);
    ToBinary(R1, BR1, 8);

    temp = C1bit(index, L0, R0, L1, R1);
    temp = temp ^ k2_15;
    return temp;
}

unsigned char mR0_14_bit(int index, unsigned char L0, unsigned char R0, unsigned char L1, unsigned char R1, unsigned char ksum, unsigned char k0, unsigned char k1, unsigned char k2, unsigned char k3)
{
    //ksum is the key not in non-linear,i.e, not useful in difference
    //k0,k1,k2,k3 is the key in non-linear
    //ksum=k0_15^k2_14^S2(k3_15)^S2(k2_15), k0=S1(k3_15),k1=S7(k3_15),k2=S1(k2_15),k3=S7(k2_15)
    unsigned char temp;
    int i;
    unsigned char BL0[8] = { 0 };
    unsigned char BR0[8] = { 0 };
    unsigned char BL1[8] = { 0 };
    unsigned char BR1[8] = { 0 };
    unsigned char C0[8] = { 0 };
    unsigned char C1[8] = { 0 };
    ToBinary(L0, BL0, 8);
    ToBinary(R0, BR0, 8);
    ToBinary(L1, BL1, 8);
    ToBinary(R1, BR1, 8);
    for (i = 0; i < BlockSize; i++)
    {
        C0[i] = C0bit(i, L0, R0, L1, R1);
        C1[i] = C1bit(i, L0, R0, L1, R1);
    }
    temp = ksum ^ ((k0 ^ C0[(index + 1) % BlockSize]) & (k1 ^ C0[(index + 7) % BlockSize])) ^ ((k2 ^ C1[(index + 1) % BlockSize]) & (k3 ^ C1[(index + 7) % BlockSize])) ^ BL1[index] ^ C0[(index + 2) % BlockSize] ^ C1[(index + 2) % BlockSize];
    return temp;
}

unsigned char mR1_14_bit(int index, unsigned char L0, unsigned char R0, unsigned char L1, unsigned char R1, unsigned char ksum, unsigned char k0, unsigned char k1, unsigned char k2, unsigned char k3)
{
    //ksum is the key not in non-linear,i.e, not useful in difference
    //k0,k1,k2,k3 is the key in non-linear
    //ksum=k1_15^k3_14^S2(k3_15)^S2(k2_15), k0=S1(k3_15),k1=S7(k3_15),k2=S1(k2_15),k3=S7(k2_15)
    unsigned char temp;
    int i;
    unsigned char BL0[8] = { 0 };
    unsigned char BR0[8] = { 0 };
    unsigned char BL1[8] = { 0 };
    unsigned char BR1[8] = { 0 };
    unsigned char C0[8] = { 0 };
    unsigned char C1[8] = { 0 };
    ToBinary(L0, BL0, 8);
    ToBinary(R0, BR0, 8);
    ToBinary(L1, BL1, 8);
    ToBinary(R1, BR1, 8);
    for (i = 0; i < BlockSize; i++)
    {
        C0[i] = C0bit(i, L0, R0, L1, R1);
        C1[i] = C1bit(i, L0, R0, L1, R1);
    }
    temp = ksum ^ ((k0 ^ C0[(index + 1) % BlockSize]) & (k1 ^ C0[(index + 7) % BlockSize])) ^ ((k2 ^ C1[(index + 1) % BlockSize]) & (k3 ^ C1[(index + 7) % BlockSize])) ^ BL0[index] ^ C0[(index + 2) % BlockSize] ^ C1[(index + 2) % BlockSize];
    return temp;
}

unsigned char mL0_13_bit(int index, unsigned char L0, unsigned char R0, unsigned char L1, unsigned char R1, unsigned char ksum, unsigned char k0, unsigned char k1, unsigned char k2, unsigned char k3)
{
    //ksum is the key not in non-linear,i.e, not useful in difference
    //k0,k1,k2,k3 is the key in non-linear
    //ksum=k1_15^k3_14^S2(k3_15)^S2(k2_15), k0=S1(k3_15),k1=S7(k3_15),k2=S1(k2_15),k3=S7(k2_15)
    unsigned char temp;
    temp = mR1_14_bit(index, L0, R0, L1, R1, ksum, k0, k1, k2, k3);

    return temp;
}
unsigned char mL1_13_bit(int index, unsigned char L0, unsigned char R0, unsigned char L1, unsigned char R1, unsigned char ksum, unsigned char k0, unsigned char k1, unsigned char k2, unsigned char k3)
{
    //ksum is the key not in non-linear,i.e, not useful in difference
    //k0,k1,k2,k3 is the key in non-linear
    //ksum=k1_15^k3_14^S2(k3_15)^S2(k2_15), k0=S1(k3_15),k1=S7(k3_15),k2=S1(k2_15),k3=S7(k2_15)
    unsigned char temp;
    temp = mR0_14_bit(index, L0, R0, L1, R1, ksum, k0, k1, k2, k3);

    return temp;
}
unsigned char mR0_13_bit(int index, unsigned char L0, unsigned char R0, unsigned char L1, unsigned char R1, unsigned char ksum, unsigned char k0sum, unsigned char k1sum, unsigned char k2sum, unsigned char k3sum, unsigned char k0, unsigned char k1, unsigned char k2, unsigned char k3, unsigned char k4, unsigned char k5, unsigned char k6, unsigned char k7, unsigned char k8, unsigned char k9)
{
    //ksum is the key not in non-linear,i.e, not useful in difference
    //k0sum,k1sum,...,k0,k1,k2,k3 is the key in non-linear
    //ksum=k0_14^k2_13^k3_15^S2(k1_15)^S2(k3_14)^S4(k3_15)^S4(k2_15)^S2(k0_15)^S2(k2_14)^S4(k3_15)^S4(k2_15)
    //k0sum=S1(k1_15)^S1(k3_14)^S3(k3_15)^S3(k2_15); k1sum=S7(k1_15)^S7(k3_14)^S1(k3_15)^S1(k2_15); k2sum=S1(k0_15)^S1(k2_14)^S3(k3_15)^S3(k2_15); k3sum=S7(k0_15)^S7(k2_14)^S1(k3_15)^S1(k2_15);
    //k0=S2(k3_15);k1=k3_15;k2=S2(k2_15);k3=k2_15;k4=S6(k3_15);k5=S6(k2_15);k6=S3(k3_15);k7=S1(k3_15);k8=S3(k2_15);k9=S1(k2_15)
    unsigned char temp, t0, t1, t2, t3, t4, t5;
    int i;
    unsigned char BL0[8] = { 0 };
    unsigned char BR0[8] = { 0 };
    unsigned char BL1[8] = { 0 };
    unsigned char BR1[8] = { 0 };
    unsigned char C0[8] = { 0 };
    unsigned char C1[8] = { 0 };
    ToBinary(L0, BL0, 8);
    ToBinary(R0, BR0, 8);
    ToBinary(L1, BL1, 8);
    ToBinary(R1, BR1, 8);
    for (i = 0; i < BlockSize; i++)
    {
        C0[i] = C0bit(i, L0, R0, L1, R1);
        C1[i] = C1bit(i, L0, R0, L1, R1);
    }

    t0 = k0sum ^ ((k0 ^ C0[(index + 2) % BlockSize]) & (k1 ^ C0[index])) ^ ((k2 ^ C1[(index + 2) % BlockSize]) & (k3 ^ C1[index])) ^ BL0[(index + 1) % BlockSize] ^ C0[(index + 3) % BlockSize] ^ C1[(index + 3) % BlockSize];
    t1 = k1sum ^ ((k1 ^ C0[index]) & (k4 ^ C0[(index + 6) % BlockSize])) ^ ((k3 ^ C1[index]) & (k5 ^ C1[(index + 6) % BlockSize])) ^ BL0[(index + 7) % BlockSize] ^ C0[(index + 1) % BlockSize] ^ C1[(index + 1) % BlockSize];
    t2 = ((k6 ^ C0[(index + 3) % BlockSize]) & (k7 ^ C0[(index + 1) % BlockSize])) ^ ((k8 ^ C1[(index + 3) % BlockSize]) & (k9 ^ C1[(index + 1) % BlockSize])) ^ BL0[(index + 2) % BlockSize] ^ C0[(index + 4) % BlockSize] ^ C1[(index + 4) % BlockSize];

    t3 = k2sum ^ ((k0 ^ C0[(index + 2) % BlockSize]) & (k1 ^ C0[index])) ^ ((k2 ^ C1[(index + 2) % BlockSize]) & (k3 ^ C1[index])) ^ BL1[(index + 1) % BlockSize] ^ C0[(index + 3) % BlockSize] ^ C1[(index + 3) % BlockSize];
    t4 = k3sum ^ ((k1 ^ C0[index]) & (k4 ^ C0[(index + 6) % BlockSize])) ^ ((k3 ^ C1[index]) & (k5 ^ C1[(index + 6) % BlockSize])) ^ BL1[(index + 7) % BlockSize] ^ C0[(index + 1) % BlockSize] ^ C1[(index + 1) % BlockSize];
    t5 = ((k6 ^ C0[(index + 3) % BlockSize]) & (k7 ^ C0[(index + 1) % BlockSize])) ^ ((k8 ^ C1[(index + 3) % BlockSize]) & (k9 ^ C1[(index + 1) % BlockSize])) ^ BL1[(index + 2) % BlockSize] ^ C0[(index + 4) % BlockSize] ^ C1[(index + 4) % BlockSize];

    temp = ksum ^ C0[index] ^ (t0 & t1) ^ t2 ^ (t3 & t4) ^ t5;


    return temp;
}
unsigned char mR0_12_bit(int index, unsigned char L0, unsigned char R0, unsigned char L1, unsigned char R1, unsigned char kw, unsigned char kc_1, unsigned char kc_7, unsigned char kc1_0, unsigned char kc1_2, unsigned char kc1_6, unsigned char kc2_0, unsigned char kc2_2, unsigned char kc2_6, unsigned char kc3_1, unsigned char kc3_7, unsigned char k2_15_1, unsigned char k2_15_3, unsigned char k2_15_5, unsigned char k2_15_7, unsigned char k3_15_1, unsigned char k3_15_3, unsigned char k3_15_5, unsigned char k3_15_7)
{

    //Compute the value of mR0_12[index]
    unsigned char temp, t0, t1, t2, t10, t11, t12, t13, t14, t15, t16, t20, t21, t22, t23, t24, t25, t26, t3;
    int i;
    unsigned char BL0[8] = { 0 };
    unsigned char BR0[8] = { 0 };
    unsigned char BL1[8] = { 0 };
    unsigned char BR1[8] = { 0 };
    unsigned char C0[8] = { 0 };
    unsigned char C1[8] = { 0 };
    unsigned char CC[8] = { 0 };
    unsigned char CL[8] = { 0 };
    unsigned char CL0[8] = { 0 };
    unsigned char CL1[8] = { 0 };
    ToBinary(L0, BL0, 8);
    ToBinary(R0, BR0, 8);
    ToBinary(L1, BL1, 8);
    ToBinary(R1, BR1, 8);
    //Compute the C0 and C1
    for (i = 0; i < BlockSize; i++)
    {
        C0[i] = C0bit(i, L0, R0, L1, R1);
        C1[i] = C1bit(i, L0, R0, L1, R1);
    }
    //Conpute the CC=C0^C1
    for (i = 0; i < BlockSize; i++)
    {
        CC[i] = C0[i] ^ C1[i];
    }
    //Compute CL=L0^L1
    for (i = 0; i < BlockSize; i++)
    {
        CL[i] = BL0[i] ^ BL1[i];
    }
    //Compute the CL0=L0^C0<<<2^C1<<<2
    for (i = 0; i < BlockSize; i++)
    {
        CL0[i] = BL0[i] ^ C0[(i + 2) % BlockSize] ^ C1[(i + 2) % BlockSize];
    }
    //Compute the CL1=C0^CL<<<2
    for (i = 0; i < BlockSize; i++)
    {
        CL1[i] = C0[i] ^ CL[(i + 2) % BlockSize];
    }

    //The part of t0
    t0 = CC[(index + 2) % BlockSize] ^ kw ^ CL0[index] ^ ((k3_15_1 ^ C0[(index + 1) % BlockSize]) & (k3_15_7 ^ C0[(index + 7) % BlockSize])) ^ ((k2_15_1 ^ C1[(index + 1) % BlockSize]) & (k2_15_7 ^ C1[(index + 7) % BlockSize]));
    //The part of t1
    t10 = kc3_1 ^ CL1[(index + 1) % BlockSize];
    t11 = kc2_2 ^ CL0[(index + 2) % BlockSize] ^ ((k3_15_3 ^ C0[(index + 3) % BlockSize]) & (k3_15_1 ^ C0[(index + 1) % BlockSize])) ^ ((k2_15_3 ^ C1[(index + 3) % BlockSize]) & (k2_15_1 ^ C1[(index + 1) % BlockSize]));
    t12 = CL[index] ^ kc1_0;
    t13 = kc2_0 ^ CL0[index] ^ ((k3_15_1 ^ C0[(index + 1) % BlockSize]) & (k3_15_7 ^ C0[(index + 7) % BlockSize])) ^ ((k2_15_1 ^ C1[(index + 1) % BlockSize]) & (k2_15_7 ^ C1[(index + 7) % BlockSize]));
    t14 = CL[(index + 2) % BlockSize] ^ kc1_2;
    t15 = (CL[(index + 2) % BlockSize] ^ kc1_2) & (CL[index] ^ kc1_0);
    t16 = CC[(index + 7) % BlockSize] ^ kc_7;
    t1 = (t10 ^ (t11 & t12) ^ (t13 & t14) ^ t15) & t16;

    //The part of t2
    t20 = kc3_7 ^ CL1[(index + 7) % BlockSize];
    t21 = kc2_0 ^ CL0[(index + 0) % BlockSize] ^ ((k3_15_1 ^ C0[(index + 1) % BlockSize]) & (k3_15_7 ^ C0[(index + 7) % BlockSize])) ^ ((k2_15_1 ^ C1[(index + 1) % BlockSize]) & (k2_15_7 ^ C1[(index + 7) % BlockSize]));
    t22 = CL[(index + 6) % BlockSize] ^ kc1_6;
    t23 = kc2_6 ^ CL0[(index + 6) % BlockSize] ^ ((k3_15_7 ^ C0[(index + 7) % BlockSize]) & (k3_15_5 ^ C0[(index + 5) % BlockSize])) ^ ((k2_15_7 ^ C1[(index + 7) % BlockSize]) & (k2_15_5 ^ C1[(index + 5) % BlockSize]));
    t24 = CL[(index + 0) % BlockSize] ^ kc1_0;
    t25 = (CL[(index + 6) % BlockSize] ^ kc1_6) & (CL[index] ^ kc1_0);
    t26 = CC[(index + 1) % BlockSize] ^ kc_1;
    t2 = (t20 ^ (t21 & t22) ^ (t23 & t24) ^ t25) & t26;

    t3 = ((CC[(index + 1) % BlockSize] ^ kc_1) & (CC[(index + 7) % BlockSize] ^ kc_7));

    temp = t0 ^ t1 ^ t2 ^ t3;


    return temp;
}

unsigned char mR1_12_bit(int index, unsigned char L0, unsigned char R0, unsigned char L1, unsigned char R1, unsigned char kw, unsigned char kc_1, unsigned char kc_7, unsigned char kc1_0, unsigned char kc1_2, unsigned char kc1_6, unsigned char kc2_0, unsigned char kc2_2, unsigned char kc2_6, unsigned char kc3_1, unsigned char kc3_7, unsigned char k2_15_1, unsigned char k2_15_3, unsigned char k2_15_5, unsigned char k2_15_7, unsigned char k3_15_1, unsigned char k3_15_3, unsigned char k3_15_5, unsigned char k3_15_7)
{

    //Compute the value of mR0_12[index]
    unsigned char temp, t0, t1, t2, t10, t11, t12, t13, t14, t15, t16, t20, t21, t22, t23, t24, t25, t26, t3;
    int i;
    unsigned char BL0[8] = { 0 };
    unsigned char BR0[8] = { 0 };
    unsigned char BL1[8] = { 0 };
    unsigned char BR1[8] = { 0 };
    unsigned char C0[8] = { 0 };
    unsigned char C1[8] = { 0 };
    unsigned char CC[8] = { 0 };
    unsigned char CL[8] = { 0 };
    unsigned char CL0[8] = { 0 };
    unsigned char CL1[8] = { 0 };
    ToBinary(L0, BL0, 8);
    ToBinary(R0, BR0, 8);
    ToBinary(L1, BL1, 8);
    ToBinary(R1, BR1, 8);
    //Compute the C0 and C1
    for (i = 0; i < BlockSize; i++)
    {
        C0[i] = C0bit(i, L0, R0, L1, R1);
        C1[i] = C1bit(i, L0, R0, L1, R1);
    }
    //Conpute the CC=C0^C1
    for (i = 0; i < BlockSize; i++)
    {
        CC[i] = C0[i] ^ C1[i];
    }
    //Compute CL=L0^L1
    for (i = 0; i < BlockSize; i++)
    {
        CL[i] = BL0[i] ^ BL1[i];
    }
    //Compute the CL0=L1^C0<<<2^C1<<<2
    for (i = 0; i < BlockSize; i++)
    {
        CL0[i] = BL1[i] ^ C0[(i + 2) % BlockSize] ^ C1[(i + 2) % BlockSize];
    }
    //Compute the CL1=C0^CL<<<2
    for (i = 0; i < BlockSize; i++)
    {
        CL1[i] = C0[i] ^ CL[(i + 2) % BlockSize];
    }

    //The part of t0
    t0 = CC[(index + 2) % BlockSize] ^ kw ^ CL0[index] ^ ((k3_15_1 ^ C0[(index + 1) % BlockSize]) & (k3_15_7 ^ C0[(index + 7) % BlockSize])) ^ ((k2_15_1 ^ C1[(index + 1) % BlockSize]) & (k2_15_7 ^ C1[(index + 7) % BlockSize]));
    //The part of t1
    t10 = kc3_1 ^ CL1[(index + 1) % BlockSize];
    t11 = kc2_2 ^ CL0[(index + 2) % BlockSize] ^ ((k3_15_3 ^ C0[(index + 3) % BlockSize]) & (k3_15_1 ^ C0[(index + 1) % BlockSize])) ^ ((k2_15_3 ^ C1[(index + 3) % BlockSize]) & (k2_15_1 ^ C1[(index + 1) % BlockSize]));
    t12 = CL[index] ^ kc1_0;
    t13 = kc2_0 ^ CL0[index] ^ ((k3_15_1 ^ C0[(index + 1) % BlockSize]) & (k3_15_7 ^ C0[(index + 7) % BlockSize])) ^ ((k2_15_1 ^ C1[(index + 1) % BlockSize]) & (k2_15_7 ^ C1[(index + 7) % BlockSize]));
    t14 = CL[(index + 2) % BlockSize] ^ kc1_2;
    t15 = (CL[(index + 2) % BlockSize] ^ kc1_2) & (CL[index] ^ kc1_0);
    t16 = CC[(index + 7) % BlockSize] ^ kc_7;
    t1 = (t10 ^ (t11 & t12) ^ (t13 & t14) ^ t15) & t16;

    //The part of t2
    t20 = kc3_7 ^ CL1[(index + 7) % BlockSize];
    t21 = kc2_0 ^ CL0[(index + 0) % BlockSize] ^ ((k3_15_1 ^ C0[(index + 1) % BlockSize]) & (k3_15_7 ^ C0[(index + 7) % BlockSize])) ^ ((k2_15_1 ^ C1[(index + 1) % BlockSize]) & (k2_15_7 ^ C1[(index + 7) % BlockSize]));
    t22 = CL[(index + 6) % BlockSize] ^ kc1_6;
    t23 = kc2_6 ^ CL0[(index + 6) % BlockSize] ^ ((k3_15_7 ^ C0[(index + 7) % BlockSize]) & (k3_15_5 ^ C0[(index + 5) % BlockSize])) ^ ((k2_15_7 ^ C1[(index + 7) % BlockSize]) & (k2_15_5 ^ C1[(index + 5) % BlockSize]));
    t24 = CL[(index + 0) % BlockSize] ^ kc1_0;
    t25 = (CL[(index + 6) % BlockSize] ^ kc1_6) & (CL[index] ^ kc1_0);
    t26 = CC[(index + 1) % BlockSize] ^ kc_1;
    t2 = (t20 ^ (t21 & t22) ^ (t23 & t24) ^ t25) & t26;

    t3 = ((CC[(index + 1) % BlockSize] ^ kc_1) & (CC[(index + 7) % BlockSize] ^ kc_7));

    temp = t0 ^ t1 ^ t2 ^ t3;


    return temp;
}


void Diff_mR0_12_bit_16(int& count, int N, unsigned long long mk, unsigned char dL0in, unsigned char dR0in, unsigned char dL1in, unsigned char dR1in, int* cL0, int* cLL0, int* cR0, int* cRR0, int* cL1, int* cLL1, int* cR1, int* cRR1)
{
    //From the 14 round distinguisher, we have known that k2_15=k3_15
    //Guess 16 bit key and judge i,i+2 position difference
    CollectPhase(count, N, mk, dL0in, dR0in, dL1in, dR1in, cL0, cLL0, cR0, cRR0, cL1, cLL1, cR1, cRR1);
    int* CountKey = new int[65536];
    int NK = 65536;
    int pair;
    //subkey used in i-th bit of mR0_12
    //kc=k2_15^k3_15^k0_14^k1_14^k2_13^k3_13
    //ksum=k0_14^k2_13^k3_15^S2(k1_15)^S2(k3_14)^S4(k3_15)^S4(k2_15)^S2(k0_15)^S2(k2_14)^S4(k3_15)^S4(k2_15)
    unsigned char dmr0_12, kw;
    //unsigned char kc,ksum, k0sum, k1sum, k2sum, k3sum, k0, k1, k2, k3, k4, k5, k6, k7, k8, k9;
    unsigned char L0_16, R0_16, L1_16, R1_16, LL0_16, RR0_16, LL1_16, RR1_16, mr0_12, mrr0_12;
    //unsigned char Bkc[8] = { 0 };
    unsigned char Bksum[8] = { 0 };
    unsigned char Bmr0_12[8] = { 0 };
    unsigned char Bmrr0_12[8] = { 0 };
    unsigned char Bmr1_12[8] = { 0 };
    unsigned char Bmrr1_12[8] = { 0 };
    unsigned char Bmr0_13[8] = { 0 };
    unsigned char Bmrr0_13[8] = { 0 };
    unsigned char dBmr0_12[8] = { 0 };//the difference in mR0_13
    unsigned char dBmr1_12[8] = { 0 };//the difference in mR0_13
    unsigned char dBmr0_13[8] = { 0 };//the difference in mR0_13
    unsigned char Bk2_15[8] = { 0 };
    unsigned char Bk3_15[8] = { 0 };
    unsigned char Bk1_15_k3_14[8] = { 0 };
    unsigned char Bk0_15_k2_14[8] = { 0 };
    unsigned char Bkc_1[8] = { 0 };
    unsigned char Bkc_7[8] = { 0 };
    unsigned char Bkc1_0[8] = { 0 };
    unsigned char Bkc1_2[8] = { 0 };
    unsigned char Bkc1_6[8] = { 0 };
    unsigned char Bkc2_0[8] = { 0 };
    unsigned char Bkc2_2[8] = { 0 };
    unsigned char Bkc2_6[8] = { 0 };
    unsigned char Bkc3_1[8] = { 0 };
    unsigned char Bkc3_7[8] = { 0 };
    unsigned char Bk2_15_1[8] = { 0 };
    unsigned char Bk2_15_3[8] = { 0 };
    unsigned char Bk2_15_5[8] = { 0 };
    unsigned char Bk2_15_7[8] = { 0 };
    unsigned char Bk3_15_1[8] = { 0 };
    unsigned char Bk3_15_3[8] = { 0 };
    unsigned char Bk3_15_5[8] = { 0 };
    unsigned char Bk3_15_7[8] = { 0 };
    unsigned char Bkc[8] = { 0 };
    unsigned char Bkc1[8] = { 0 };
    unsigned char Bkc2[8] = { 0 };
    unsigned char Bkc3[8] = { 0 };
    unsigned char Bmr0_13_1[8] = { 0 };
    unsigned char Bmr0_13_7[8] = { 0 };
    unsigned char dBmr0_13_1[8] = { 0 };
    unsigned char dBmr0_13_7[8] = { 0 };
    unsigned char con[8] = { 0,0,0,0,0,0,0,0 };
    unsigned char dmr013[8] = { 0,0,0,0,0,0,0,1 };


    //unsigned char Bk1_15_k3_14[8] = { 0 };
    //unsigned char Bk0_15_k2_14[8] = { 0 };
    unsigned char Bk0sum[8] = { 0 };
    unsigned char Bk1sum[8] = { 0 };
    unsigned char Bk2sum[8] = { 0 };
    unsigned char Bk3sum[8] = { 0 };
    unsigned char Bk0[8] = { 0 };
    unsigned char Bk1[8] = { 0 };
    unsigned char Bk2[8] = { 0 };
    unsigned char Bk3[8] = { 0 };
    unsigned char Bk4[8] = { 0 };
    unsigned char Bk5[8] = { 0 };
    unsigned char Bk6[8] = { 0 };
    unsigned char Bk7[8] = { 0 };
    unsigned char Bk8[8] = { 0 };
    unsigned char Bk9[8] = { 0 };


    int guesskey, j;
    int max, keycount;
    max = 0;
    keycount = 0;
    kw = 0;
    unsigned char Bguesskey[16] = { 0 };
    //ksum = 0;
    int Diffpos = 2;
    //Initialize the array CountKey
    for (int x = 0; x < 65536; x++)
    {
        CountKey[x] = 0;
    }
    for (int x = 0; x < BlockSize; x++)
    {
        Bkc1[x] = con[x];
    }
 
    for (guesskey = 0; guesskey < NK; guesskey++)
    {
        ToBinaryU(guesskey, Bguesskey, 16);

        //Bk2_i and Bk3_i, consider the result from the 14-round distinguishers
        Bk2_15[(Diffpos + 1) % BlockSize] = Bguesskey[0];
        Bk3_15[(Diffpos + 1) % BlockSize] = Bguesskey[0];
        Bk2_15[(Diffpos + 3) % BlockSize] = Bguesskey[1];
        Bk3_15[(Diffpos + 3) % BlockSize] = Bguesskey[1];
        Bk2_15[(Diffpos + 5) % BlockSize] = Bguesskey[2];
        Bk3_15[(Diffpos + 5) % BlockSize] = Bguesskey[2];
        Bk2_15[(Diffpos + 7) % BlockSize] = Bguesskey[3];
        Bk3_15[(Diffpos + 7) % BlockSize] = Bguesskey[3];

        Bkc[(Diffpos + 1) % BlockSize] = Bguesskey[4];
        Bkc[(Diffpos + 3) % BlockSize] = Bguesskey[5];
        Bkc[(Diffpos + 5) % BlockSize] = Bguesskey[6];
        Bkc[(Diffpos + 7) % BlockSize] = Bguesskey[7];

        Bkc2[(Diffpos) % BlockSize] = Bguesskey[8];
        Bkc2[(Diffpos + 2) % BlockSize] = Bguesskey[9];
        Bkc2[(Diffpos + 4) % BlockSize] = Bguesskey[10];
        Bkc2[(Diffpos + 6) % BlockSize] = Bguesskey[11];

        Bkc3[(Diffpos + 1) % BlockSize] = Bguesskey[12];
        Bkc3[(Diffpos + 3) % BlockSize] = Bguesskey[13];
        Bkc3[(Diffpos + 5) % BlockSize] = Bguesskey[14];
        Bkc3[(Diffpos + 7) % BlockSize] = Bguesskey[15];

        //The value of Bkc1, consider the result from the 12-round distinguisher 
        Bkc1[0] = 0; Bkc1[1] = 0; Bkc1[2] = 0; Bkc1[3] = 0; Bkc1[4] = 0; Bkc1[5] = 0; Bkc1[6] = 0; Bkc1[7] = 0;
        //Compute k1sum-k9
        for (j = 0; j < 8; j++)
        {
            Bkc_1[j] = Bkc[(j + 1) % BlockSize];
            Bkc_7[j] = Bkc[(j + 7) % BlockSize];
            Bkc1_0[j] = Bkc1[j];
            Bkc1_2[j] = Bkc1[(j + 2) % BlockSize];
            Bkc1_6[j] = Bkc1[(j + 6) % BlockSize];
            Bkc2_0[j] = Bkc2[j];
            Bkc2_2[j] = Bkc2[(j + 2) % BlockSize];
            Bkc2_6[j] = Bkc2[(j + 6) % BlockSize];
            Bkc3_1[j] = Bkc3[(j + 1) % BlockSize];
            Bkc3_7[j] = Bkc3[(j + 7) % BlockSize];
            Bk2_15_1[j] = Bk2_15[(j + 1) % BlockSize];
            Bk2_15_3[j] = Bk2_15[(j + 3) % BlockSize];
            Bk2_15_5[j] = Bk2_15[(j + 5) % BlockSize];
            Bk2_15_7[j] = Bk2_15[(j + 7) % BlockSize];

            Bk3_15_1[j] = Bk3_15[(j + 1) % BlockSize];
            Bk3_15_3[j] = Bk3_15[(j + 3) % BlockSize];
            Bk3_15_5[j] = Bk3_15[(j + 5) % BlockSize];
            Bk3_15_7[j] = Bk3_15[(j + 7) % BlockSize];

        }
        //Test all count pair cipher
        for (pair = 0; pair < count; pair++)
        {
            L0_16 = cL0[pair];
            LL0_16 = cLL0[pair];
            R0_16 = cR0[pair];
            RR0_16 = cRR0[pair];
            L1_16 = cL1[pair];
            LL1_16 = cLL1[pair];
            R1_16 = cR1[pair];
            RR1_16 = cRR1[pair];

            for (int k = 0; k < 8; k++)
            {

                Bmr0_12[k] = mR0_12_bit(k, L0_16, R0_16, L1_16, R1_16, kw, Bkc_1[k], Bkc_7[k], Bkc1_0[k], Bkc1_2[k], Bkc1_6[k], Bkc2_0[k], Bkc2_2[k], Bkc2_6[k], Bkc3_1[k], Bkc3_7[k], Bk2_15_1[k], Bk2_15_3[k], Bk2_15_5[k], Bk2_15_7[k], Bk3_15_1[k], Bk3_15_3[k], Bk3_15_5[k], Bk3_15_7[k]);
                Bmrr0_12[k] = mR0_12_bit(k, LL0_16, RR0_16, LL1_16, RR1_16, kw, Bkc_1[k], Bkc_7[k], Bkc1_0[k], Bkc1_2[k], Bkc1_6[k], Bkc2_0[k], Bkc2_2[k], Bkc2_6[k], Bkc3_1[k], Bkc3_7[k], Bk2_15_1[k], Bk2_15_3[k], Bk2_15_5[k], Bk2_15_7[k], Bk3_15_1[k], Bk3_15_3[k], Bk3_15_5[k], Bk3_15_7[k]);
                dBmr0_12[k] = Bmr0_12[k] ^ Bmrr0_12[k];

                Bmr1_12[k] = mR1_12_bit(k, L0_16, R0_16, L1_16, R1_16, kw, Bkc_1[k], Bkc_7[k], Bkc1_0[k], Bkc1_2[k], Bkc1_6[k], Bkc2_0[k], Bkc2_2[k], Bkc2_6[k], Bkc3_1[k], Bkc3_7[k], Bk2_15_1[k], Bk2_15_3[k], Bk2_15_5[k], Bk2_15_7[k], Bk3_15_1[k], Bk3_15_3[k], Bk3_15_5[k], Bk3_15_7[k]);
                Bmrr1_12[k] = mR1_12_bit(k, LL0_16, RR0_16, LL1_16, RR1_16, kw, Bkc_1[k], Bkc_7[k], Bkc1_0[k], Bkc1_2[k], Bkc1_6[k], Bkc2_0[k], Bkc2_2[k], Bkc2_6[k], Bkc3_1[k], Bkc3_7[k], Bk2_15_1[k], Bk2_15_3[k], Bk2_15_5[k], Bk2_15_7[k], Bk3_15_1[k], Bk3_15_3[k], Bk3_15_5[k], Bk3_15_7[k]);
                dBmr1_12[k] = Bmr1_12[k] ^ Bmrr1_12[k];
            }

            /*
            printf("The %d-th pair\n", pair);
            printf("Bmr0_12=");
            for (int k = 0; k < 8; k++)
            {
                printf("%d", Bmr0_12[k]);
            }
            printf("\n");
            printf("Bmrr0_12=");
            for (int k = 0; k < 8; k++)
            {
                printf("%d", Bmrr0_12[k]);
            }
            printf("\n");
            */
            if ((dBmr0_12[(Diffpos + 2) % BlockSize] == Bdiff[(Diffpos + 2) % BlockSize]) && (dBmr1_12[(Diffpos + 2) % BlockSize] == Bdiff[(Diffpos + 2) % BlockSize]) && (dBmr1_12[(Diffpos + 4) % BlockSize] == Bdiff[(Diffpos + 4) % BlockSize]) && (dBmr0_12[(Diffpos + 4) % BlockSize] == Bdiff[(Diffpos + 4) % BlockSize]))
            {
                if ((dBmr0_12[(Diffpos) % BlockSize] == Bdiff[(Diffpos) % BlockSize]) && (dBmr1_12[(Diffpos) % BlockSize] == Bdiff[(Diffpos) % BlockSize]) && (dBmr1_12[(Diffpos + 6) % BlockSize] == Bdiff[(Diffpos + 6) % BlockSize]) && (dBmr0_12[(Diffpos + 6) % BlockSize] == Bdiff[(Diffpos + 6) % BlockSize]))
                {
                    /*
                    printf("======================================\m");
                    printf("The %d-th pair\n", pair);
                    for (int z = 0; z < BlockSize; z++)
                    {
                        printf("%d", Bmr0_12[z]);
                    }
                    printf("\n");
                    for (int z = 0; z < BlockSize; z++)
                    {
                        printf("%d", Bmrr0_12[z]);
                    }
                    printf("\n");
                    for (int z = 0; z < BlockSize; z++)
                    {
                        printf("%d", dBmr0_12[z]);
                    }
                    printf("\n");
                    */
                    CountKey[guesskey] = CountKey[guesskey] + 1;
                }
            }





        }

    }

    for (int y = 0; y < NK; y++)
    {
        if (CountKey[y] > max)
        {
            max = CountKey[y];
        }
    }

    ofstream fw;
    fw.open("Result.txt");
    for (int y = 0; y < NK; y++)
    {
        if (CountKey[y] == max)
        {
            keycount = keycount + 1;
            fw << y << endl;
            //printf("%d\n", y);
        }
    }
    fw.close();

    printf("max=%d\n", max);
    printf("the 8-th=%d\n", CountKey[8]);
    printf("Ketcount=%d\n", keycount);
    printf("Ketcount rate=%f\n", 1.0 * keycount / NK);
    printf("#################################################################################\n");




}



int main()
{
    int N, count;
    unsigned long long mk;
    unsigned char dL0in, dR0in, dL1in, dR1in;
    unsigned char L0, L1, R0, R1, k0, k1, k2, k3;
    int* cL0 = new int[8192];
    int* cLL0 = new int[8192];
    int* cL1 = new int[8192];
    int* cLL1 = new int[8192];
    int* cR0 = new int[8192];
    int* cRR0 = new int[8192];
    int* cR1 = new int[8192];
    int* cRR1 = new int[8192];
    dL0in = 0;
    dR0in = diff;
    dL1in = 0;
    dR1in = diff;
    //This difference can get 2 bits key information.
    mk = 0x11;
    count = 0;
    N = 5000;
    Diff_mR0_12_bit_16(count, N, mk, dL0in, dR0in, dL1in, dR1in, cL0, cLL0, cR0, cRR0, cL1, cLL1, cR1, cRR1);
    //CollectPhase(count, N, mk, dL0in, dR0in, dL1in, dR1in, cL0, cLL0, cR0, cRR0, cL1, cLL1, cR1, cRR1);
    //GuessKey_15_2(count, N, mk, dL0in, dR0in, dL1in, dR1in, cL0, cLL0, cR0, cRR0, cL1, cLL1, cR1, cRR1);




    return 0;
}