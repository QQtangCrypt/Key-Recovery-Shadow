/*
* This is the plaintext recovery for Shadow32
* Data: 2023 11 20
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
static const unsigned char P[64] = { 48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,4,5,6,7,8,9,10,11,12,13,14,15,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,0,1,2,3,16,17,18,19 };
static const unsigned char pk0[8] = { 0,1,2,3,8,9,10,11 };
static const unsigned char pk1[8] = { 4,5,6,7,12,13,14,15 };
static const unsigned char pk2[8] = { 16,17,18,19,24,25,26,27 };
static const unsigned char pk3[8] = { 20,21,22,23,28,29,30,31 };


//unsigned char 0--2^8-2

void ToBinary(unsigned long long v, unsigned char* B, int length)
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
    ToBinary(mk, K, 64);

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

void Shadow_Enc(unsigned char& L0, unsigned char& R0, unsigned char& L1, unsigned char& R1, unsigned long long mk)
{
    int r;
    unsigned char k0[ROUND] = { 0 };
    unsigned char k1[ROUND] = { 0 };
    unsigned char k2[ROUND] = { 0 };
    unsigned char k3[ROUND] = { 0 };
    KeySchedule(mk, k0, k1, k2, k3);
    for (r = 0; r < ROUND; r++)
    {
        Shadow_Enc_One_Round(L0, R0, L1, R1, k0[r], k1[r], k2[r], k3[r]);
    }

}



int main()
{

    srand((int)time(0));
    unsigned char L0, R0, L1, R1;
    unsigned char l0, r0, l1, r1, ll0, rr0, ll1, rr1;
    clock_t start, finish;
    double duration, sum = 0, avg = 0;
    //test the correntness of key schdele 
    unsigned long long mk;
    unsigned long long message;
    unsigned char k0[ROUND] = { 0 };
    unsigned char k1[ROUND] = { 0 };
    unsigned char k2[ROUND] = { 0 };
    unsigned char k3[ROUND] = { 0 };
    unsigned char dL0 = 0;
    unsigned char dR0 = 4;
    unsigned char dL1 = 0;
    unsigned char dR1 = 4;
    unsigned char cL0, cR0, cL1, cR1;
    unsigned char BL0[8] = { 0 };
    unsigned char BR0[8] = { 0 };
    unsigned char BL1[8] = { 0 };
    unsigned char BR1[8] = { 0 };
    unsigned char CipherL0[5096] = { 0 };
    unsigned char CipherR0[5096] = { 0 };
    unsigned char CipherL1[5096] = { 0 };
    unsigned char CipherR1[5096] = { 0 };
    unsigned char CipherLL0[5096] = { 0 };
    unsigned char CipherRR0[5096] = { 0 };
    unsigned char CipherLL1[5096] = { 0 };
    unsigned char CipherRR1[5096] = { 0 };
    unsigned long long count = 0;
    unsigned char gk16;
    unsigned char Gk16[256] = { 0 };
    unsigned char gk15;
    unsigned char Gk15[256] = { 0 };
    unsigned char temp0, temp1;
    unsigned long long n, tt;
    unsigned char vk, co;
    //mk = dis(gen)*dis(gen)*dis(gen)*dis(gen);
    mk = 0x11111;
    start = clock();
    L0 = 0;
    R0 = 0;
    L1 = 0;
    R1 = 0;
    Shadow_Enc(L0, R0, L1, R1, mk);
    vk = L0 ^ L1;



    cL0 = 182;
    cR0 = 174;
    cL1 = 176;
    cR1 = 161;
    co = vk ^ cL0 ^ cL1;
    //printf("%d\n", co);
    //Shadow_Enc(L0, R0, L1, R1, mk);
    printf("The Ciphertext is %d %d %d %d\n", cL0, cR0, cL1, cR1);



    for (n = 0; n < 0xffffff; n++)
    {
        l0 = n % 256;
        tt = unsigned long long(n / 256);
        r0 = tt % 256;
        tt = unsigned long long(tt / 256);
        r1 = tt % 256;
        l1 = l0 ^ co;
        ll0 = l0;
        rr0 = r0;
        ll1 = l1;
        rr1 = r1;
        Shadow_Enc(l0, r0, l1, r1, mk);

        if (l0 == cL0 && l1 == cL1 && r0 == cR0 && r1 == cR1)
        {
            printf("The plain text is found!\n");
            printf("plaintext=l0=%d, r0=%d, l1=%d,r1=%d\n", ll0, rr0, ll1, rr1);
            break;
        }
    }
    if (n == 0xffffff)
        printf("The plain text are not found!\n");

    printf("count=%d,n=%x\n", count, n);

    return 0;
}