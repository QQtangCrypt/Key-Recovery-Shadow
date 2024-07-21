/*
* This is the key recovery for Shadow32
* Data:2023 12 26
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
/*
#define diff 1
#define diffpos1 0
#define diffpos2 6
#define keypos1 1
#define keypos2 5
#define keypos3 7
*/
#define diff 4
#define difftt 8
#define diffpos1 1
#define diffpos2 7
#define keypos1 0
#define keypos2 2
#define keypos3 6
//////////////////////////////////////////////// random number generator
uint32_t p = 0x7fffffff;
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> dis(0, p - 1);
////////////////////////////////////////////////
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
static const unsigned char Pos[8][6] = { 1,2,3,4,5,7, 0,1,2,3,4,6, 0,1,2,3,5,7, 0,1,2,4,6,7, 0,1,3,5,6,7, 0,2,4,5,6,7, 1,3,4,5,6,7, 0,2,3,4,5,6 };


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
       // printf("Round=%d,k0=%d,k1=%d,k2=%d,k3=%d\n", i, k0[i], k1[i], k2[i], k3[i]);
    //}

    for (r = 0; r < ROUND; r++)
    {

        Shadow_Enc_One_Round(L0in, R0in, L1in, R1in, k2[ROUND-1-r], k3[ROUND - 1 - r], k0[ROUND - 1 - r], k1[ROUND - 1 - r]);

        //Shadow_Enc_One_Round(L0in, R0in, L1in, R1in, k0[r], k1[r], k2[r], k3[r]);
        //printf("Round=%d,k0=%d,k1=%d,k2=%d,k3=%d,L0=%d,R0=%d,L1=%d,R1=%d\n", r, k0[r], k1[r], k2[r], k3[r], L0in, R0in, L1in, R1in);
    }
    L0out = L0in;
    R0out = R0in;
    L1out = L1in;
    R1out = R1in;

}

