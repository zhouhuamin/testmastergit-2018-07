#include "signature.h"
#include <stdio.h>

void RecvRecoResult(int len, char *pBuffer)
{
	FILE *pFile = NULL;
	string strFileName = "signaturenm.data";
	if ((pFile = fopen(strFileName.c_str(), "a+b")) == NULL)
	{
		return;
	}
	fwrite((char*)pBuffer, 1, len, pFile);
	fclose(pFile);
}

int main(int argc, char* argv[])
{
	int a = 0;
	int b = 0;
	int c = 0;
	int d = 0;
	int e = 0;
	int f = 0;
	Func1(a, b, c, RecvRecoResult);
	Func2(a, b, c, d, e, f, RecvRecoResult);
}
