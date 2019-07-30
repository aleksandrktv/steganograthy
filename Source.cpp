//#include "stdafx.h"

#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include "Windows.h";
#include<clocale>
#include<string>
#include <sstream>
#include<fstream>
#include <ctime>
using namespace std;

string toBinary(char c);
void randomGenerator(int srandValue, int count, int length, int *mas1,int *mas2);
int toDec(int *mas);

int main()
{

	int sum=0;
	unsigned int start_time = clock(); // начальное время
									   // здесь должен быть фрагмент кода, время выполнения которого нужно измерить
	 //ЧТЕНИЕ СООБЩЕНИЯ, КОДИРОВАНИЕ И Т.Д.
	setlocale(LC_ALL, "rus");
	using std::fseek;
	using std::fread;
	using std::fclose;
	ifstream in("input_msg.txt", std::ifstream::ate| ios::binary);
	int sizeMsg = in.tellg();
	in.seekg(0, std::ios::beg);
	std::cout << "Size messege = " << sizeMsg << endl;
	std::cout << "Size messege*8 = " << sizeMsg*8 << endl;

	string msg;
	for (int i = 0; i < sizeMsg; i++) {
		char ss = in.get();
		/*if (ss == '\n') {
			sizeMsg--;
		}*/
		
			std::cout << ss << ",";
			//msg += toBinary(in.get());		//преобразовали сообщение в набор битов типа String
			msg += toBinary(ss);
	
	}



	int *str = new int[sizeMsg * 8];
	for (int i = 0; i <sizeMsg * 8; i++)
		str[i] = msg[i] & 0x0F;			//теперь строку String преобразовали в набор битов Int
	std::cout << "Old Messege" << endl;//Вывод для проверки
/*	for (int i = 0; i <sizeMsg * 8; i++) 
		std::cout<< str[i];
	std::cout << endl;
	*/
	
	for (int i = 0; i < sizeMsg * 8; i++) {
		std::cout << str[i];
		if ((i + 1) % 8 == 0) std::cout << endl;
	}
	in.close();
	ofstream fout("outputstr.txt");
	
//	fout.open();
	int decodeMs[8];
	int z = 0;
	for (int i = 0; i <sizeMsg * 8; i++) {

		decodeMs[z++] = str[i];
		if ((i + 1) % 8 == 0) {
			std::cout << (char)toDec(decodeMs);
			fout << (char)toDec(decodeMs);
			z = 0;
		}

	}
	fout.close();
	//ЧАСТЬ СО СТРУКТУРАМИ БМП, ТО ЕСТЬ РАБОТА С ИЗОБРАЖЕНИЕМ  
	// Объявляем структуры
	BITMAPFILEHEADER bfh_l;
	BITMAPINFOHEADER bih_l;
	RGBTRIPLE rgb_l;

	FILE * f1, *f2;

	f1 = fopen("lena.bmp", "r+b");
	f2 = fopen("lena3.bmp", "w+b");


	fread(&bfh_l, sizeof(bfh_l), 1, f1);               //Запихиваем файловый заголовок в структуру BITMAPFILEHEADER
	fwrite(&bfh_l, sizeof(bfh_l), 1, f2);   //
	fread(&bih_l, sizeof(bih_l), 1, f1);               //Запихиваем заголовок изображения в структуру BITMAPINFOHEADER
	fwrite(&bih_l, sizeof(bih_l), 1, f2);   //

	size_t padding = 0;
	if ((bih_l.biWidth * 3) % 4) padding = 4 - (bih_l.biWidth * 3) % 4;

	
	int sizeImg = bih_l.biWidth*bih_l.biHeight*3;//Может быть и вообще не надо
	/*cout << "width*height =" << sizeImg << endl;
	cout << "size of image =" << sizeImg * 3 << endl;
	*/

	int countPix = sizeMsg * 3;
	int *masRandomValueString = new int[countPix];
	int *masRandomValueColumn = new int[countPix];

	//цель: получение рандомных неповторяющихся координат для пикселей
//	masRandomValueString = randomGenerator(7, countPix, bih_l.biWidth,masRandomValueString); //генерируем нужное количество рандомных итых
//	masRandomValueColumn = randomGenerator(11, countPix, bih_l.biHeight,masRandomValueColumn);//генерируем нужное количество рандомных житых
	
	randomGenerator(7, countPix, bih_l.biWidth, masRandomValueString,masRandomValueColumn); //генерируем нужное количество рандомных итых
				
	/*std::cout << endl;
	for (int i = 0; i < countPix; i++)
		std::cout << masRandomValueString[i] << " : " << masRandomValueColumn[i] << endl;
	*/
/*for (int i = 0; i < countPix; i++) {
		masRandomValueString[i] = 0;
		masRandomValueColumn[i] = i;
	}*/

	 fout.open("log.txt");
	
	/*for (int i = 0; i < countPix; i++) {
		fout << masRandomValueString[i] << ":" << masRandomValueColumn[i] <<" i = "<<i <<endl;
	}*/
	//fout.close();

	int **b_mas = new int*[bih_l.biHeight];
	int **g_mas = new int*[bih_l.biHeight];
	int **r_mas = new int*[bih_l.biHeight];
	for (int i = 0; i < bih_l.biWidth; i++) {
		b_mas[i] = new int[bih_l.biHeight];
		g_mas[i] = new int[bih_l.biHeight];
		r_mas[i] = new int[bih_l.biHeight];
	}


	int k = 0; // показатель для схемы вставки : четный- 2 слоя, нечетный-1
	int ii = 0;//индекс бита в сообщении
	int jj = 0;
	//ЧАСТЬ ЗАПИСИ В БАЙТИКИ
	for (int i = 0; i< bih_l.biHeight; i++){
		for (int j = 0; j < bih_l.biWidth; j++){

			fread(&rgb_l, sizeof(rgb_l), 1, f1);//чтение исходного изображения
			b_mas[i][j] = rgb_l.rgbtBlue;
			g_mas[i][j] = rgb_l.rgbtGreen;
			r_mas[i][j] = rgb_l.rgbtRed;

			for(int count=0;count<countPix;count++)
				if ((i == masRandomValueString[count]) && (j == masRandomValueColumn[count])) {//то есть если попадаем на наш рандомный пиксель
						
					fout << " i:j = " << i << ":" << j << "  k=" << k <<" ii="<<ii<< endl;
						//Схема вставки 2-1-2
					if ((k-1) % 3 == 0) {
						//вставляет один слой: синий

						//вставляем два бита в синий слой

						if (str[ii] == 1)
							rgb_l.rgbtBlue |= (1 << 1);
						else
							rgb_l.rgbtBlue &= ~(1 << 1);
						ii++;
						if (str[ii] == 1)
							rgb_l.rgbtBlue |= (1 << 0);
						else
							rgb_l.rgbtBlue &= ~(1 << 0);

						ii++;
						k++;
					}
					else {
						
						//вставляет два слоя: Синий и зеленый

						//вставляем два бита в синий слой

						if (str[ii] == 1)
							rgb_l.rgbtBlue |= (1 << 1);
						else
							rgb_l.rgbtBlue &= ~(1 << 1);
						ii++;

						if (str[ii] == 1)
							rgb_l.rgbtBlue |= (1 << 0);
						else
							rgb_l.rgbtBlue &= ~(1 << 0);
						ii++;

						//вставляем один бит в зеленый слой
						if (str[ii] == 1)
							rgb_l.rgbtGreen |= (1 << 0);
						else
							rgb_l.rgbtGreen &= ~(1 << 0);
						ii++;
						k++;
						
						
					}
					break;
			}
				
			
			fwrite(&rgb_l, sizeof(rgb_l), 1, f2);//запись изображения 
		}


		if (padding != 0) {
			fread(&rgb_l, padding, 1, f1);
			fwrite(&rgb_l, padding, 1, f2);
		}
	}
	delete[] str;
	std::cout << "K=" << k << "ii=" << ii << endl;
	unsigned int end_time = clock(); // конечное время
	unsigned int search_time = end_time - start_time; // искомое время
	fout.close();
	fout.open("decodeLog.txt");
	//Декодирование
	fseek(f2, 0, SEEK_SET);
	BITMAPFILEHEADER bfh_l2;
	BITMAPINFOHEADER bih_l2;
	fread(&bfh_l2, sizeof(bfh_l2), 1, f2);               //Запихиваем файловый заголовок в структуру BITMAPFILEHEADER
	fread(&bih_l2, sizeof(bih_l2), 1, f2);

	RGBTRIPLE rgb_l2;
	int *decodeStr = new int[sizeMsg * 8];
	k = ii = 0;
	for (int i = 0; i < bih_l2.biHeight; i++) {
		for (int j = 0; j < bih_l2.biWidth; j++) {
			
			fread(&rgb_l2, sizeof(rgb_l2), 1, f2);
			sum += pow(abs(b_mas[i][j] - rgb_l2.rgbtBlue), 2.0);
			sum += pow(abs(g_mas[i][j] - rgb_l2.rgbtGreen), 2.0);
			sum += pow(abs(r_mas[i][j] - rgb_l2.rgbtRed), 2.0);

			for (int count = 0; count < countPix; count++)
				if ((i == masRandomValueString[count]) && (j == masRandomValueColumn[count])) {
					fout<<" i:j = " << i <<":"<<j<<"  k="<<k<< endl;

					if ((k - 1)%3 == 0) {//считываем два синих и один зеленый бит из пикселя
										 //считываем два синих бита их пикселя
										 //1 синий бит
						if (rgb_l2.rgbtBlue & (1 << 1))
							decodeStr[ii] = 1;
						else decodeStr[ii] = 0;
						ii++;
						//2 синий бит
						if (rgb_l2.rgbtBlue & (1 << 0))
							decodeStr[ii] = 1;
						else decodeStr[ii] = 0;
						ii++;
						k++;
					}
					else {
						//1 синий бит
						if (rgb_l2.rgbtBlue & (1 << 1))
							decodeStr[ii] = 1;
						else decodeStr[ii] = 0;
						ii++;
						//2 синий бит
						if (rgb_l2.rgbtBlue & (1 << 0))
							decodeStr[ii] = 1;
						else decodeStr[ii] = 0;
						ii++;
						//1 зеленый бит
						if (rgb_l2.rgbtGreen & (1 << 0))
							decodeStr[ii] = 1;
						else decodeStr[ii] = 0;
						ii++;
						k++;	

					}
					
				}
		}
	}
	std::cout << "K=" << k<<"ii="<<ii<<endl;
	std::cout << "new messege"<<endl;
	/*for (int i = 0; i <sizeMsg * 8; i++) //Вывод для проверки
		std::cout << decodeStr[i];
	std::cout << endl;*/
		
	fout.close();
	fout.open("output.txt",std::ofstream::binary);
		int decodeMsg[8];
		/*int*/ z = 0;
		for (int i = 0; i <sizeMsg*8; i++) {

			decodeMsg[z++] = decodeStr[i];
			if ((i + 1) % 8 == 0) {
				std::cout << (char)toDec(decodeMsg);
				fout << (char)toDec(decodeMsg);
				z = 0;
			}

		}

		fclose(f1); fclose(f2);
		
	
		
		/*	//Показатели MSE PSNR
		in.open("lena.bmp", std::ifstream::ate | ios::binary);
		int sizeFirstImg = in.tellg();
		sizeFirstImg -= 54;
		int *valuePix = new int[sizeFirstImg];
		in.seekg(54, std::ios::beg);
		for (int i = 0; i <sizeFirstImg; i++) {
			valuePix[i] = in.get();
		}
		in.close();

		in.open("lena3.bmp", std::ifstream::ate | ios::binary);
		int sizeSecondImg = in.tellg();
		sizeSecondImg -= 54;
		int *valuePix2 = new int[sizeSecondImg];
		in.seekg(54, std::ios::beg);
		for (int i = 0; i <sizeSecondImg; i++) {
			valuePix2[i] = in.get();
		}
		in.close();
		double MSE;
		int sum = 0;
		int j = 0;
		for (int i = 0; i < sizeFirstImg / 3; i++) {
			j = i * 3;
			sum += pow(abs(valuePix[j] - valuePix2[j]), 2.0);
			j++;
			sum += pow(abs(valuePix[j] - valuePix2[j]), 2.0);
			j++;
			sum += pow(abs(valuePix[j] - valuePix2[j]), 2.0);

		}*/
		std::cout << "\nSUMM  " << sum << endl;
		double MSE = (double)sum / 3/(double)(bih_l.biHeight*bih_l.biWidth);
		
		double PSNR = 10 * log10(pow(255, 2.0) / MSE);
		std::cout << "\nMSE " << MSE << endl;
		std::cout << "\nPSNR " << PSNR << endl;
		std::cout << "time= " << search_time / 1000 << endl;
		


	return 0;
}