//Return the number of element in set cL0--cRR1
//N is the number of chosen plain text pairs
//mk is the master key
//d*in is the input difference
//16=12+4 
//The form of the output difference is (********,00****0*,********,00****0*)
void CollectPhaseO(int& count, int N, unsigned long long mk, unsigned char dL0in, unsigned char dR0in, unsigned char dL1in, unsigned char dR1in, int* cL0, int* cLL0, int* cR0, int* cRR0, int* cL1, int* cLL1, int* cR1, int* cRR1)
{


    int n;
    //dL0--dR1 is the output difference after 16 round
    unsigned char L0, LL0, R0, RR0, L1, LL1, R1, RR1, L0_16, LL0_16, R0_16, RR0_16, L1_16, LL1_16, R1_16, RR1_16, mL0and_15, mLL0and_15, mL1and_15, mLL1and_15, dL0_16, dL1_16, dR0_16, dR1_16, dmL0and_15, dmL1and_15, dmR0_15, dmR1_15;
    unsigned char BdL0_16[8] = { 0 };
    unsigned char BdL1_16[8] = { 0 };
    unsigned char BdR0_16[8] = { 0 };
    unsigned char BdR1_16[8] = { 0 };
    unsigned char BdmR0_15[8] = { 0 };
    unsigned char BdmR1_15[8] = { 0 };
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
        LL0 = L0 ^ dL0in;
        RR0 = R0 ^ dR0in;
        LL1 = L1 ^ dL1in;
        RR1 = R1 ^ dR1in;
        //printf("+++++++++++++++++++++++++++++++++++++++++++++++++\n");
        //printf("The encryption for L,R\n");
        //printf("PL0=%d,PR0=%d,PL1=%d,PR1=%d\n", L0, R0, L1, R1);
        Shadow_Enc(L0, R0, L1, R1, L0_16, R0_16, L1_16, R1_16, mk);
        //printf("+++++++++++++++++++++++++++++++++++++++++++++++++\n");
        //printf("The encryption for LL,RR\n");
        //printf("PLL0=%d,PRR0=%d,PLL1=%d,PRR1=%d\n", LL0, RR0, LL1, RR1);
        Shadow_Enc(LL0, RR0, LL1, RR1, LL0_16, RR0_16, LL1_16, RR1_16, mk);
        //The following we add the limits on cipher

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

        ToBinary(dmR0_15, BdmR0_15, 8);
        ToBinary(dmR1_15, BdmR1_15, 8);

        //L0 is input LL0 is output
        if ((dmR0_15 == diff) && (dmR1_15 == diff) && (BdL0_16[Pos[difftt - 1][0]] == 0) && (BdL0_16[Pos[difftt - 1][1]] == 0) && (BdL0_16[Pos[difftt - 1][2]] == 0) && (BdL0_16[Pos[difftt - 1][3]] == 0) && (BdL0_16[Pos[difftt - 1][4]] == 0) && (BdL0_16[Pos[difftt - 1][5]] == 0) && (BdL1_16[Pos[difftt - 1][0]] == 0) && (BdL1_16[Pos[difftt - 1][1]] == 0) && (BdL1_16[Pos[difftt - 1][2]] == 0) && (BdL1_16[Pos[difftt - 1][3]] == 0) && (BdL1_16[Pos[difftt - 1][4]] == 0) && (BdL1_16[Pos[difftt - 1][5]] == 0))
        {
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
void CollectPhase(int& count, int N, unsigned long long mk, unsigned char dL0in, unsigned char dR0in, unsigned char dL1in, unsigned char dR1in, int* cL0, int* cLL0, int* cR0, int* cRR0, int* cL1, int* cLL1, int* cR1, int* cRR1, unsigned char* Diff_L0, unsigned char* Diff_R0, unsigned char* Diff_L1, unsigned char* Diff_R1, unsigned char* Diff_L0_pos, unsigned char* Diff_R0_pos, unsigned char* Diff_L1_pos, unsigned char* Diff_R1_pos, int Condition_L0, int Condition_R0, int Condition_L1, int Condition_R1)
{
    int n, temp;
    int Flag_L0, Flag_L1, Flag_R0, Flag_R1;
    int l0, l1, r0, r1;
    //dL0--dR1 is the output difference after 16 round
    unsigned char L0, LL0, R0, RR0, L1, LL1, R1, RR1, L0_16, LL0_16, R0_16, RR0_16, L1_16, LL1_16, R1_16, RR1_16, mL0and_15, mLL0and_15, mL1and_15, mLL1and_15, dL0_16, dL1_16, dR0_16, dR1_16, dmL0and_15, dmL1and_15, dmR0_15, dmR1_15;
    unsigned char BdL0_16[8] = { 0 };
    unsigned char BdL1_16[8] = { 0 };
    unsigned char BdR0_16[8] = { 0 };
    unsigned char BdR1_16[8] = { 0 };
    unsigned char BdmR0_15[8] = { 0 };
    unsigned char BdmR1_15[8] = { 0 };
    count = 0;
    for (n = 0; n < N; n++)
    {
        temp = dis(gen);
        //the input of random
        //printf("Start the encryption!\n");
        L0 = temp % 256;
        temp = int(temp / 256);
        R0 = temp % 256;
        temp = int(temp / 256);
        L1 = temp % 256;
        temp = int(temp / 256);
        R1 = temp % 256;
        //input XOR with input difference
        LL0 = L0 ^ dL0in;
        RR0 = R0 ^ dR0in;
        LL1 = L1 ^ dL1in;
        RR1 = R1 ^ dR1in;

        /*
        printf("\n");
        printf("The %d-th pair\n", n);
        printf("L=\n");
        */
        Shadow_Enc(L0, R0, L1, R1, L0_16, R0_16, L1_16, R1_16, mk);
        //printf("LL=\n");
        Shadow_Enc(LL0, RR0, LL1, RR1, LL0_16, RR0_16, LL1_16, RR1_16, mk);

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

        ToBinary(dmR0_15, BdmR0_15, 8);
        ToBinary(dmR1_15, BdmR1_15, 8);


        /*
        printf("-------------------------------------\n");
        printf("L0=");
        for (int t = 0; t < 8; t++)
        {
            printf("%d", BdL0_16[t]);
        }
        printf("R0=");
        for (int t = 0; t < 8; t++)
        {
            printf("%d", BdmR0_15[t]);
        }
        printf("L1=");
        for (int t = 0; t < 8; t++)
        {
            printf("%d", BdL1_16[t]);
        }
        printf("R1=");
        for (int t = 0; t < 8; t++)
        {
            printf("%d", BdmR1_15[t]);
        }
        printf("\n");
       */
       /*
       int Condition_L0, Condition_L1, Condition_R0, Condition_R1;
       Condition_L0 = 6;
       Condition_L1 = 6;
       Condition_R0 = 8;
       Condition_R1 = 8;
       unsigned char Diff_L0[8] = { 0,0,0,1,0,0,0,0 };
       unsigned char Diff_L1[8] = { 0,0,0,1,0,0,0,0};
       unsigned char Diff_R0[8] = { 0,0,0,0,0,1,0,0 };
       unsigned char Diff_R1[8] = { 0,0,0,0,0,1,0,0 };

       unsigned char Diff_L0_pos[6] = {0,1,2,3,5,7};
       unsigned char Diff_L1_pos[6] = { 0,1,2,3,5,7 };
       unsigned char Diff_R0_pos[8] = { 0,1,2,3,4,5,6,7 };
       unsigned char Diff_R1_pos[8] = { 0,1,2,3,4,5,6,7 };
       */

        for (l0 = 0; l0 < Condition_L0; l0++)
        {
            if (BdL0_16[Diff_L0_pos[l0]] != Diff_L0[Diff_L0_pos[l0]])
            {
                break;
            }

        }
        if (l0 < Condition_L0)
        {
            continue;
        }

        for (l1 = 0; l1 < Condition_L1; l1++)
        {
            if (BdL1_16[Diff_L1_pos[l1]] != Diff_L1[Diff_L1_pos[l1]])
            {
                break;
            }

        }
        if (l1 < Condition_L1)
        {
            continue;
        }

        for (r0 = 0; r0 < Condition_R0; r0++)
        {
            if (BdmR0_15[Diff_R0_pos[r0]] != Diff_R0[Diff_R0_pos[r0]])
            {
                break;
            }

        }
        if (r0 < Condition_R0)
        {
            continue;
        }

        for (r1 = 0; r1 < Condition_R1; r1++)
        {
            if (BdmR1_15[Diff_R1_pos[r1]] != Diff_R1[Diff_R1_pos[r1]])
            {
                break;
            }

        }
        if (r1 < Condition_R1)
        {
            continue;
        }

        cL0[count] = L0_16;
        cLL0[count] = LL0_16;
        cR0[count] = R0_16;
        cRR0[count] = RR0_16;
        cL1[count] = L1_16;
        cLL1[count] = LL1_16;
        cR1[count] = R1_16;
        cRR1[count] = RR1_16;
        count = count + 1;

        //printf("###################################\n");
        //printf("Leave the above pair!\n");





    }
}
void CollectPhaseG(int& count, int N, unsigned long long mk, unsigned char dL0in, unsigned char dR0in, unsigned char dL1in, unsigned char dR1in, int* cL0, int* cLL0, int* cR0, int* cRR0, int* cL1, int* cLL1, int* cR1, int* cRR1)
{

    int n, temp;
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
        temp = dis(gen);
        //the input of random
        //printf("Start the encryption!\n");
        L0 = temp % 256;
        temp = int(temp / 256);
        R0 = temp % 256;
        temp = int(temp / 256);
        L1 = temp % 256;
        temp = int(temp / 256);
        R1 = temp % 256;
        //input XOR with input difference
        /*
        LL0 = L0 ^ diff;
        RR0 = R0 ^ 16;
        LL1 = L1 ^ diff;
        RR1 = R1 ^ 16;
        ran1 = dis(gen) % 2;
        ran2 = dis(gen) % 2;
        rans = ran1 * 8 + ran2 * 2;
        RR0 = RR0 ^ rans;
        ran1 = dis(gen) % 2;
        ran2 = dis(gen) % 2;
        rans = ran1 * 8 + ran2 * 2;
        RR1 = RR1 ^ rans;
        */

        LL0 = L0 ^ diff;
        RR0 = R0 ^ 16;
        LL1 = L1 ^ diff;
        RR1 = R1 ^ 16;
        ran1 = dis(gen) % 2;
        ran2 = dis(gen) % 2;
        rans = ran1 *8 + ran2 * 2;
        RR0 = RR0 ^ rans;
        ran1 = dis(gen) % 2;
        ran2 = dis(gen) % 2;
        rans = ran1 * 8 + ran2 * 2;
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
            //printf("\n");
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
            ////if ((BdL1_16[Pos[difftt][0]] == 0) && (BdL1_16[Pos[difftt][1]] == 0) && (BdL1_16[Pos[difftt][2]] == 0) && (BdmR0_15[Pos[difftt][3]] == 0) && (BdmR0_15[Pos[difftt][4]] == 0) && (BdmR0_15[Pos[difftt][5]] == 0) && (BdmR0_15[Pos[difftt][6]] == 0) && (BdmR0_15[Pos[difftt][7]] == 0) && (BdmR0_15[Pos[difftt][8]] == 0))
            //if ((BdmR0_15[7] == 0) && (BdmR1_15[7] == 0))
            if(1)
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
    //printf("N=%d,ssss=%d,count=%d\n", N, ssss, count);
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


unsigned char R0_15(int i, unsigned char L0, unsigned char R0, unsigned char L1, unsigned char R1, unsigned char ks, unsigned char k0, unsigned char k1)
{
    unsigned char BL0[8] = { 0 };
    unsigned char BR0[8] = { 0 };
    unsigned char BL1[8] = { 0 };
    unsigned char BR1[8] = { 0 };
    unsigned char C0[8] = { 0 };
    unsigned char C1[8] = { 0 };
    unsigned char out;
    int j;
    ToBinary(L0, BL0, 8);
    ToBinary(R0, BR0, 8);
    ToBinary(L1, BL1, 8);
    ToBinary(R1, BR1, 8);
    for (j = 0; j < 8; j++)
    {
        C0[j] = BR0[j] ^ (BL1[(j + 1) % 8] & BL1[(j + 7) % 8]) ^ BL1[(j + 2) % 8];
        C1[j] = BR1[j] ^ (BL0[(j + 1) % 8] & BL0[(j + 7) % 8]) ^ BL0[(j + 2) % 8];
    }
    out = ks ^ BL1[i] ^ C0[(i + 2) % 8] ^ ((k0 ^ C0[(i + 1) % 8]) & (k1 ^ C0[(i + 7) % 8]));
    return out;
}
unsigned char R1_15(int i, unsigned char L0, unsigned char R0, unsigned char L1, unsigned char R1, unsigned char ks, unsigned char k0, unsigned char k1)
{
    unsigned char BL0[8] = { 0 };
    unsigned char BR0[8] = { 0 };
    unsigned char BL1[8] = { 0 };
    unsigned char BR1[8] = { 0 };
    unsigned char C0[8] = { 0 };
    unsigned char C1[8] = { 0 };
    unsigned char out;
    int j;
    ToBinary(L0, BL0, 8);
    ToBinary(R0, BR0, 8);
    ToBinary(L1, BL1, 8);
    ToBinary(R1, BR1, 8);
    for (j = 0; j < 8; j++)
    {
        C0[j] = BR0[j] ^ (BL1[(j + 1) % 8] & BL1[(j + 7) % 8]) ^ BL1[(j + 2) % 8];
        C1[j] = BR1[j] ^ (BL0[(j + 1) % 8] & BL0[(j + 7) % 8]) ^ BL0[(j + 2) % 8];
    }
    out = ks ^ BL0[i] ^ C1[(i + 2) % 8] ^ ((k0 ^ C1[(i + 1) % 8]) & (k1 ^ C1[(i + 7) % 8]));
    return out;
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
void Diff_R0_15_R1_15(int& count, int N, unsigned long long mk, unsigned char dL0in, unsigned char dR0in, unsigned char dL1in, unsigned char dR1in, int* cL0, int* cLL0, int* cR0, int* cRR0, int* cL1, int* cLL1, int* cR1, int* cRR1, unsigned char* Diff_L0, unsigned char* Diff_R0, unsigned char* Diff_L1, unsigned char* Diff_R1, unsigned char* Diff_L0_pos, unsigned char* Diff_R0_pos, unsigned char* Diff_L1_pos, unsigned char* Diff_R1_pos, int Condition_L0, int Condition_R0, int Condition_L1, int Condition_R1)

{
    CollectPhase(count, N, mk, dL0in, dR0in, dL1in, dR1in, cL0, cLL0, cR0, cRR0, cL1, cLL1, cR1, cRR1, Diff_L0, Diff_R0, Diff_L1, Diff_R1, Diff_L0_pos, Diff_R0_pos, Diff_L1_pos, Diff_R1_pos, Condition_L0, Condition_R0, Condition_L1, Condition_R1);
    //When diff=4, k2_0=k2[3],k2_1=k2[5],k2_1=k2[7]
    unsigned char k2_0, k2_1, k2_2, k3_0, k3_1, k3_2;
    int guesskey, keybound, n;
    int keycount[64] = { 0 };
    int Kcount = 0, max = 0;
    unsigned char key[6] = { 0 };
    unsigned char R0v4, R0vv4, R0v6, R0vv6, R1v4, R1vv4, R1v6, R1vv6;
    unsigned char L0, R0, L1, R1, LL1, RR0, LL0, RR1;
    unsigned char ks = 0;
    keybound = 64;
    for (guesskey = 0; guesskey < keybound; guesskey++)
    {
        ToBinary(guesskey, key, 6);
        k2_0 = key[0];
        k2_1 = key[1];
        k2_2 = key[2];
        k3_0 = key[3];
        k3_1 = key[4];
        k3_2 = key[5];

        for (n = 0; n < count; n++)
        {
            L0 = cL0[n];
            L1 = cL1[n];
            R0 = cR0[n];
            R1 = cR1[n];

            LL0 = cLL0[n];
            LL1 = cLL1[n];
            RR0 = cRR0[n];
            RR1 = cRR1[n];

            R0v4 = mR0_14_bit(4, L0, R0, L1, R1, ks, k3_1, k3_0, k2_1, k2_0);
            R0vv4 = mR0_14_bit(4, LL0, RR0, LL1, RR1, ks, k3_1, k3_0, k2_1, k2_0);
            R0v6 = mR0_14_bit(6, L0, R0, L1, R1, ks, k3_2, k3_1, k2_2, k2_1);
            R0vv6 = mR0_14_bit(6, LL0, RR0, LL1, RR1, ks, k3_2, k3_1, k2_2, k2_1);

            R1v4 = mR1_14_bit(4, L0, R0, L1, R1, ks, k3_1, k3_0, k2_1, k2_0);
            R1vv4 = mR1_14_bit(4, LL0, RR0, LL1, RR1, ks, k3_1, k3_0, k2_1, k2_0);
            R1v6 = mR1_14_bit(6, L0, R0, L1, R1, ks, k3_2, k3_1, k2_2, k2_1);
            R1vv6 = mR1_14_bit(6, LL0, RR0, LL1, RR1, ks, k3_2, k3_1, k2_2, k2_1);

            if ((R0v4 ^ R0vv4 == 0) && (R0v6 ^ R0vv6 == 0) && (R1v4 ^ R1vv4 == 0) && (R1v6 ^ R1vv6 == 0))
            {
                keycount[guesskey] = keycount[guesskey] + 1;

            }



        }



    }

    for (int k = 0; k < keybound; k++)
    {
        if (keycount[k] > max)
        {
            max = keycount[k];
        }
    }

    for (int k = 0; k < keybound; k++)
    {
        if (keycount[k] == max)
        {
            printf("%d\n", k);
            Kcount = Kcount + 1;
        }
    }
    printf("Max=%d\n", max);
    printf("Kcount=%d\n", Kcount);
    printf("Rate=%f\n", (1.0 * Kcount) / keybound);


}

void CollectPhase_14(int& count, int& count_14, int N, unsigned long long mk, unsigned char dL0in, unsigned char dR0in, unsigned char dL1in, unsigned char dR1in, int* cL0, int* cLL0, int* cR0, int* cRR0, int* cL1, int* cLL1, int* cR1, int* cRR1, int* cL0_14, int* cLL0_14, int* cR0_14, int* cRR0_14, int* cL1_14, int* cLL1_14, int* cR1_14, int* cRR1_14, unsigned char* Diff_L0, unsigned char* Diff_R0, unsigned char* Diff_L1, unsigned char* Diff_R1, unsigned char* Diff_L0_pos, unsigned char* Diff_R0_pos, unsigned char* Diff_L1_pos, unsigned char* Diff_R1_pos, int Condition_L0, int Condition_R0, int Condition_L1, int Condition_R1)

{
    CollectPhase(count, N, mk, dL0in, dR0in, dL1in, dR1in, cL0, cLL0, cR0, cRR0, cL1, cLL1, cR1, cRR1, Diff_L0, Diff_R0, Diff_L1, Diff_R1, Diff_L0_pos, Diff_R0_pos, Diff_L1_pos, Diff_R1_pos, Condition_L0, Condition_R0, Condition_L1, Condition_R1);
    //When diff=4, k2_0=k2[3],k2_1=k2[5],k2_1=k2[7]
    count_14 = 0;
    unsigned char k2_0, k2_1, k2_2, k3_0, k3_1, k3_2;
    int guesskey, keybound, n, j, x;
    int keycount[128] = { 0 };
    int Kcount = 0, max = 0;
    unsigned char key[7] = { 0 };
    unsigned char R0v4, R0vv4, R0v6, R0vv6, R1v4, R1vv4, R1v6, R1vv6;
    unsigned char L0, R0, L1, R1, LL1, RR0, LL0, RR1;
    unsigned char Bk0[8] = { 0 };
    unsigned char Bk1[8] = { 0 };
    unsigned char Bk2[8] = { 0 };
    unsigned char Bk3[8] = { 0 };
    unsigned char Bk3_15[8] = { 0 };
    unsigned char Bk2_15[8] = { 0 };
    unsigned char Bmr0_14[8] = { 0 };
    unsigned char Bmrr0_14[8] = { 0 };
    unsigned char Bmr1_14[8] = { 0 };
    unsigned char Bmrr1_14[8] = { 0 };
    unsigned char Bdmr1_14[8] = { 0 };
    unsigned char Bdmr0_14[8] = { 0 };
    unsigned char ks = 0;
    keybound = 128;
    for (guesskey = 0; guesskey < 1; guesskey++)
    {
        //printf("\n");
        //printf("Start the new key!!!\n");
        //printf("The %d-th key=\n", guesskey);
        ToBinary(guesskey, key, 7);
        Bk3_15[0] = key[0];
        Bk3_15[1] = key[1];
        Bk3_15[2] = key[2];
        Bk3_15[3] = key[3];
        Bk3_15[4] = key[4];
        Bk3_15[6] = key[5];
        Bk3_15[7] = key[6];

        Bk2_15[0] = key[0];
        Bk2_15[1] = key[1];
        Bk2_15[2] = key[2];
        Bk2_15[3] = key[3];
        Bk2_15[4] = key[4];
        Bk2_15[6] = key[5];
        Bk2_15[7] = key[6];

        for (j = 0; j < BlockSize; j++)
        {
            Bk0[j] = Bk3_15[(j + 1) % BlockSize];
            Bk1[j] = Bk3_15[(j + 7) % BlockSize];
            Bk2[j] = Bk2_15[(j + 1) % BlockSize];
            Bk3[j] = Bk2_15[(j + 7) % BlockSize];
        }
        //printf("Satisfying pairs!!!\n");
        for (n = 0; n < count; n++)
        {
            L0 = cL0[n];
            L1 = cL1[n];
            R0 = cR0[n];
            R1 = cR1[n];

            LL0 = cLL0[n];
            LL1 = cLL1[n];
            RR0 = cRR0[n];
            RR1 = cRR1[n];

            for (x = 0; x < BlockSize; x++)
            {
                Bmr0_14[x] = mR0_14_bit(x, L0, R0, L1, R1, ks, Bk0[x], Bk1[x], Bk2[x], Bk3[x]);
                Bmrr0_14[x] = mR0_14_bit(x, LL0, RR0, LL1, RR1, ks, Bk0[x], Bk1[x], Bk2[x], Bk3[x]);
                Bdmr0_14[x] = Bmr0_14[x] ^ Bmrr0_14[x];

                Bmr1_14[x] = mR1_14_bit(x, L0, R0, L1, R1, ks, Bk0[x], Bk1[x], Bk2[x], Bk3[x]);
                Bmrr1_14[x] = mR1_14_bit(x, LL0, RR0, LL1, RR1, ks, Bk0[x], Bk1[x], Bk2[x], Bk3[x]);
                Bdmr1_14[x] = Bmr1_14[x] ^ Bmrr1_14[x];


            }


            if ((Bdmr0_14[0] == 0) && (Bdmr0_14[1] == 0) && (Bdmr0_14[2] == 0) && (Bdmr0_14[3] == 0) && (Bdmr0_14[5] == 0) && (Bdmr0_14[7] == 0))
            {
                if ((Bdmr1_14[0] == 0) && (Bdmr1_14[1] == 0) && (Bdmr1_14[2] == 0) && (Bdmr1_14[3] == 0) && (Bdmr1_14[5] == 0) && (Bdmr1_14[7] == 0))
                {
                    keycount[guesskey] = keycount[guesskey] + 1;

                    cL0_14[count_14] = L0;
                    cLL0_14[count_14] = LL0;
                    cR0_14[count_14] = R0;
                    cRR0_14[count_14] = RR0;
                    cL1_14[count_14] = L1;
                    cLL1_14[count_14] = LL1;
                    cR1_14[count_14] = R1;
                    cRR1_14[count_14] = RR1;
                    count_14 = count_14 + 1;
                    //printf("%d, ", n);

                }

            }




        }



    }




}

unsigned char mR0_13_bit(int index, unsigned char L0, unsigned char R0, unsigned char L1, unsigned char R1, unsigned char ksum, unsigned char k0, unsigned char k1, unsigned char k2, unsigned char k3, unsigned char k4, unsigned char k5, unsigned char k6, unsigned char k7, unsigned char k8, unsigned char k9)
{

    unsigned char temp, t0, t1, t2, t3, t4, t5, t10, t11, t12, t13, t14, t15, t16;
    int i;
    unsigned char BL0[8] = { 0 };
    unsigned char BR0[8] = { 0 };
    unsigned char BL1[8] = { 0 };
    unsigned char BR1[8] = { 0 };
    unsigned char C0[8] = { 0 };
    unsigned char C1[8] = { 0 };
    unsigned char CL[8] = { 0 };
    unsigned char CL1[8] = { 0 };
    unsigned char CL0[8] = { 0 };
    ToBinary(L0, BL0, 8);
    ToBinary(R0, BR0, 8);
    ToBinary(L1, BL1, 8);
    ToBinary(R1, BR1, 8);
    for (i = 0; i < BlockSize; i++)
    {
        C0[i] = C0bit(i, L0, R0, L1, R1);
        C1[i] = C1bit(i, L0, R0, L1, R1);
    }
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

    t10 = ksum ^ CL1[(index) % BlockSize];
    t11 = k0 ^ CL0[(index + 1) % BlockSize] ^ ((k5 ^ C0[(index + 2) % BlockSize]) & (k4 ^ C0[(index) % BlockSize])) ^ ((k8 ^ C1[(index + 2) % BlockSize]) & (k7 ^ C1[(index) % BlockSize]));
    t12 = CL[(index + 7) % BlockSize] ^ k3;
    t13 = k1 ^ CL0[(index + 7) % BlockSize] ^ ((k4 ^ C0[(index) % BlockSize]) & (k6 ^ C0[(index + 6) % BlockSize])) ^ ((k7 ^ C1[(index) % BlockSize]) & (k9 ^ C1[(index + 6) % BlockSize]));
    t14 = CL[(index + 1) % BlockSize] ^ k2;
    t15 = (CL[(index + 1) % BlockSize] ^ k2) & (CL[(index + 7) % BlockSize] ^ k3);

    t1 = (t10 ^ (t11 & t12) ^ (t13 & t14) ^ t15);

    temp = t1;


    return temp;
}

//
unsigned char mR1_13_bit(int index, unsigned char L0, unsigned char R0, unsigned char L1, unsigned char R1, unsigned char ksum, unsigned char k0, unsigned char k1, unsigned char k2, unsigned char k3, unsigned char k4, unsigned char k5, unsigned char k6, unsigned char k7, unsigned char k8, unsigned char k9)
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

    t0 = mR0_13_bit(index, L0, R0, L1, R1, ksum, k0, k1, k2, k3, k4, k5, k6, k7, k8, k9);
    temp = ksum ^ t0 ^ C0[index] ^ C1[index];


    return temp;
}

void Diff_mR_13(int& count, int& count_14, int N, unsigned long long mk, unsigned char dL0in, unsigned char dR0in, unsigned char dL1in, unsigned char dR1in, int* cL0, int* cLL0, int* cR0, int* cRR0, int* cL1, int* cLL1, int* cR1, int* cRR1, int* cL0_14, int* cLL0_14, int* cR0_14, int* cRR0_14, int* cL1_14, int* cLL1_14, int* cR1_14, int* cRR1_14, unsigned char* Diff_L0, unsigned char* Diff_R0, unsigned char* Diff_L1, unsigned char* Diff_R1, unsigned char* Diff_L0_pos, unsigned char* Diff_R0_pos, unsigned char* Diff_L1_pos, unsigned char* Diff_R1_pos, int Condition_L0, int Condition_R0, int Condition_L1, int Condition_R1)
{
    CollectPhase_14(count, count_14, N, mk, dL0in, dR0in, dL1in, dR1in, cL0, cLL0, cR0, cRR0, cL1, cLL1, cR1, cRR1, cL0_14, cLL0_14, cR0_14, cRR0_14, cL1_14, cLL1_14, cR1_14, cRR1_14, Diff_L0, Diff_R0, Diff_L1, Diff_R1, Diff_L0_pos, Diff_R0_pos, Diff_L1_pos, Diff_R1_pos, Condition_L0, Condition_R0, Condition_L1, Condition_R1);
    //Guess 21 bits key and obtain the dmR0_13[2,3,4,5,7] and dmR1_13[2,3,4,5,7]

    unsigned char k2_0, k2_1, k2_2, k3_0, k3_1, k3_2;
    int guesskey, keybound, n, j, x;
    int* keycount = new int[4194304];
    int Kcount = 0, max = 0;
    int submax=0;
    unsigned char key[22] = { 0 };
    unsigned char R0v4, R0vv4, R0v6, R0vv6, R1v4, R1vv4, R1v6, R1vv6;
    unsigned char L0, R0, L1, R1, LL1, RR0, LL0, RR1;
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

    unsigned char Bk3_15[8] = { 0 };
    unsigned char Bk2_15[8] = { 0 };
    unsigned char Bmr0_13[8] = { 0 };
    unsigned char Bmrr0_13[8] = { 0 };
    unsigned char Bmr1_13[8] = { 0 };
    unsigned char Bmrr1_13[8] = { 0 };
    unsigned char Bdmr1_13[8] = { 0 };
    unsigned char Bdmr0_13[8] = { 0 };
    unsigned char Bkc1[8] = { 0 };
    unsigned char Bkc2[8] = { 0 };

    unsigned char ks = 0;
    keybound = 4194304;
    for (guesskey = 0; guesskey < keybound; guesskey++)
    {

        ToBinaryU(guesskey, key, 22);
        Bk3_15[0] = key[0];
        Bk3_15[1] = key[1];
        Bk3_15[2] = key[2];
        Bk3_15[3] = key[3];
        Bk3_15[4] = key[4];
        Bk3_15[5] = key[5];
        Bk3_15[6] = key[6];
        Bk3_15[7] = key[7];

        Bk2_15[0] = key[0];
        Bk2_15[1] = key[1];
        Bk2_15[2] = key[2];
        Bk2_15[3] = key[3];
        Bk2_15[4] = key[4];
        Bk2_15[5] = key[5];
        Bk2_15[6] = key[6];
        Bk2_15[7] = key[7];

        Bkc1[0] = key[8];
        Bkc1[1] = key[9];
        Bkc1[2] = key[10];
        Bkc1[3] = key[11];
        Bkc1[4] = key[12];
        Bkc1[5] = key[13];
        Bkc1[6] = key[14];

        Bkc2[0] = key[15];
        Bkc2[1] = key[16];
        Bkc2[2] = key[17];
        Bkc2[3] = key[18];
        Bkc2[4] = key[19];
        Bkc2[5] = key[20];
        Bkc2[6] = key[21];

        for (j = 0; j < BlockSize; j++)
        {
            Bk0[j] = Bkc2[(j + 1) % BlockSize];
            Bk1[j] = Bkc2[(j + 7) % BlockSize];
            Bk2[j] = Bkc1[(j + 1) % BlockSize];
            Bk3[j] = Bkc1[(j + 7) % BlockSize];
            Bk4[j] = Bk3_15[j];
            Bk5[j] = Bk3_15[(j + 2) % BlockSize];
            Bk6[j] = Bk3_15[(j + 6) % BlockSize];
            Bk7[j] = Bk2_15[j];
            Bk8[j] = Bk2_15[(j + 2) % BlockSize];
            Bk9[j] = Bk2_15[(j + 6) % BlockSize];


        }

        for (n = 0; n < count_14; n++)
        {
            L0 = cL0[n];
            L1 = cL1[n];
            R0 = cR0[n];
            R1 = cR1[n];

            LL0 = cLL0[n];
            LL1 = cLL1[n];
            RR0 = cRR0[n];
            RR1 = cRR1[n];

            for (x = 0; x < BlockSize; x++)
            {
                Bmr0_13[x] = mR0_13_bit(x, L0, R0, L1, R1, ks, Bk0[x], Bk1[x], Bk2[x], Bk3[x], Bk4[x], Bk5[x], Bk6[x], Bk7[x], Bk8[x], Bk9[x]);
                Bmrr0_13[x] = mR0_13_bit(x, LL0, RR0, LL1, RR1, ks, Bk0[x], Bk1[x], Bk2[x], Bk3[x], Bk4[x], Bk5[x], Bk6[x], Bk7[x], Bk8[x], Bk9[x]);
                Bdmr0_13[x] = Bmr0_13[x] ^ Bmrr0_13[x];

                Bmr1_13[x] = mR1_13_bit(x, L0, R0, L1, R1, ks, Bk0[x], Bk1[x], Bk2[x], Bk3[x], Bk4[x], Bk5[x], Bk6[x], Bk7[x], Bk8[x], Bk9[x]);
                Bmrr1_13[x] = mR1_13_bit(x, LL0, RR0, LL1, RR1, ks, Bk0[x], Bk1[x], Bk2[x], Bk3[x], Bk4[x], Bk5[x], Bk6[x], Bk7[x], Bk8[x], Bk9[x]);
                Bdmr1_13[x] = Bmr1_13[x] ^ Bmrr1_13[x];


            }


            if ((Bdmr0_13[2] == 0) && (Bdmr0_13[3] == 0) && (Bdmr0_13[4] == 0) && (Bdmr0_13[5] == 1) && (Bdmr0_13[7] == 0))
            {
                if ((Bdmr1_13[2] == 0) && (Bdmr1_13[3] == 0) && (Bdmr1_13[4] == 0) && (Bdmr1_13[5] == 1) && (Bdmr1_13[7] == 0))
                {
                    keycount[guesskey] = keycount[guesskey] + 1;




                }

            }




        }



    }
    for (int k = 0; k < keybound; k++)
    {
        if (keycount[k] > max)
        {
            max = keycount[k];
        }
    }
    for (int k = 0; k < keybound; k++)
    {
        if (keycount[k]!=max&&keycount[k] > submax)
        {
            submax = keycount[k];
        }
    }
    ofstream fw;
    fw.open("Result.txt");
    for (int k = 0; k < keybound; k++)
    {
        if (keycount[k] == max)
        {
            //printf("%d\n", k);
            fw << k << endl;
            Kcount = Kcount + 1;
        }
    }
    fw.close();
    printf("Max=%d\n", max);
    printf("SubMax=%d\n", submax);
    printf("1024-th=%d\n", keycount[1024]);
    printf("Kcount=%d\n", Kcount);
    printf("count=%d\n", count);
    printf("count_14=%d\n", count_14);
    printf("Rate=%f\n", (1.0 * Kcount) / keybound);




}

void CollectPhase_13(int& count, int& count_14, int& count_13, int N, unsigned long long mk, unsigned char dL0in, unsigned char dR0in, unsigned char dL1in, unsigned char dR1in, int* cL0, int* cLL0, int* cR0, int* cRR0, int* cL1, int* cLL1, int* cR1, int* cRR1, int* cL0_14, int* cLL0_14, int* cR0_14, int* cRR0_14, int* cL1_14, int* cLL1_14, int* cR1_14, int* cRR1_14, int* cL0_13, int* cLL0_13, int* cR0_13, int* cRR0_13, int* cL1_13, int* cLL1_13, int* cR1_13, int* cRR1_13, unsigned char* Diff_L0, unsigned char* Diff_R0, unsigned char* Diff_L1, unsigned char* Diff_R1, unsigned char* Diff_L0_pos, unsigned char* Diff_R0_pos, unsigned char* Diff_L1_pos, unsigned char* Diff_R1_pos, int Condition_L0, int Condition_R0, int Condition_L1, int Condition_R1)

{
    CollectPhase_14(count, count_14, N, mk, dL0in, dR0in, dL1in, dR1in, cL0, cLL0, cR0, cRR0, cL1, cLL1, cR1, cRR1, cL0_14, cLL0_14, cR0_14, cRR0_14, cL1_14, cLL1_14, cR1_14, cRR1_14, Diff_L0, Diff_R0, Diff_L1, Diff_R1, Diff_L0_pos, Diff_R0_pos, Diff_L1_pos, Diff_R1_pos, Condition_L0, Condition_R0, Condition_L1, Condition_R1);
    //Guess 21 bits key and obtain the dmR0_13[2,3,4,5,7] and dmR1_13[2,3,4,5,7]

    unsigned char k2_0, k2_1, k2_2, k3_0, k3_1, k3_2;
    int guesskey, keybound, n, j, x;
    int* keycount = new int[4194304];
    int Kcount = 0, max = 0;
    unsigned char key[22] = { 0 };
    unsigned char R0v4, R0vv4, R0v6, R0vv6, R1v4, R1vv4, R1v6, R1vv6;
    unsigned char L0, R0, L1, R1, LL1, RR0, LL0, RR1;
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

    unsigned char Bk3_15[8] = { 0 };
    unsigned char Bk2_15[8] = { 0 };
    unsigned char Bmr0_13[8] = { 0 };
    unsigned char Bmrr0_13[8] = { 0 };
    unsigned char Bmr1_13[8] = { 0 };
    unsigned char Bmrr1_13[8] = { 0 };
    unsigned char Bdmr1_13[8] = { 0 };
    unsigned char Bdmr0_13[8] = { 0 };
    unsigned char Bkc1[8] = { 0 };
    unsigned char Bkc2[8] = { 0 };

    unsigned char ks = 0;
    keybound = 4194304;
    for (guesskey = 1024; guesskey < 1025; guesskey++)
    {

        ToBinaryU(guesskey, key, 22);
        Bk3_15[0] = key[0];
        Bk3_15[1] = key[1];
        Bk3_15[2] = key[2];
        Bk3_15[3] = key[3];
        Bk3_15[4] = key[4];
        Bk3_15[5] = key[5];
        Bk3_15[6] = key[6];
        Bk3_15[7] = key[7];

        Bk2_15[0] = key[0];
        Bk2_15[1] = key[1];
        Bk2_15[2] = key[2];
        Bk2_15[3] = key[3];
        Bk2_15[4] = key[4];
        Bk2_15[5] = key[5];
        Bk2_15[6] = key[6];
        Bk2_15[7] = key[7]^1;

        Bkc1[0] = key[8];
        Bkc1[1] = key[9];
        Bkc1[2] = key[10];
        Bkc1[3] = key[11];
        Bkc1[4] = key[12];
        Bkc1[5] = key[13];
        Bkc1[6] = key[14];

        Bkc2[0] = key[15];
        Bkc2[1] = key[16];
        Bkc2[2] = key[17];
        Bkc2[3] = key[18];
        Bkc2[4] = key[19];
        Bkc2[5] = key[20];
        Bkc2[6] = key[21];

        for (j = 0; j < BlockSize; j++)
        {
            Bk0[j] = Bkc2[(j + 1) % BlockSize];
            Bk1[j] = Bkc2[(j + 7) % BlockSize];
            Bk2[j] = Bkc1[(j + 1) % BlockSize];
            Bk3[j] = Bkc1[(j + 7) % BlockSize];
            Bk4[j] = Bk3_15[j];
            Bk5[j] = Bk3_15[(j + 2) % BlockSize];
            Bk6[j] = Bk3_15[(j + 6) % BlockSize];
            Bk7[j] = Bk2_15[j];
            Bk8[j] = Bk2_15[(j + 2) % BlockSize];
            Bk9[j] = Bk2_15[(j + 6) % BlockSize];


        }

        for (n = 0; n < count_14; n++)
        {
            L0 = cL0_14[n];
            L1 = cL1_14[n];
            R0 = cR0_14[n];
            R1 = cR1_14[n];

            LL0 = cLL0_14[n];
            LL1 = cLL1_14[n];
            RR0 = cRR0_14[n];
            RR1 = cRR1_14[n];

            for (x = 0; x < BlockSize; x++)
            {
                Bmr0_13[x] = mR0_13_bit(x, L0, R0, L1, R1, ks, Bk0[x], Bk1[x], Bk2[x], Bk3[x], Bk4[x], Bk5[x], Bk6[x], Bk7[x], Bk8[x], Bk9[x]);
                Bmrr0_13[x] = mR0_13_bit(x, LL0, RR0, LL1, RR1, ks, Bk0[x], Bk1[x], Bk2[x], Bk3[x], Bk4[x], Bk5[x], Bk6[x], Bk7[x], Bk8[x], Bk9[x]);
                Bdmr0_13[x] = Bmr0_13[x] ^ Bmrr0_13[x];

                Bmr1_13[x] = mR1_13_bit(x, L0, R0, L1, R1, ks, Bk0[x], Bk1[x], Bk2[x], Bk3[x], Bk4[x], Bk5[x], Bk6[x], Bk7[x], Bk8[x], Bk9[x]);
                Bmrr1_13[x] = mR1_13_bit(x, LL0, RR0, LL1, RR1, ks, Bk0[x], Bk1[x], Bk2[x], Bk3[x], Bk4[x], Bk5[x], Bk6[x], Bk7[x], Bk8[x], Bk9[x]);
                Bdmr1_13[x] = Bmr1_13[x] ^ Bmrr1_13[x];


            }


            if ((Bdmr0_13[2] == 0) && (Bdmr0_13[3] == 0) && (Bdmr0_13[4] == 0) && (Bdmr0_13[5] == 1) && (Bdmr0_13[7] == 0))
            {
                if ((Bdmr1_13[2] == 0) && (Bdmr1_13[3] == 0) && (Bdmr1_13[4] == 0) && (Bdmr1_13[5] == 1) && (Bdmr1_13[7] == 0))
                {
                    keycount[guesskey] = keycount[guesskey] + 1;
                    cL0_13[count_13] = L0;
                    cLL0_13[count_13] = LL0;
                    cR0_13[count_13] = R0;
                    cRR0_13[count_13] = RR0;
                    cL1_13[count_13] = L1;
                    cLL1_13[count_13] = LL1;
                    cR1_13[count_13] = R1;
                    cRR1_13[count_13] = RR1;
                    count_13 = count_13 + 1;



                }

            }




        }



    }
    /*
    for (int k = 0; k < keybound; k++)
    {
        if (keycount[k] > max)
        {
            max = keycount[k];
        }
    }
    ofstream fw;
    fw.open("Result.txt");
    for (int k = 0; k < keybound; k++)
    {
        if (keycount[k] == max)
        {
            //printf("%d\n", k);
            fw << k << endl;
            Kcount = Kcount + 1;
        }
    }
    fw.close();
    printf("Max=%d\n", max);
    printf("1024-th=%d\n", keycount[1024]);
    printf("Kcount=%d\n", Kcount);
    printf("count=%d\n", count);
    printf("count_14=%d\n", count_14);
    printf("Rate=%f\n", (1.0 * Kcount) / keybound);

    */


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

void Diff_mR0_12_bit_16(int& count, int& count_14, int& count_13, int N, unsigned long long mk, unsigned char dL0in, unsigned char dR0in, unsigned char dL1in, unsigned char dR1in, int* cL0, int* cLL0, int* cR0, int* cRR0, int* cL1, int* cLL1, int* cR1, int* cRR1, int* cL0_14, int* cLL0_14, int* cR0_14, int* cRR0_14, int* cL1_14, int* cLL1_14, int* cR1_14, int* cRR1_14, int* cL0_13, int* cLL0_13, int* cR0_13, int* cRR0_13, int* cL1_13, int* cLL1_13, int* cR1_13, int* cRR1_13, unsigned char* Diff_L0, unsigned char* Diff_R0, unsigned char* Diff_L1, unsigned char* Diff_R1, unsigned char* Diff_L0_pos, unsigned char* Diff_R0_pos, unsigned char* Diff_L1_pos, unsigned char* Diff_R1_pos, int Condition_L0, int Condition_R0, int Condition_L1, int Condition_R1)

{
    //From the 14 round distinguisher, we have known that k2_15=k3_15
    //Guess 16 bit key and judge i,i+2 position difference
    //CollectPhase(count, N, mk, dL0in, dR0in, dL1in, dR1in, cL0, cLL0, cR0, cRR0, cL1, cLL1, cR1, cRR1);
    CollectPhase_13(count, count_14, count_13, N, mk, dL0in, dR0in, dL1in, dR1in, cL0, cLL0, cR0, cRR0, cL1, cLL1, cR1, cRR1, cL0_14, cLL0_14, cR0_14, cRR0_14, cL1_14, cLL1_14, cR1_14, cRR1_14, cL0_13, cLL0_13, cR0_13, cRR0_13, cL1_13, cLL1_13, cR1_13, cRR1_13, Diff_L0, Diff_R0, Diff_L1, Diff_R1, Diff_L0_pos, Diff_R0_pos, Diff_L1_pos, Diff_R1_pos, Condition_L0, Condition_R0, Condition_L1, Condition_R1);
    int* CountKey = new int[16384];
    int NK = 16384;
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
    unsigned char con[8] = { 0,0,0,1,0,0,0,0 };
    //unsigned char dmr013[8] = { 0,0,0,0,0,0,0,1 };


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
    unsigned char Bguesskey[14] = { 0 };
    //ksum = 0;
    int Diffpos = 4;
    //Initialize the array CountKey
    for (int x = 0; x < 16384; x++)
    {
        CountKey[x] = 0;
    }
    for (int x = 0; x < BlockSize; x++)
    {
        Bkc1[x] = con[x];
    }
    //Start guess the 16 bit key for k1_15^k3_14; k0_15^k2_14; k2_15;
    for (guesskey = 0; guesskey < NK; guesskey++)
    {
        ToBinaryU(guesskey, Bguesskey, 14);
        //Guess the 24 bit key
        Bk2_15[(Diffpos + 1) % BlockSize] = Bguesskey[0];
        Bk3_15[(Diffpos + 1) % BlockSize] = Bguesskey[0];
        Bk2_15[(Diffpos + 3) % BlockSize] = Bguesskey[1];
        Bk3_15[(Diffpos + 3) % BlockSize] = Bguesskey[1];
        Bk2_15[(Diffpos + 5) % BlockSize] = Bguesskey[2];
        Bk3_15[(Diffpos + 5) % BlockSize] = Bguesskey[2];
        Bk2_15[(Diffpos + 7) % BlockSize] = Bguesskey[3];
        Bk3_15[(Diffpos + 7) % BlockSize] = Bguesskey[3];

        //Bkc[(Diffpos + 1) % BlockSize] = Bguesskey[4];
        Bkc[(Diffpos + 3) % BlockSize] = Bguesskey[4];
        Bkc[(Diffpos + 5) % BlockSize] = Bguesskey[5];
        Bkc[(Diffpos + 7) % BlockSize] = Bguesskey[6];

        Bkc2[(Diffpos) % BlockSize] = Bguesskey[7];
        Bkc2[(Diffpos + 2) % BlockSize] = Bguesskey[8];
        Bkc2[(Diffpos + 4) % BlockSize] = Bguesskey[9];
        Bkc2[(Diffpos + 6) % BlockSize] = Bguesskey[10];

        //Bkc3[(Diffpos + 1) % BlockSize] = Bguesskey[12];
        Bkc3[(Diffpos + 3) % BlockSize] = Bguesskey[11];
        Bkc3[(Diffpos + 5) % BlockSize] = Bguesskey[12];
        Bkc3[(Diffpos + 7) % BlockSize] = Bguesskey[13];

        //Bkc[0] = 0; Bkc[1] = 0; Bkc[2] = 0; Bkc[3] = 0; Bkc[4] = 0; Bkc[5] = 0; Bkc[6] = 0; Bkc[7] = 0;
        Bkc1[0] = 0; Bkc1[1] = 0; Bkc1[2] = 0; Bkc1[3] = 1; Bkc1[4] = 0; Bkc1[5] = 0; Bkc1[6] = 0; Bkc1[7] = 0;
        //Bkc2[0] = 0; Bkc2[1] = 0; Bkc2[2] = 0; Bkc2[3] = 0; Bkc2[4] = 0; Bkc2[5] = 0; Bkc2[6] = 0; Bkc2[7] = 0;
        //Bkc3[0] = 0; Bkc3[1] = 1; Bkc3[2] = 0; Bkc3[3] = 0; Bkc3[4] = 0; Bkc3[5] = 0; Bkc3[6] = 0; Bkc3[7] = 0;
        //Bk2_15[0] = 0; Bk2_15[1] = 0; Bk2_15[2] = 0; Bk2_15[3] = 0; Bk2_15[4] = 0; Bk2_15[5] = 0; Bk2_15[6] = 0; Bk2_15[7] = 0;
        //Bk3_15[0] = 0; Bk3_15[1] = 0; Bk3_15[2] = 0; Bk3_15[3] = 0; Bk3_15[4] = 0; Bk3_15[5] = 0; Bk3_15[6] = 0; Bk3_15[7] = 0;
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


            if ((dBmr0_12[(Diffpos + 0) % BlockSize] == 0) && (dBmr1_12[(Diffpos + 0) % BlockSize] == 0) && (dBmr0_12[(Diffpos + 2) % BlockSize] == 0) && (dBmr1_12[(Diffpos + 2) % BlockSize] == 0))
            {

                CountKey[guesskey] = CountKey[guesskey] + 1;
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
    printf("the 0-th=%d\n", CountKey[0]);
    printf("Ketcount=%d\n", keycount);
    printf("Ketcount rate=%f\n", 1.0 * keycount / NK);
    printf("#################################################################################\n");




}


unsigned char mR0_11_bit(int index, unsigned char L0, unsigned char R0, unsigned char L1, unsigned char R1, unsigned char ksum, unsigned char kw_1, unsigned char kw_7, unsigned char kc_0, unsigned char kc_2, unsigned char kc_6, unsigned char kc1_1, unsigned char kc1_3, unsigned char kc1_5, unsigned char kc1_7, unsigned char kc2_1, unsigned char kc2_3, unsigned char kc2_5, unsigned char kc2_7, unsigned char kc3_0, unsigned char kc3_2, unsigned char kc3_6, unsigned char kc4_1, unsigned char kc4_7, unsigned char k2_15_0, unsigned char k2_15_2, unsigned char k2_15_4, unsigned char k2_15_6, unsigned char k3_15_0, unsigned char k3_15_2, unsigned char k3_15_4, unsigned char k3_15_6)
{

    //Compute the value of mR0_12[index]
    unsigned char temp, t0, t1, t2, t10, t11, t12, t13, t14, t15, t16, t20, t21, t22, t23, t24, t25, t26, t3;
    unsigned char temp0, temp3, temp4, temp10, temp12, temp11, temp20, temp21, temp22, temp1, temp2;
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
    unsigned char CL2[8] = { 0 };
    unsigned char CL3[8] = { 0 };
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
    //Compute the CL0=L0^C0<<<2^C1<<<2
    for (i = 0; i < BlockSize; i++)
    {
        CL2[i] = BL1[i] ^ C0[(i + 2) % BlockSize] ^ C1[(i + 2) % BlockSize];
    }
    //Compute the CL1=C0^CL<<<2
    for (i = 0; i < BlockSize; i++)
    {
        CL3[i] = C1[i] ^ CL[(i + 2) % BlockSize];
    }

    /*
    printf("********************************\n");
    printf("C0=%d\n", ToInteger(C0, 8));
    printf("C1=%d\n", ToInteger(C1, 8));
    printf("CC=%d\n", ToInteger(CC, 8));
    printf("CL=%d\n", ToInteger(CL, 8));
    printf("CL0=%d\n", ToInteger(CL0, 8));
    printf("CL1=%d\n", ToInteger(CL1, 8));
    printf("CL2=%d\n", ToInteger(CL2, 8));
    printf("CL3=%d\n", ToInteger(CL3, 8));
    */
    //temp0 = ksum ^ CL[(index + 2) % BlockSize] ^ CL1[index] ^ CC[index];
    temp0 = ksum ^ C1[index];

    //the mR0_12<<<1
    temp10 = CL[(index + 7) % BlockSize] ^ kc4_7;

    t0 = CC[(index + 3) % BlockSize] ^ kw_1 ^ CL0[(index + 1) % BlockSize] ^ ((k3_15_2 ^ C0[(index + 2) % BlockSize]) & (k3_15_0 ^ C0[(index + 0) % BlockSize])) ^ ((k2_15_2 ^ C1[(index + 2) % BlockSize]) & (k2_15_0 ^ C1[(index + 0) % BlockSize]));
    //The part of t1
    t10 = kc3_2 ^ CL1[(index + 2) % BlockSize];
    t11 = kc2_3 ^ CL0[(index + 3) % BlockSize] ^ ((k3_15_4 ^ C0[(index + 4) % BlockSize]) & (k3_15_2 ^ C0[(index + 2) % BlockSize])) ^ ((k2_15_4 ^ C1[(index + 4) % BlockSize]) & (k2_15_2 ^ C1[(index + 2) % BlockSize]));
    t12 = CL[(index + 1) % BlockSize] ^ kc1_1;
    t13 = kc2_1 ^ CL0[(index + 1) % BlockSize] ^ ((k3_15_2 ^ C0[(index + 2) % BlockSize]) & (k3_15_0 ^ C0[(index + 0) % BlockSize])) ^ ((k2_15_2 ^ C1[(index + 2) % BlockSize]) & (k2_15_0 ^ C1[(index + 0) % BlockSize]));
    t14 = CL[(index + 3) % BlockSize] ^ kc1_3;
    t15 = (CL[(index + 3) % BlockSize] ^ kc1_3) & (CL[(index + 1) % BlockSize] ^ kc1_1);
    t16 = CC[(index + 0) % BlockSize] ^ kc_0;
    t1 = (t10 ^ (t11 & t12) ^ (t13 & t14) ^ t15) & t16;

    //The part of t2
    t20 = kc3_0 ^ CL1[(index + 0) % BlockSize];
    t21 = kc2_1 ^ CL0[(index + 1) % BlockSize] ^ ((k3_15_2 ^ C0[(index + 2) % BlockSize]) & (k3_15_0 ^ C0[(index + 0) % BlockSize])) ^ ((k2_15_2 ^ C1[(index + 2) % BlockSize]) & (k2_15_0 ^ C1[(index + 0) % BlockSize]));
    t22 = CL[(index + 7) % BlockSize] ^ kc1_7;
    t23 = kc2_7 ^ CL0[(index + 7) % BlockSize] ^ ((k3_15_0 ^ C0[(index + 0) % BlockSize]) & (k3_15_6 ^ C0[(index + 6) % BlockSize])) ^ ((k2_15_0 ^ C1[(index + 0) % BlockSize]) & (k2_15_6 ^ C1[(index + 6) % BlockSize]));
    t24 = CL[(index + 1) % BlockSize] ^ kc1_1;
    t25 = (CL[(index + 7) % BlockSize] ^ kc1_7) & (CL[(index + 1) % BlockSize] ^ kc1_1);
    t26 = CC[(index + 2) % BlockSize] ^ kc_2;
    t2 = (t20 ^ (t21 & t22) ^ (t23 & t24) ^ t25) & t26;

    t3 = ((CC[(index + 2) % BlockSize] ^ kc_2) & (CC[(index + 0) % BlockSize] ^ kc_0));

    temp11 = t0 ^ t1 ^ t2 ^ t3;
    temp1 = temp10 & temp11;

    //the mR0_12<<<7
    temp20 = CL[(index + 1) % BlockSize] ^ kc4_1;

    t0 = CC[(index + 1) % BlockSize] ^ kw_7 ^ CL0[(index + 7) % BlockSize] ^ ((k3_15_0 ^ C0[(index + 0) % BlockSize]) & (k3_15_6 ^ C0[(index + 6) % BlockSize])) ^ ((k2_15_0 ^ C1[(index + 0) % BlockSize]) & (k2_15_6 ^ C1[(index + 6) % BlockSize]));


    t10 = kc3_0 ^ CL1[(index + 0) % BlockSize];
    t11 = kc2_1 ^ CL0[(index + 1) % BlockSize] ^ ((k3_15_2 ^ C0[(index + 2) % BlockSize]) & (k3_15_0 ^ C0[(index + 0) % BlockSize])) ^ ((k2_15_2 ^ C1[(index + 2) % BlockSize]) & (k2_15_0 ^ C1[(index + 0) % BlockSize]));
    t12 = CL[(index + 7) % BlockSize] ^ kc1_7;
    t13 = kc2_7 ^ CL0[(index + 7) % BlockSize] ^ ((k3_15_0 ^ C0[(index + 0) % BlockSize]) & (k3_15_6 ^ C0[(index + 6) % BlockSize])) ^ ((k2_15_0 ^ C1[(index + 0) % BlockSize]) & (k2_15_6 ^ C1[(index + 6) % BlockSize]));
    t14 = CL[(index + 1) % BlockSize] ^ kc1_1;
    t15 = (CL[(index + 1) % BlockSize] ^ kc1_1) & (CL[(index + 7) % BlockSize] ^ kc1_7);
    t16 = CC[(index + 6) % BlockSize] ^ kc_6;
    t1 = (t10 ^ (t11 & t12) ^ (t13 & t14) ^ t15) & t16;

    //The part of t2
    t20 = kc3_6 ^ CL1[(index + 6) % BlockSize];
    t21 = kc2_7 ^ CL0[(index + 7) % BlockSize] ^ ((k3_15_0 ^ C0[(index + 0) % BlockSize]) & (k3_15_6 ^ C0[(index + 6) % BlockSize])) ^ ((k2_15_0 ^ C1[(index + 0) % BlockSize]) & (k2_15_6 ^ C1[(index + 6) % BlockSize]));
    t22 = CL[(index + 5) % BlockSize] ^ kc1_5;
    t23 = kc2_5 ^ CL0[(index + 5) % BlockSize] ^ ((k3_15_6 ^ C0[(index + 6) % BlockSize]) & (k3_15_4 ^ C0[(index + 4) % BlockSize])) ^ ((k2_15_6 ^ C1[(index + 6) % BlockSize]) & (k2_15_4 ^ C1[(index + 4) % BlockSize]));
    t24 = CL[(index + 7) % BlockSize] ^ kc1_7;
    t25 = (CL[(index + 5) % BlockSize] ^ kc1_5) & (CL[(index + 7) % BlockSize] ^ kc1_7);
    t26 = CC[(index + 0) % BlockSize] ^ kc_0;
    t2 = (t20 ^ (t21 & t22) ^ (t23 & t24) ^ t25) & t26;

    t3 = ((CC[(index + 0) % BlockSize] ^ kc_0) & (CC[(index + 6) % BlockSize] ^ kc_6));

    temp21 = t0 ^ t1 ^ t2 ^ t3;
    temp2 = temp20 & temp21;

    temp3 = (CL[(index + 1) % BlockSize] ^ kc4_1) & (CL[(index + 7) % BlockSize] ^ kc4_7);

    t10 = kc2_1 ^ CL0[(index + 1) % BlockSize] ^ ((k3_15_2 ^ C0[(index + 2) % BlockSize]) & (k3_15_0 ^ C0[index])) ^ ((k2_15_2 ^ C1[(index + 2) % BlockSize]) & (k2_15_0 ^ C1[index]));
    t11 = CL[(index + 7) % BlockSize] ^ kc1_7;
    t20 = kc2_7 ^ CL0[(index + 7) % BlockSize] ^ ((k3_15_0 ^ C0[(index + 0) % BlockSize]) & (k3_15_6 ^ C0[(index + 6) % BlockSize])) ^ ((k2_15_0 ^ C1[(index + 0) % BlockSize]) & (k2_15_6 ^ C1[(index + 6) % BlockSize]));
    t21 = CL[(index + 1) % BlockSize] ^ kc1_1;

    t3 = ((CL[(index + 1) % BlockSize] ^ kc1_1) & (CL[(index + 7) % BlockSize]) ^ kc1_7);
    temp4 = (t10 & t11) ^ (t20 & t21) ^ t3;

    temp = temp0 ^ temp1 ^ temp2 ^ temp3 ^ temp4;


    return temp;
}
unsigned char mR1_11_bit(int index, unsigned char L0, unsigned char R0, unsigned char L1, unsigned char R1, unsigned char ksum, unsigned char kw_1, unsigned char kw_7, unsigned char kc_0, unsigned char kc_2, unsigned char kc_6, unsigned char kc1_1, unsigned char kc1_3, unsigned char kc1_5, unsigned char kc1_7, unsigned char kc2_1, unsigned char kc2_3, unsigned char kc2_5, unsigned char kc2_7, unsigned char kc3_0, unsigned char kc3_2, unsigned char kc3_6, unsigned char kc4_1, unsigned char kc4_7, unsigned char k2_15_0, unsigned char k2_15_2, unsigned char k2_15_4, unsigned char k2_15_6, unsigned char k3_15_0, unsigned char k3_15_2, unsigned char k3_15_4, unsigned char k3_15_6)
{

    //Compute the value of mR0_12[index]
    unsigned char temp, t0, t1, t2, t10, t11, t12, t13, t14, t15, t16, t20, t21, t22, t23, t24, t25, t26, t3;
    unsigned char temp0, temp3, temp4, temp10, temp12, temp11, temp20, temp21, temp22, temp1, temp2;
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

    temp0 = ksum ^ C1[index] ^ CC[index];

    //the mR0_12<<<1
    temp10 = CL[(index + 7) % BlockSize] ^ kc4_7;

    t0 = CC[(index + 3) % BlockSize] ^ kw_1 ^ CL0[(index + 1) % BlockSize] ^ ((k3_15_2 ^ C0[(index + 2) % BlockSize]) & (k3_15_0 ^ C0[(index + 0) % BlockSize])) ^ ((k2_15_2 ^ C1[(index + 2) % BlockSize]) & (k2_15_0 ^ C1[(index + 0) % BlockSize]));
    //The part of t1
    t10 = kc3_2 ^ CL1[(index + 2) % BlockSize];
    t11 = kc2_3 ^ CL0[(index + 3) % BlockSize] ^ ((k3_15_4 ^ C0[(index + 4) % BlockSize]) & (k3_15_2 ^ C0[(index + 2) % BlockSize])) ^ ((k2_15_4 ^ C1[(index + 4) % BlockSize]) & (k2_15_2 ^ C1[(index + 2) % BlockSize]));
    t12 = CL[(index + 1) % BlockSize] ^ kc1_1;
    t13 = kc2_1 ^ CL0[(index + 1) % BlockSize] ^ ((k3_15_2 ^ C0[(index + 2) % BlockSize]) & (k3_15_0 ^ C0[(index + 0) % BlockSize])) ^ ((k2_15_2 ^ C1[(index + 2) % BlockSize]) & (k2_15_0 ^ C1[(index + 0) % BlockSize]));
    t14 = CL[(index + 3) % BlockSize] ^ kc1_3;
    t15 = (CL[(index + 3) % BlockSize] ^ kc1_3) & (CL[(index + 1) % BlockSize] ^ kc1_1);
    t16 = CC[(index + 0) % BlockSize] ^ kc_0;
    t1 = (t10 ^ (t11 & t12) ^ (t13 & t14) ^ t15) & t16;

    //The part of t2
    t20 = kc3_0 ^ CL1[(index + 0) % BlockSize];
    t21 = kc2_1 ^ CL0[(index + 1) % BlockSize] ^ ((k3_15_2 ^ C0[(index + 2) % BlockSize]) & (k3_15_0 ^ C0[(index + 0) % BlockSize])) ^ ((k2_15_2 ^ C1[(index + 2) % BlockSize]) & (k2_15_0 ^ C1[(index + 0) % BlockSize]));
    t22 = CL[(index + 7) % BlockSize] ^ kc1_7;
    t23 = kc2_7 ^ CL0[(index + 7) % BlockSize] ^ ((k3_15_0 ^ C0[(index + 0) % BlockSize]) & (k3_15_6 ^ C0[(index + 6) % BlockSize])) ^ ((k2_15_0 ^ C1[(index + 0) % BlockSize]) & (k2_15_6 ^ C1[(index + 6) % BlockSize]));
    t24 = CL[(index + 1) % BlockSize] ^ kc1_1;
    t25 = (CL[(index + 7) % BlockSize] ^ kc1_7) & (CL[(index + 1) % BlockSize] ^ kc1_1);
    t26 = CC[(index + 2) % BlockSize] ^ kc_2;
    t2 = (t20 ^ (t21 & t22) ^ (t23 & t24) ^ t25) & t26;

    t3 = ((CC[(index + 2) % BlockSize] ^ kc_2) & (CC[(index + 0) % BlockSize] ^ kc_0));

    temp11 = t0 ^ t1 ^ t2 ^ t3;
    temp1 = temp10 & temp11;

    //the mR0_12<<<7
    temp20 = CL[(index + 1) % BlockSize] ^ kc4_1;

    t0 = CC[(index + 1) % BlockSize] ^ kw_7 ^ CL0[(index + 7) % BlockSize] ^ ((k3_15_0 ^ C0[(index + 0) % BlockSize]) & (k3_15_6 ^ C0[(index + 6) % BlockSize])) ^ ((k2_15_0 ^ C1[(index + 0) % BlockSize]) & (k2_15_6 ^ C1[(index + 6) % BlockSize]));


    t10 = kc3_0 ^ CL1[(index + 0) % BlockSize];
    t11 = kc2_1 ^ CL0[(index + 1) % BlockSize] ^ ((k3_15_2 ^ C0[(index + 2) % BlockSize]) & (k3_15_0 ^ C0[(index + 0) % BlockSize])) ^ ((k2_15_2 ^ C1[(index + 2) % BlockSize]) & (k2_15_0 ^ C1[(index + 0) % BlockSize]));
    t12 = CL[(index + 7) % BlockSize] ^ kc1_7;
    t13 = kc2_7 ^ CL0[(index + 7) % BlockSize] ^ ((k3_15_0 ^ C0[(index + 0) % BlockSize]) & (k3_15_6 ^ C0[(index + 6) % BlockSize])) ^ ((k2_15_0 ^ C1[(index + 0) % BlockSize]) & (k2_15_6 ^ C1[(index + 6) % BlockSize]));
    t14 = CL[(index + 1) % BlockSize] ^ kc1_1;
    t15 = (CL[(index + 1) % BlockSize] ^ kc1_1) & (CL[(index + 7) % BlockSize] ^ kc1_7);
    t16 = CC[(index + 6) % BlockSize] ^ kc_6;
    t1 = (t10 ^ (t11 & t12) ^ (t13 & t14) ^ t15) & t16;

    //The part of t2
    t20 = kc3_6 ^ CL1[(index + 6) % BlockSize];
    t21 = kc2_7 ^ CL0[(index + 7) % BlockSize] ^ ((k3_15_0 ^ C0[(index + 0) % BlockSize]) & (k3_15_6 ^ C0[(index + 6) % BlockSize])) ^ ((k2_15_0 ^ C1[(index + 0) % BlockSize]) & (k2_15_6 ^ C1[(index + 6) % BlockSize]));
    t22 = CL[(index + 5) % BlockSize] ^ kc1_5;
    t23 = kc2_5 ^ CL0[(index + 5) % BlockSize] ^ ((k3_15_6 ^ C0[(index + 6) % BlockSize]) & (k3_15_4 ^ C0[(index + 4) % BlockSize])) ^ ((k2_15_6 ^ C1[(index + 6) % BlockSize]) & (k2_15_4 ^ C1[(index + 4) % BlockSize]));
    t24 = CL[(index + 7) % BlockSize] ^ kc1_7;
    t25 = (CL[(index + 5) % BlockSize] ^ kc1_5) & (CL[(index + 7) % BlockSize] ^ kc1_7);
    t26 = CC[(index + 0) % BlockSize] ^ kc_0;
    t2 = (t20 ^ (t21 & t22) ^ (t23 & t24) ^ t25) & t26;

    t3 = ((CC[(index + 0) % BlockSize] ^ kc_0) & (CC[(index + 6) % BlockSize] ^ kc_6));

    temp21 = t0 ^ t1 ^ t2 ^ t3;
    temp2 = temp20 & temp21;

    temp3 = (CL[(index + 1) % BlockSize] ^ kc4_1) & (CL[(index + 7) % BlockSize] ^ kc4_7);

    t10 = kc2_1 ^ CL0[(index + 1) % BlockSize] ^ ((k3_15_2 ^ C0[(index + 2) % BlockSize]) & (k3_15_0 ^ C0[index])) ^ ((k2_15_2 ^ C1[(index + 2) % BlockSize]) & (k2_15_0 ^ C1[index]));
    t11 = CL[(index + 7) % BlockSize] ^ kc1_7;
    t20 = kc2_7 ^ CL0[(index + 7) % BlockSize] ^ ((k3_15_0 ^ C0[(index + 0) % BlockSize]) & (k3_15_6 ^ C0[(index + 6) % BlockSize])) ^ ((k2_15_0 ^ C1[(index + 0) % BlockSize]) & (k2_15_6 ^ C1[(index + 6) % BlockSize]));
    t21 = CL[(index + 1) % BlockSize] ^ kc1_1;

    t3 = ((CL[(index + 1) % BlockSize] ^ kc1_1) & (CL[(index + 7) % BlockSize]) ^ kc1_7);
    temp4 = (t10 & t11) ^ (t20 & t21) ^ t3;

    temp = temp0 ^ temp1 ^ temp2 ^ temp3 ^ temp4;


    return temp;
}


void Diff_mR0_11_bit_18(int& count, int& count_14, int& count_13, int N, unsigned long long mk, unsigned char dL0in, unsigned char dR0in, unsigned char dL1in, unsigned char dR1in, int* cL0, int* cLL0, int* cR0, int* cRR0, int* cL1, int* cLL1, int* cR1, int* cRR1, int* cL0_14, int* cLL0_14, int* cR0_14, int* cRR0_14, int* cL1_14, int* cLL1_14, int* cR1_14, int* cRR1_14, int* cL0_13, int* cLL0_13, int* cR0_13, int* cRR0_13, int* cL1_13, int* cLL1_13, int* cR1_13, int* cRR1_13, unsigned char* Diff_L0, unsigned char* Diff_R0, unsigned char* Diff_L1, unsigned char* Diff_R1, unsigned char* Diff_L0_pos, unsigned char* Diff_R0_pos, unsigned char* Diff_L1_pos, unsigned char* Diff_R1_pos, int Condition_L0, int Condition_R0, int Condition_L1, int Condition_R1)

{
    
    CollectPhaseG(count, N, mk, dL0in, dR0in, dL1in, dR1in, cL0, cLL0, cR0, cRR0, cL1, cLL1, cR1, cRR1);
    //CollectPhase_13(count, count_14, count_13, N, mk, dL0in, dR0in, dL1in, dR1in, cL0, cLL0, cR0, cRR0, cL1, cLL1, cR1, cRR1, cL0_14, cLL0_14, cR0_14, cRR0_14, cL1_14, cLL1_14, cR1_14, cRR1_14, cL0_13, cLL0_13, cR0_13, cRR0_13, cL1_13, cLL1_13, cR1_13, cRR1_13, Diff_L0, Diff_R0, Diff_L1, Diff_R1, Diff_L0_pos, Diff_R0_pos, Diff_L1_pos, Diff_R1_pos, Condition_L0, Condition_R0, Condition_L1, Condition_R1);
    int* CountKey = new int[262144];
    int NK = 262144;
    int pair;
    unsigned char ksum;
    unsigned char L0_16, R0_16, L1_16, R1_16, LL0_16, RR0_16, LL1_16, RR1_16, mr0_12, mrr0_12;
    unsigned char Bksum[8] = { 0 };
    unsigned char Bmr0_12[8] = { 0 };
    unsigned char Bmrr0_12[8] = { 0 };
    unsigned char Bmr1_12[8] = { 0 };
    unsigned char Bmrr1_12[8] = { 0 };
    unsigned char Bmr0_11[8] = { 0 };
    unsigned char Bmrr0_11[8] = { 0 };
    unsigned char Bmr1_11[8] = { 0 };
    unsigned char Bmrr1_11[8] = { 0 };
    unsigned char dBmr0_11[8] = { 0 };
    unsigned char dBmr1_11[8] = { 0 };
    unsigned char Bmr0_13[8] = { 0 };
    unsigned char Bmrr0_13[8] = { 0 };
    unsigned char dBmr0_12[8] = { 0 };//the difference in mR0_13
    unsigned char dBmr1_12[8] = { 0 };//the difference in mR0_13
    unsigned char dBmr0_13[8] = { 0 };//the difference in mR0_13
    unsigned char Bk2_15[8] = { 0 };
    unsigned char Bk3_15[8] = { 0 };
    unsigned char Bkc_0[8] = { 0 };
    unsigned char Bkc_2[8] = { 0 };
    unsigned char Bkc_6[8] = { 0 };
    unsigned char Bkc1_1[8] = { 0 };
    unsigned char Bkc1_3[8] = { 0 };
    unsigned char Bkc1_5[8] = { 0 };
    unsigned char Bkc1_7[8] = { 0 };
    unsigned char Bkc2_1[8] = { 0 };
    unsigned char Bkc2_3[8] = { 0 };
    unsigned char Bkc2_5[8] = { 0 };
    unsigned char Bkc2_7[8] = { 0 };
    unsigned char Bkc3_0[8] = { 0 };
    unsigned char Bkc3_2[8] = { 0 };
    unsigned char Bkc3_6[8] = { 0 };
    unsigned char Bkc4_1[8] = { 0 };
    unsigned char Bkc4_7[8] = { 0 };
    unsigned char Bkw_1[8] = { 0 };
    unsigned char Bkw_7[8] = { 0 };
    unsigned char Bk2_15_0[8] = { 0 };
    unsigned char Bk2_15_2[8] = { 0 };
    unsigned char Bk2_15_4[8] = { 0 };
    unsigned char Bk2_15_6[8] = { 0 };
    unsigned char Bk3_15_0[8] = { 0 };
    unsigned char Bk3_15_2[8] = { 0 };
    unsigned char Bk3_15_4[8] = { 0 };
    unsigned char Bk3_15_6[8] = { 0 };
    unsigned char Bkc[8] = { 0 };
    unsigned char Bkc1[8] = { 0,0,0,0,0,0,0,0 };
    unsigned char Bkc2[8] = { 0 };
    unsigned char Bkc3[8] = { 0 };
    unsigned char Bkc4[8] = { 0 };
    unsigned char Bkw[8] = { 0 };
    int guesskey, j;
    int max, keycount;
    max = 0;
    keycount = 0;
    ksum = 0;
    unsigned char Bguesskey[18] = { 0 };
    //ksum = 0;
    int Diffpos =6;
    //Initialize the array CountKey
    for (int x = 0; x < 262144; x++)
    {
        CountKey[x] = 0;
    }

    for (guesskey = 0; guesskey < NK; guesskey++)
    {
        ToBinaryU(guesskey, Bguesskey, 18);
        //Guess the 24 bit key
        Bk2_15[(Diffpos + 0) % BlockSize] = Bguesskey[0];
        Bk3_15[(Diffpos + 0) % BlockSize] = Bguesskey[0];
        Bk2_15[(Diffpos + 2) % BlockSize] = Bguesskey[1];
        Bk3_15[(Diffpos + 2) % BlockSize] = Bguesskey[1];
        Bk2_15[(Diffpos + 4) % BlockSize] = Bguesskey[2];
        Bk3_15[(Diffpos + 4) % BlockSize] = Bguesskey[2];
        Bk2_15[(Diffpos + 6) % BlockSize] = Bguesskey[3];
        Bk3_15[(Diffpos + 6) % BlockSize] = Bguesskey[3];


        Bkc2[(Diffpos + 1) % BlockSize] = Bguesskey[4];
        Bkc2[(Diffpos + 3) % BlockSize] = Bguesskey[5];
        Bkc2[(Diffpos + 5) % BlockSize] = Bguesskey[6];
        Bkc2[(Diffpos + 7) % BlockSize] = Bguesskey[7];

        Bkc3[(Diffpos + 0) % BlockSize] = Bguesskey[8];
        Bkc3[(Diffpos + 2) % BlockSize] = Bguesskey[9];
        Bkc3[(Diffpos + 4) % BlockSize] = Bguesskey[10];
        Bkc3[(Diffpos + 6) % BlockSize] = Bguesskey[11];

        Bkc4[(Diffpos + 1) % BlockSize] = Bguesskey[12];
        Bkc4[(Diffpos + 3) % BlockSize] = Bguesskey[13];
        //Bkc4[(Diffpos + 5) % BlockSize] = Bguesskey[9];
        Bkc4[(Diffpos + 7) % BlockSize] = Bguesskey[14];

        Bkw[(Diffpos + 1) % BlockSize] = Bguesskey[15];
        Bkw[(Diffpos + 3) % BlockSize] = Bguesskey[16];
        //Bkc4[(Diffpos + 5) % BlockSize] = Bguesskey[9];
        Bkw[(Diffpos + 7) % BlockSize] = Bguesskey[17];

        Bkc[0] = 0; Bkc[1] = 0; Bkc[2] = 0; Bkc[3] = 0; Bkc[4] = 0; Bkc[5] = 0; Bkc[6] = 0; Bkc[7] = 0;
        Bkc1[0] = 0; Bkc1[1] = 0; Bkc1[2] = 0; Bkc1[3] = 0; Bkc1[4] = 0; Bkc1[5] = 0; Bkc1[6] = 0; Bkc1[7] = 0;
        //Bkc2[0] = 0; Bkc2[1] = 0; Bkc2[2] = 0; Bkc2[3] = 0; Bkc2[4] = 0; Bkc2[5] = 0; Bkc2[6] = 0; Bkc2[7] = 0;
        //Bkc3[0] = 0; Bkc3[1] = 1; Bkc3[2] = 0; Bkc3[3] = 0; Bkc3[4] = 0; Bkc3[5] = 0; Bkc3[6] = 0; Bkc3[7] = 0;
        //Bk2_15[0] = 0; Bk2_15[1] = 0; Bk2_15[2] = 0; Bk2_15[3] = 0; Bk2_15[4] = 0; Bk2_15[5] = 0; Bk2_15[6] = 0; Bk2_15[7] = 0;
        //Bk3_15[0] = 0; Bk3_15[1] = 0; Bk3_15[2] = 0; Bk3_15[3] = 0; Bk3_15[4] = 0; Bk3_15[5] = 0; Bk3_15[6] = 0; Bk3_15[7] = 0;
        //Compute k1sum-k9
        for (j = 0; j < 8; j++)
        {
            Bkc_0[j] = Bkc[(j + 0) % BlockSize];
            Bkc_2[j] = Bkc[(j + 2) % BlockSize];
            Bkc_6[j] = Bkc[(j + 6) % BlockSize];
            Bkc1_1[j] = Bkc1[(j + 1) % BlockSize];
            Bkc1_3[j] = Bkc1[(j + 3) % BlockSize];
            Bkc1_5[j] = Bkc1[(j + 5) % BlockSize];
            Bkc1_7[j] = Bkc1[(j + 7) % BlockSize];
            Bkc2_1[j] = Bkc2[(j + 1) % BlockSize];
            Bkc2_3[j] = Bkc2[(j + 3) % BlockSize];
            Bkc2_5[j] = Bkc2[(j + 5) % BlockSize];
            Bkc2_7[j] = Bkc2[(j + 7) % BlockSize];
            Bkc3_0[j] = Bkc3[(j + 0) % BlockSize];
            Bkc3_2[j] = Bkc3[(j + 2) % BlockSize];
            Bkc3_6[j] = Bkc3[(j + 6) % BlockSize];
            Bkc4_1[j] = Bkc4[(j + 1) % BlockSize];
            Bkc4_7[j] = Bkc4[(j + 7) % BlockSize];
            Bkw_1[j] = Bkw[(j + 1) % BlockSize];
            Bkw_7[j] = Bkw[(j + 7) % BlockSize];
            Bk2_15_0[j] = Bk2_15[(j + 0) % BlockSize];
            Bk2_15_2[j] = Bk2_15[(j + 2) % BlockSize];
            Bk2_15_4[j] = Bk2_15[(j + 4) % BlockSize];
            Bk2_15_6[j] = Bk2_15[(j + 6) % BlockSize];

            Bk3_15_0[j] = Bk3_15[(j + 0) % BlockSize];
            Bk3_15_2[j] = Bk3_15[(j + 2) % BlockSize];
            Bk3_15_4[j] = Bk3_15[(j + 4) % BlockSize];
            Bk3_15_6[j] = Bk3_15[(j + 6) % BlockSize];

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

                Bmr0_11[k] = mR0_11_bit(k, L0_16, R0_16, L1_16, R1_16, ksum, Bkw_1[k], Bkw_7[k], Bkc_0[k], Bkc_2[k], Bkc_6[k], Bkc1_1[k], Bkc1_3[k], Bkc1_5[k], Bkc1_7[k], Bkc2_1[k], Bkc2_3[k], Bkc2_5[k], Bkc2_7[k], Bkc3_0[k], Bkc3_2[k], Bkc3_6[k], Bkc4_1[k], Bkc4_7[k], Bk2_15_0[k], Bk2_15_2[k], Bk2_15_4[k], Bk2_15_6[k], Bk3_15_0[k], Bk3_15_2[k], Bk3_15_4[k], Bk3_15_6[k]);
                Bmrr0_11[k] = mR0_11_bit(k, LL0_16, RR0_16, LL1_16, RR1_16, ksum, Bkw_1[k], Bkw_7[k], Bkc_0[k], Bkc_2[k], Bkc_6[k], Bkc1_1[k], Bkc1_3[k], Bkc1_5[k], Bkc1_7[k], Bkc2_1[k], Bkc2_3[k], Bkc2_5[k], Bkc2_7[k], Bkc3_0[k], Bkc3_2[k], Bkc3_6[k], Bkc4_1[k], Bkc4_7[k], Bk2_15_0[k], Bk2_15_2[k], Bk2_15_4[k], Bk2_15_6[k], Bk3_15_0[k], Bk3_15_2[k], Bk3_15_4[k], Bk3_15_6[k]);
                dBmr0_11[k] = Bmr0_11[k] ^ Bmrr0_11[k];

                Bmr1_11[k] = mR1_11_bit(k, L0_16, R0_16, L1_16, R1_16, ksum, Bkw_1[k], Bkw_7[k], Bkc_0[k], Bkc_2[k], Bkc_6[k], Bkc1_1[k], Bkc1_3[k], Bkc1_5[k], Bkc1_7[k], Bkc2_1[k], Bkc2_3[k], Bkc2_5[k], Bkc2_7[k], Bkc3_0[k], Bkc3_2[k], Bkc3_6[k], Bkc4_1[k], Bkc4_7[k], Bk2_15_0[k], Bk2_15_2[k], Bk2_15_4[k], Bk2_15_6[k], Bk3_15_0[k], Bk3_15_2[k], Bk3_15_4[k], Bk3_15_6[k]);
                Bmrr1_11[k] = mR1_11_bit(k, LL0_16, RR0_16, LL1_16, RR1_16, ksum, Bkw_1[k], Bkw_7[k], Bkc_0[k], Bkc_2[k], Bkc_6[k], Bkc1_1[k], Bkc1_3[k], Bkc1_5[k], Bkc1_7[k], Bkc2_1[k], Bkc2_3[k], Bkc2_5[k], Bkc2_7[k], Bkc3_0[k], Bkc3_2[k], Bkc3_6[k], Bkc4_1[k], Bkc4_7[k], Bk2_15_0[k], Bk2_15_2[k], Bk2_15_4[k], Bk2_15_6[k], Bk3_15_0[k], Bk3_15_2[k], Bk3_15_4[k], Bk3_15_6[k]);
                dBmr1_11[k] = Bmr1_11[k] ^ Bmrr1_11[k];

            }

            //&& (dBmr1_11[(Diffpos + 0) % BlockSize] == 0) && (dBmr1_11[(Diffpos + 2) % BlockSize] == 0)
            if ((dBmr0_11[(Diffpos + 0) % BlockSize] == 0) && (dBmr0_11[(Diffpos + 2) % BlockSize] == 0) && (dBmr1_11[(Diffpos + 0) % BlockSize] == 0) && (dBmr1_11[(Diffpos + 2) % BlockSize] == 0))
            {

                CountKey[guesskey] = CountKey[guesskey] + 1;
            }





        }

    }
    int submax=0;
    for (int y = 0; y < NK; y++)
    {
        if (CountKey[y] > max)
        {
            max = CountKey[y];
        }
    }
    int subsubmax = 0;
    for (int y = 0; y < NK; y++)
    {
        if (CountKey[y] != max && CountKey[y] > submax)
        {
            submax = CountKey[y];
        }
    }
    for (int y = 0; y < NK; y++)
    {
        if (CountKey[y]!=max&&CountKey[y] != submax&& CountKey[y] > subsubmax)
        {
            subsubmax = CountKey[y];
        }
    }

    ofstream fw;
    fw.open("Result.txt");
    for (int y = 0; y < NK; y++)
    {
        //if ((CountKey[y] == max)|| (CountKey[y] == submax) || (CountKey[y] == subsubmax))
        //if ((CountKey[y] == max) || (CountKey[y] == submax))
        if ((CountKey[y] == max) || (CountKey[y] == submax) )
        {
            keycount = keycount + 1;
            fw << y << endl;
            //printf("%d\n", y);
        }
    }
    fw.close();

    printf("max=%d\n", max);
    printf("submax=%d\n", submax);
    printf("subsubmax=%d\n", subsubmax);
    printf("the 0-th=%d\n", CountKey[0]);
    printf("Ketcount=%d\n", keycount);
    printf("Ketcount rate=%f\n", 1.0 * keycount / NK);
    printf("#################################################################################\n");




}





int main()
{
    int N, count, count_14, count_13;
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

    int* cL0_14 = new int[8192];
    int* cLL0_14 = new int[8192];
    int* cL1_14 = new int[8192];
    int* cLL1_14 = new int[8192];
    int* cR0_14 = new int[8192];
    int* cRR0_14 = new int[8192];
    int* cR1_14 = new int[8192];
    int* cRR1_14 = new int[8192];

    int* cL0_13 = new int[8192];
    int* cLL0_13 = new int[8192];
    int* cL1_13 = new int[8192];
    int* cLL1_13 = new int[8192];
    int* cR0_13 = new int[8192];
    int* cRR0_13 = new int[8192];
    int* cR1_13 = new int[8192];
    int* cRR1_13 = new int[8192];
    int Condition_L0, Condition_L1, Condition_R0, Condition_R1;
    Condition_L0 = 1;
    Condition_L1 = 1;
    Condition_R0 = 3;
    Condition_R1 = 3;
    unsigned char Diff_L0[8] = { 0,0,0,0,0,0,0,0 };
    unsigned char Diff_L1[8] = { 0,0,0,0,0,0,0,0 };
    unsigned char Diff_R0[8] = { 0,0,0,0,0,1,0,0 };
    unsigned char Diff_R1[8] = { 0,0,0,0,0,1,0,0 };

    unsigned char Diff_L0_pos[1] = { 7 };
    unsigned char Diff_L1_pos[1] = { 7 };
    unsigned char Diff_R0_pos[3] = { 0,1,6 };
    unsigned char Diff_R1_pos[3] = { 0,1,6 };
    dL0in = 0;
    dR0in = diff;
    dL1in = 0;
    dR1in = diff;
    //This difference can get 2 bits key information.
    mk = 0x11;
    count = 0;
    count_14 = 0;
    count_13 = 0;
    double start, end, durationTime;
    N = 2048;
    start = clock();
    Diff_mR0_11_bit_18(count, count_14, count_13, N, mk, dL0in, dR0in, dL1in, dR1in, cL0, cLL0, cR0, cRR0, cL1, cLL1, cR1, cRR1, cL0_14, cLL0_14, cR0_14, cRR0_14, cL1_14, cLL1_14, cR1_14, cRR1_14, cL0_13, cLL0_13, cR0_13, cRR0_13, cL1_13, cLL1_13, cR1_13, cRR1_13, Diff_L0, Diff_R0, Diff_L1, Diff_R1, Diff_L0_pos, Diff_R0_pos, Diff_L1_pos, Diff_R1_pos, Condition_L0, Condition_R0, Condition_L1, Condition_R1);
    //GuessKey_15_2(count, N, mk, dL0in, dR0in, dL1in, dR1in, cL0, cLL0, cR0, cRR0, cL1, cLL1, cR1, cRR1);
    end = clock();
    durationTime = ((double)(end - start)) / CLK_TCK;
    //printf("The information in CollecPhase  \n");
    //printf("N=%d\n", N);
    //printf("\n");
    printf("count=%d\n", count);
    //printf("rate=%f\n", 1.0 * count / N);
    printf("Time of 10 round is %f\n", durationTime);



    return 0;
}