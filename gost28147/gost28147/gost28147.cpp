#include "stdafx.h"
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <cstdio>
#include <windows.h>

using namespace std;
#define _CRT_SECURE_NO_WARNINGS
typedef unsigned char	byte;

//���������� ������ �����
long filesize(FILE *stream)
{
	long curpos, length;
	curpos = ftell(stream);
	fseek(stream, 0L, SEEK_END);
	length = ftell(stream);
	fseek(stream, curpos, SEEK_SET);
	return length;
}

//������� ���� 28147-89, ������� ������
void rpz(int mode, char* source_str, char* target_str)
{
	//�������� � �������� ����
	FILE *f_begin, *f_end; 
	//������� ������
	byte Tab_Z[8][16] =
	{
		0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
		0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
		0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
		0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
		0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
		0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
		0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
		0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF
	};
	//����
	unsigned long key[8] =
	{
		0x0123,
		0x4567,
		0x89AB,
		0xCDEF,
		0x0123,
		0x4567,
		0x89AB,
		0xCDEF
	};
	//���������� 32-���������,
	char N[4]; 
	//���������� N1, N2 � ��������
	unsigned long n1 = 0, n2 = 0, SUM232 = 0; 
	//��������� �����
	f_begin = fopen(source_str, "rb");
	f_end = fopen(target_str, "wb");
	//��������� ���������� ������
	float number_block;
	number_block = 8 * filesize(f_begin);
	number_block = number_block / 64;
	int block = number_block;
	//�������� � ����������� �����
	if (number_block - block>0) block++;
	int sh;
	if (filesize(f_begin) >= 4) 
		sh = 4;	
	else sh = filesize(f_begin);
	int sh1 = 0;
	int flag = 0;
	//���� � �������������� ������ � ��������� �� �������, ����� ��������� �������� � ������� � ������������ ������
	for (int i = 0; i<block; i++)
	{
		//������ � ���������� N1
		for (int q = 0; q<4; q++) 
			*((byte *)&N + q) = 0x00;
		if ((sh1 + sh)<filesize(f_begin))
		{
			fread(N, sh, 1, f_begin);
			sh1 += sh;
		}
		else
		{
			sh = filesize(f_begin) - sh1;
			fread(N, sh, 1, f_begin);
			flag = 1;
		}
		n1 = *((unsigned long *)&N);
		//������ � ���������� N2
		for (int q = 0; q<4; q++) *((byte *)&N + q) = 0x00;
		if ((sh1 + sh)<filesize(f_begin))
		{
			fread(N, sh, 1, f_begin);
			sh1 += sh;
		}
		else
		{
			if (flag == 0)
			{
				sh = filesize(f_begin) - sh1;
				fread(N, sh, 1, f_begin);
			}
		}
		n2 = *((unsigned long *)&N);
		//����� ������� ������
		int c = 0;
		for (int k = 0; k<32; k++)
		{
			if (mode == 1) 
			{ 
				if (k == 24) 
					c = 7; 
			}
			else 
			{ 
				if (k == 8) 
					c = 7; 
			}
			//��������� � ��������� ��1
			SUM232 = key[c] + n1;
			//�������� �� ������� ������ Tab_Z
			byte first_byte = 0, second_byte = 0, zam_symbol = 0;
			int n = 7;
			for (int q = 3; q >= 0; q--)
			{
				zam_symbol = *((byte *)&SUM232 + q);
				first_byte = (zam_symbol & 0xF0) >> 4;
				second_byte = (zam_symbol & 0x0F);
				first_byte = Tab_Z[n][first_byte];
				n--;
				second_byte = Tab_Z[n][second_byte];
				n--;
				zam_symbol = (first_byte << 4) | second_byte;
				*((byte *)&SUM232 + q) = zam_symbol;
			}
			//����������� ����� �� 11
			SUM232 = (SUM232 << 11) | (SUM232 >> 21);
			//���������� � ��������� ��2
			SUM232 = n2^SUM232; 
			if (k<31)
			{
				n2 = n1;
				n1 = SUM232;
			}
			if (mode == 1)
			{
				if (k<24)
				{
					c++;
					if (c>7) c = 0;
				}
				else
				{
					c--;
					if (c<0) c = 7;
				}
			}
			else
			{
				if (k<8)
				{
					c++;
					if (c>7) c = 0;
				}
				else
				{
					c--;
					if (c<0) c = 7;
				}
			}
		}
		n2 = SUM232;
		//����� � ����
		char sym_rez;
		for (int q = 0; q <= 3; q++)
		{
			sym_rez = *((byte *)&n1 + q);
			fprintf(f_end, "%c", sym_rez);
		}
		for (int q = 0; q <= 3; q++)
		{
			sym_rez = *((byte *)&n2 + q);
			fprintf(f_end, "%c", sym_rez);
		}
	}
	fclose(f_begin);
	fclose(f_end);
}

int main()
{
	setlocale(LC_ALL, "Russian");
	//�������� �����
	int mode = 0;
	do
	{
		printf("Mode of operation:\n");
		printf("Encryption 1\n");
		printf("Decryption 2\n");
		scanf("%d", &mode);
	} while ((mode != 1) && (mode != 2)); //���� ���� �� ������ 1 ��� 2
	//�������� ����� ������� � ���������
	char source_str[100], target_str[100];
	printf("Enter the name of the source file:\n");
	scanf("%s", &source_str);
	printf("Enter the name of the target file:\n");
	scanf("%s", &target_str);
	//��������� ��������
	rpz(mode, source_str, target_str);
	return 0;
}