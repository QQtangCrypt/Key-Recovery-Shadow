/*
* This is the key recovery for Shadow32
* Data:2023 12 26
* Exhausitive search phase
* Guess remaining master key bits
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
static const unsigned char P[64] = { 56,57,58,59,16,17,18,19,20,21,22,23,24,25,26,27,60,61,62,63,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };
static const unsigned char Sinv[64] = { 0, 1, 2, 3, 5, 0, 7, 0, 10, 11, 0, 0, 15, 0, 0, 0, 4, 5, 6, 7, 0, 0, 0, 0, 14, 15, 0, 0, 0, 0, 0, 0, 8, 9, 10, 11, 13, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
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
void ToBinaryU(unsigned long long v, unsigned char* B, int length)
{
    int i;
    for (i = 0; i < length; i++)
    {
        B[i] = 0;
    }
    for (i = 0; i < length; i++)
    {
        B[length - 1 - i] = v % 2;
        v = unsigned long long(v / 2);
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

unsigned char ToIntegerU(unsigned char* B, unsigned long long length)
{
    unsigned long long out;
    unsigned long long i, t;
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
    //for (int m = 0; m < 64; m++)
    //{
        //printf("%d", K[m]);
    //}
    //printf("\n");
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
        //{
            //printf("%d", AC[i]);
        //}
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
            //printf("%d", NX[i]);
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

        Shadow_Enc_One_Round(L0in, R0in, L1in, R1in, k0[r], k1[r], k2[r], k3[r]);
        //printf("Round=%d,k0=%d,k1=%d,k2=%d,k3=%d,L0=%d,R0=%d,L1=%d,R1=%d\n", r, k0[r], k1[r], k2[r], k3[r],L0in, R0in, L1in, R1in);
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


int main()
{
    int count, count_14, count_13;
    unsigned long long mk, N, n;
    unsigned char dL0in, dR0in, dL1in, dR1in;
    unsigned char L0, L1, R0, R1, LL0, LL1, RR0, RR1, k0, k1, k2, k3;
    unsigned char L0out, L1out, R0out, R1out, LL0out, LL1out, RR0out, RR1out;
    unsigned char vL0out, vL1out, vR0out, vR1out, vLL0out, vLL1out, vRR0out, vRR1out;
    double start, end, duration;
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
    unsigned char K0[16] = { 0 };
    unsigned char K1[16] = { 0 };
    unsigned char K2[16] = { 0 };
    unsigned char K3[16] = { 0 };
    dL0in = 0;
    dR0in = diff;
    dL1in = 0;
    dR1in = diff;
    L0 = 0;
    R0 = 1;
    L1 = 2;
    R1 = 3;
    vL0out = 20;
    vR0out = 23;
    vL1out = 23;
    vR1out = 18;

    LL0 = 71;
    RR0 = 1;
    LL1 = 26;
    RR1 = 3;
    vLL0out = 32;
    vRR0out = 102;
    vLL1out = 124;
    vRR1out = 223;
    count = 0;
    count_14 = 0;
    count_13 = 0;
    N = 33554432;
    unsigned char K[64] = { 0 };
    unsigned char KK[64] = { 0 };
    unsigned char gk[36] = { 0 };
    start = clock();
  


    for (n = 0; n < 68719476736; n++)
    {
        ToBinaryU(n, gk, 36);
        K[20] = gk[0];
        K[21] = gk[1];
        K[22] = gk[2];
        K[23] = gk[3];
        K[24] = K[20];
        K[25] = K[21];
        K[26] = K[22];
        K[27] = K[23];

        KK[20] = K[20] & (K[20] ^ K[26]);
        KK[21] = K[21] & (K[21] ^ K[27]);
        KK[22] = K[22] & (K[22] ^ K[20] ^ K[26]);
        KK[23] = K[23] & (K[23] ^ K[21] ^ K[27]);

        K[36] = KK[20];
        K[37] = KK[21];
        K[38] = KK[22];
        K[39] = KK[23];
        
        K[28] = K[36];
        K[29] = K[37];
        K[30] = K[38];
        K[31] = K[39];

        K[44] = K[36];
        K[45] = K[37];
        K[46] = K[38];
        K[47] = K[39];

        K[52] = K[44];
        K[53] = K[45];
        K[54] = K[46];
        K[55] = K[47];

        KK[8] = K[28];
        KK[9] = K[29];
        KK[10] = K[30];
        KK[11] = K[31];





        K[0] = gk[4];
        K[1] = gk[5];
        K[2] = gk[6];
        K[3] = gk[7];
        K[4] = gk[8];
        K[5] = gk[9];
        K[6] = gk[10];
        K[7] = gk[11];
        K[12] = gk[12];
        K[13] = gk[13];
        K[14] = gk[14];
        K[15] = gk[15];

        unsigned char tempS,tempout;
        tempS = K[14] * 32 + K[15] * 16 + KK[8] * 8 + KK[9] * 4 + KK[10] * 2 + KK[11];

        tempout = Sinv[tempS];
        K[11] = tempout % 2;
        tempout = int(tempout / 2);
        K[10] = tempout % 2;
        tempout = int(tempout / 2);
        K[9] = tempout % 2;
        tempout = int(tempout / 2);
        K[8] = tempout % 2;
  
        

 


        //The information exacted from round key




        K[32] = gk[16];
        K[33] = gk[17];
        K[34] = gk[18];
        K[35] = gk[19];
        K[40] = gk[20];
        K[41] = gk[21];
        K[42] = gk[22];
        K[43] = gk[23];
        K[48] = gk[24];
        K[49] = gk[25];
        K[50] = gk[26];
        K[51] = gk[27];
        K[60] = gk[28];
        K[61] = gk[29];


        K[56] = gk[30];
        K[57] = gk[31];
        K[58] = gk[32];
        K[59] = gk[33];
        K[62] = gk[34];
        K[63] = gk[35];

        KK[56] = K[56] & (K[56] ^ K[62]);
        KK[57] = K[57] & (K[57] ^ K[63]);
        KK[58] = K[58] & (K[58] ^ K[56] ^ K[62]);
        KK[59] = K[59] & (K[59] ^ K[57] ^ K[63]);

        K[16] = KK[56];
        K[17] = KK[57];
        K[18] = KK[58];
        K[19] = KK[59];

        mk = ToIntegerU(K, 64);
        Shadow_Enc(L0, R0, L1, R1, L0out, R0out, L1out, R1out, mk);
        //printf("pp=%d %d %d %d\n", L0out, R0out, L1out, R1out);
        //Shadow_Enc(LL0, RR0, LL1, RR1, LL0out, RR0out, LL1out, RR1out, mk);
        //printf("pp=%d %d %d %d\n", LL0out, RR0out, LL1out, RR1out);
        if ((L0out == vL0out) && (R0out == vR0out) && (L1out == vL1out) && (R1out == vR1out))
        {
            printf("============================================\n");
            //The key schedule for Shadow is not a bijection, thus there may be many equivalent master key.
            printf("The master key is found!\n");
            //printf("The right key is: %lld\n", n);
            for (int tt = 0; tt < 64; tt++)
            {
                printf("%d", K[tt]);
            }
            printf("\n");
            break;
        }

    }

    //printf("%d %d %d %d\d", L0out, R0out, L1out, R1out);
    end = clock();
    duration = (double)(end - start) / CLOCKS_PER_SEC;
    printf("The try time is %f\n", duration);


    return 0;
}