string toBinary(char c)
{
	static char mask[] = { 1, 2, 4, 8, 16, 32, 64, 128 };
	string result;
	int i = 8;
	while (i--)
	{
		result += (c & mask[i]) ? '1' : '0';
	}
	return result;
}
/*int * randomGenerator(int srandValue, int count, int length) {

	int *mas = new int[count];
	int k = 0;
	srand(srandValue);
	for (int i = 0; i < count; i++) {
		int value = rand() % length;
		for (int j = 0; j < i; j++)
			if (mas[j] == value) k++;
		if (k == 0) {
			mas[i] = value; k = 0;
			//std::cout << mas[i] << "; ";
		}
		else {
			k = 0; i--;
		}
	}

	return mas;
}*/
void randomGenerator(int srandValue, int count, int length, int *mas1,int *mas2) {

//	int *mas = new int[count];
	
	int k = 0;
	srand(srandValue);
	for (int i = 0; i < count; i++) {
		int value1 = rand() % length;
		int value2 = rand() % length;
		for (int j = 0; j < i; j++)
			if (mas1[j] == value1 && mas2[j]==value2) k++;
		if (k == 0) {
			mas1[i] = value1; 
			mas2[i] = value2;
			k = 0;
			//std::cout << mas[i] << "; ";
		}
		else {
			k = 0; i--;
		}
	}

}
int toDec(int *mas) {
	int value = 0;
	for (int i = 7; i >= 0; i--) {
		value += mas[7 - i] * pow(2, i);
	}

	return value;
}