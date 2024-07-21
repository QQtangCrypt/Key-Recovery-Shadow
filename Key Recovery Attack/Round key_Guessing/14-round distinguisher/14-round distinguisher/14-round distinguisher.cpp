/*
* This is the key recovery for Shadow32
* In this code, we use a 14-round differential distinguihser (0x00,0x04,0x00,0x04)--->(0x00,0x04,0x00,0x04) to guess 2-bit round key information.
* The other specific information can refer to the table IX in our paper.
* The experiments corresponding other three differential distinguishers are similar.
* Note that we choose the value of N considering the experimental results.
* For example,through experimental verification, the probability of the actual 14-round differential divider is greater than 2^{-14}.
*/


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
//////////////////////////////////////////////// random number generator
uint32_t p = 0xffff;
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
   // }

    for (r = 0; r < ROUND; r++)
    {
        Shadow_Enc_One_Round(L0in, R0in, L1in, R1in, k2[ROUND - 1 - r], k3[ROUND - 1 - r], k0[ROUND - 1 - r], k1[ROUND - 1 - r]);

        //Shadow_Enc_One_Round(L0in, R0in, L1in, R1in, k0[ROUND-1-r], k1[ROUND - 1 - r], k2[ROUND - 1 - r], k3[ROUND - 1 - r]);
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
void CollectPhase(int& count, int N, unsigned long long mk, unsigned char dL0in, unsigned char dR0in, unsigned char dL1in, unsigned char dR1in, int* cL0, int* cLL0, int* cR0, int* cRR0, int* cL1, int* cLL1, int* cR1, int* cRR1)
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
        if ((dmR0_15 == 4) && (dmR1_15 == 4) && (BdL0_16[0] == 0) && (BdL0_16[1] == 0) && (BdL0_16[2] == 0) && (BdL0_16[3] == 0) && (BdL0_16[5] == 0) && (BdL0_16[7] == 0) && (BdL1_16[0] == 0) && (BdL1_16[1] == 0) && (BdL1_16[2] == 0) && (BdL1_16[3] == 0) && (BdL1_16[5] == 0) && (BdL1_16[7] == 0))
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

void GuessKey_15_2(int& count, int N, unsigned long long mk, unsigned char dL0in, unsigned char dR0in, unsigned char dL1in, unsigned char dR1in, int* cL0, int* cLL0, int* cR0, int* cRR0, int* cL1, int* cLL1, int* cR1, int* cRR1)
{
    CollectPhase(count, N, mk, dL0in, dR0in, dL1in, dR1in, cL0, cLL0, cR0, cRR0, cL1, cLL1, cR1, cRR1);
    int CountKey[65536] = { 0 };//Corresponding rk_15_2[3,5,7]
    unsigned char L0, R0, L1, R1, L0_15, LL0_15, R0_15, RR0_15, L1_15, LL1_15, R1_15, L0_16, LL0_16, R0_16, RR0_16, L1_16, LL1_16, R1_16, RR1_16, rk_15_2_3, rk_15_2_5, rk_15_3_2, rk_15_3_0, rk_15_2_2, rk_15_2_0, rk_15_2_4, rk_15_3_4;
    unsigned char tR0_15_4, tR0_15_6, tRR0_15_4, tRR0_15_6, R1_15_4, R1_15_6, RR1_15_4, RR1_15_6, R0_15_4, R0_15_6, RR0_15_4, RR0_15_6, tR1_15_4, tR1_15_6, tRR1_15_4, tRR1_15_6, rk_15_3_5, rk_15_3_3, rk_15_2_1, rk_15_2_7, rk_15_3_1, rk_15_3_7, L0_14_0, LL0_14_0, L0_14_6, LL0_14_6, L0_14_2, LL0_14_2, L0_14_4, LL0_14_4, L0_14_1, LL0_14_1, L0_14_3, LL0_14_3, L1_14_0, LL1_14_0, L1_14_6, LL1_14_6, L1_14_2, LL1_14_2, L1_14_4, LL1_14_4, L1_14_1, LL1_14_1, L1_14_3, LL1_14_3;
    int keyvalue, keycount, i, nk, temp, max;
    unsigned char BL0_16[8] = { 0 };
    unsigned char BL1_16[8] = { 0 };
    unsigned char BR0_16[8] = { 0 };
    unsigned char BR1_16[8] = { 0 };
    unsigned char BL0[8] = { 0 };
    unsigned char BL1[8] = { 0 };
    unsigned char BR0[8] = { 0 };
    unsigned char BR1[8] = { 0 };
    unsigned char out[8] = { 0 };
    unsigned char k2, k3, k0, k1, kk2, kk3, kk0, kk1;
    unsigned char t0, t1;
    int k2v3, k2v5, k2v7, k3v3, k3v5, k3v7, k0v4, k0v6;
    unsigned dR0, dR1;
    kk2 = 16;
    kk3 = 0;
    k0 = 0;
    k1 = 0;
    kk0 = 0;
    kk1 = 0;
    keycount = 0;
    nk = 64;
    //printf("#############################################################################\n");
    //printf("Start the guesss  state!!!!!!!\n");
    /*
    * The value of k0 are not need to be guessed
    */
    for (keyvalue = 0; keyvalue < nk; keyvalue++)
    {

        temp = keyvalue;
        k3v7 = temp % 2;
        temp = int(temp / 2);
        k3v5 = temp % 2;
        temp = int(temp / 2);
        k3v3 = temp % 2;
        temp = int(temp / 2);
        k2v7 = temp % 2;
        temp = int(temp / 2);
        k2v5 = temp % 2;
        temp = int(temp / 2);
        k2v3 = temp % 2;
        //temp = int(temp / 2);
        //k0v6 = temp % 2;
        //temp = int(temp / 2);
        //k0v4 = temp % 2;

        k2 = k2v7 + 4 * k2v5 + 16 * k2v3;
        k3 = k3v7 + 4 * k3v5 + 16 * k3v3;
        k0 = 0;
        // printf("k0=%d,k1=%d,k2=%d,k3=%d\n", k0,k1,k2, k3);

        for (i = 0; i < N; i++)
        {
            L0 = cL0[i];
            L0_16 = cLL0[i];
            R0 = cR0[i];
            R0_16 = cRR0[i];
            L1 = cL1[i];
            L1_16 = cLL1[i];
            R1 = cR1[i];
            R1_16 = cRR1[i];
            ToBinary(L0, BL0, 8);
            ToBinary(L1, BL1, 8);
            ToBinary(R0, BR0, 8);
            ToBinary(R1, BR1, 8);

            //printf("cL0=%d,cR0=%d,cL1=%d,cR1=%d\n", L0, R0, L1, R1);
            //printf("cLL0=%d,cRR0=%d,cLL1=%d,cRR1=%d\n", L0_16, R0_16, L1_16, R1_16);

            Shadow_Dnc_One_Round(L0_16, R0_16, L1_16, R1_16, k0, k1, k2, k3);

            Shadow_Dnc_One_Round_Half(L0_16, R0_16, L1_16, R1_16, kk0, kk1, kk2, kk3);

            Shadow_Dnc_One_Round(L0, R0, L1, R1, k0, k1, k2, k3);

            Shadow_Dnc_One_Round_Half(L0, R0, L1, R1, kk0, kk1, kk2, kk3);

            //printf("+++++++++++++++++++++++++++++++++++++++++++++++++\n");
            //printf("The Guessing 14-round!\n");
            //printf("cL0_14=%d,cR0_14=%d,cL1_14=%d,cR1_14=%d\n", L0, R0, L1, R1);
            //printf("cLL0_14=%d,cRR0_14=%d,cLL1_14=%d,cRR1_14=%d\n", L0_16, R0_16, L1_16, R1_16);

            //printf(";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n");
            //printf("The XOR value of R0 and R1\n");
            //printf("-----------------------------------------------------\n");
            //printf("dR0=%d,dR1=%d\n", R0_16^R0, R1_16^R1);

            //Jie Mi Half Round Has Problem
            dR0 = R0 ^ R0_16;
            dR1 = R1 ^ R1_16;
            ToBinary(dR0, BR0, 8);
            ToBinary(dR1, BR1, 8);


            if ((BR0[4] == 0) && (BR0[6] == 0) && (BR1[4] == 0) && (BR1[6] == 0))
            {
                CountKey[keyvalue] = CountKey[keyvalue] + 1;



            }


        }
    }



    //Output the key guess data
    max = 0;
    for (int j = 0; j < nk; j++)
    {
        printf("%d\n", CountKey[j]);
        //if (CountKey[j] = CountKey[0])
        //{
           // printf("key=%d\n", j);
       // }
        //{
           // max = CountKey[j];
        //}


    }
    printf("The 0-th\n");
    for (int j = 0; j < nk; j++)
    {
        //printf("%d\n", CountKey[j]);
        if (CountKey[j] == CountKey[0])
        {

            ToBinary(j, out, 8);
            printf("%d\n", out[0]);
        }
        //{
           // max = CountKey[j];
        //}


    }
    printf("The 1-th\n");
    for (int j = 0; j < nk; j++)
    {
        //printf("%d\n", CountKey[j]);
        if (CountKey[j] == CountKey[0])
        {

            ToBinary(j, out, 8);
            printf("%d\n", out[1]);
        }
        //{
           // max = CountKey[j];
        //}


    }
    printf("The 2-th\n");
    for (int j = 0; j < nk; j++)
    {
        //printf("%d\n", CountKey[j]);
        if (CountKey[j] == CountKey[0])
        {

            ToBinary(j, out, 8);
            printf("%d\n", out[2]);
        }
        //{
           // max = CountKey[j];
        //}


    }
    printf("The 3-th\n");
    for (int j = 0; j < nk; j++)
    {
        //printf("%d\n", CountKey[j]);
        if (CountKey[j] == CountKey[0])
        {

            ToBinary(j, out, 8);
            printf("%d\n", out[3]);
        }
        //{
           // max = CountKey[j];
        //}


    }
    printf("The 4-th\n");
    for (int j = 0; j < nk; j++)
    {
        //printf("%d\n", CountKey[j]);
        if (CountKey[j] == CountKey[0])
        {

            ToBinary(j, out, 8);
            printf("%d\n", out[4]);
        }
        //{
           // max = CountKey[j];
        //}


    }
    printf("The 5-th\n");
    for (int j = 0; j < nk; j++)
    {
        //printf("%d\n", CountKey[j]);
        if (CountKey[j] == CountKey[0])
        {

            ToBinary(j, out, 8);
            printf("%d\n", out[5]);
        }
        //{
           // max = CountKey[j];
        //}


    }
    printf("The 6-th\n");
    for (int j = 0; j < nk; j++)
    {
        //printf("%d\n", CountKey[j]);
        if (CountKey[j] == CountKey[0])
        {

            ToBinary(j, out, 8);
            printf("%d\n", out[6]);
        }
        //{
           // max = CountKey[j];
        //}


    }
    printf("The 7-th\n");
    for (int j = 0; j < nk; j++)
    {
        //printf("%d\n", CountKey[j]);
        if (CountKey[j] == CountKey[0])
        {

            ToBinary(j, out, 8);
            printf("%d\n", out[7]);
        }
        //{
           // max = CountKey[j];
        //}


    }
    //printf("max=%d", max);
    //printf("The 0-th =%d\n", CountKey[0]);
    //printf("Ketcount=%d\n", keycount);
    //printf("Ketcount rate=%f\n", 1.0*keycount/nk);

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
    mk = 0x11;
    count = 0;
    N = 1000;
    dL0in = 0;
    dR0in = 4;
    dL1in = 0;
    dR1in = 4;
    //This difference can get 2 bits key information.
    GuessKey_15_2(count, N, mk, dL0in, dR0in, dL1in, dR1in, cL0, cLL0, cR0, cRR0, cL1, cLL1, cR1, cRR1);

    return 0;
}