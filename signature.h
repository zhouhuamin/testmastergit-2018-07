#ifndef SIGNATURE_H
#define SIGNATURE_H

struct ParameterInfo1
{
	int a;
	int b;
	int c;
};

struct ParameterInfo2
{
	int a;
	int b;
	int c;
	int d;
	int e;
	int f;
};

typedef void (*SIGNATURE_CALLBACK)(int len, char *pBuffer);
extern "C" int Func1(int a, int b, int c, SIGNATURE_CALLBACK pCallback);
extern "C" int Func2(int a, int b, int c, int d, int e, int f, SIGNATURE_CALLBACK pCallback);

#endif
