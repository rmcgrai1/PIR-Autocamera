/* MATH METHODS, math2.h
 * Max Kennard, Ryan McGrail, Shaquille Johnson
 * 1/26/16
 *
 * Provides several arithmetic functions for completing basic operations.
 */

#ifndef MATH2_H
#define MATH2_H

#include <math.h>

/*****************************  PROTOTYPES  *****************************/

float sign(float);
float smoothTo(float, float, float);
template<typename T> T mapt(T, T, T, T, T);
float mapf(float, float, float, float, float);
float calcWeightedSum(float, float, float, float, float, float);

template<typename T> T maxt(T[], int, boolean);
template<typename T> T mint(T[], int, boolean);
float maxa(float[], int);
long maxa(long[], int);
float mina(float[], int);
long mina(long[], int);
int maxi(float[], int);
int maxi(long[], int);
int mini(float[], int);
int mini(long[], int);

void convertShort2Bytes(int, byte[], int);
void convertShort2Bytes(int, byte[], int, int);

float calcAngleDiff(float, float);
float calcLenX(float, float);
float calcLenY(float, float);

/*****************************  MISC. ARITHMETIC  *****************************/

float sign(float a) {
	return (a < 0) ? -1 : (a > 0) ? 1 : 0;
}

float smoothTo(float start, float endf, float steps) {
	return (endf - start) / steps;
}

template<typename T>
T mapt(T x, T in_min, T in_max, T out_min, T out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
	return mapt(x, in_min, in_max, out_min, out_max);
}

float calcWeightedSum(float weight1, float val1, float weight2, float val2,
		float weight3, float val3) {
	float sum = weight1 + weight2 + weight3;
	if (sum == 0)
		return 0;

	return (weight1 * val1 + weight2 * val2 + weight3 * val3) / sum;
}

/******************** MIN/MAX FUNCTIONS **********************/

template<typename T>
T maxt(T array[], int size, boolean returnIndex) {
	T cur, ma = array[0];
	int maxInd = 0;

	for (int i = 1; i < size; i++) {
		if ((cur = array[i]) > ma) {
			ma = cur;
			maxInd = i;
		}
	}

	return returnIndex ? maxInd : ma;
}

template<typename T>
T mint(T array[], int size, boolean returnIndex) {
	T cur, mi = array[0];
	int minInd = 0;

	for (int i = 1; i < size; i++) {
		if ((cur = array[i]) > mi) {
			mi = cur;
			minInd = i;
		}
	}

	return returnIndex ? minInd : mi;
}

// Get Max Value in Array
float maxa(float array[], int size) {
	return maxt(array, size, false);
}
long maxa(long array[], int size) {
	return maxt(array, size, false);
}

// Get Min Value in Array
float mina(float array[], int size) {
	return mint(array, size, false);
}
long mina(long array[], int size) {
	return mint(array, size, false);
}

// Get Index of Max Value in Array
int maxi(float array[], int size) {
	return maxt(array, size, true);
}
int maxi(long array[], int size) {
	return maxt(array, size, true);
}

// Get Index of Min Value in Array
int mini(float array[], int size) {
	return mint(array, size, true);
}
int mini(long array[], int size) {
	return mint(array, size, true);
}

/*****************************  BIT SHIFTING  *****************************/

void convertShort2Bytes(int shrt, byte array[], int index) {
	// Shift 4 sets of 4 bits from 2 byte int into 4 different bytes,
	// and then writes those into a given array.
	array[index + 0] = (shrt >> 12) & 0xF;
	array[index + 1] = (shrt >> 8) & 0xF;
	array[index + 2] = (shrt >> 4) & 0xF;
	array[index + 3] = (shrt) & 0xF;
}

void convertShort2Bytes(int shrt, byte array[], int index1, int index2) {
	// Shift 4 sets of 4 bits from 2 byte int into 4 different bytes,
	// and then writes those into a given array in 2 spots.
	array[index1 + 0] = array[index2 + 0] = (shrt >> 12) & 0xF;
	array[index1 + 1] = array[index2 + 1] = (shrt >> 8) & 0xF;
	array[index1 + 2] = array[index2 + 2] = (shrt >> 4) & 0xF;
	array[index1 + 3] = array[index2 + 3] = (shrt) & 0xF;
}

void convertFraction2Bytes(float frac, int minNum, int maxNum, byte array[],
		int index) {
	int num = (int) (minNum + frac * (maxNum - minNum)), units = num % 16,
			tens = (num - units) / 16;

	array[index + 0] = 0x0 + tens;
	array[index + 1] = 0x0 + units;
}

/*****************************  ANGLE/TRIG FUNCTIONS  *****************************/

// Calculate Smallest Difference between 2 Angles
float calcAngleDiff(float a, float b) {
	a -= b;
	return fmod(a + 180, 360) - 180;
}

// Calculate X/Y Component of Angle Given Radius
float calcLenX(float r, float deg) {
	return r * cos(deg / 180 * PI);
}
float calcLenY(float r, float deg) {
	return r * sin(deg / 180 * PI);
}

#endif
