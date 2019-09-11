#include<stdio.h>
#include<math.h>

int g_SampleRate = 44100;
int g_channels = 2;
#define FMT s16le

#define DEF_MAX 32767
#define DEF_MIN -32768


//
void pcm_mix_auto_newlc(short *bufMix,short *buf,short *buf2,int T)
{
	for (int i = 0; i < T; i++)
	{
		int iT = 0;
		
		if (buf[i] < 0 && buf2[i] < 0)
		{
			iT = buf[i] * buf2[i] / -(pow(2, 16 - 1) - 1);
		}
		else
		{
			iT = buf[i] * buf2[i] / (pow(2, 16 - 1) - 1);
		}

		int iMax = buf[i] + buf2[i] - iT;
		if (iMax > DEF_MAX)
		{
			printf("pcm_mix_auto_newlc DEF_MAX\n");
		}
		if (iMax < DEF_MIN)
		{
			printf("pcm_mix_auto_newlc DEF_MIN\n");
		}

		bufMix[i] = iMax;
	}
}

//�Զ������㷨,������ı���ΪȨ��.
//a��Ȩ��Ϊa/(a+b+c),b��Ȩ��Ϊb/(a+b+c)��������a��Ȩ�س���a��ֵ������b��Ȩ�س���b��ֵ.
//(a*|a| + |b|*b + |c|*c)/(|a|+|b|+|c|)�÷������¸�Ƶ������ʧ
int get_mix_auto_align_sgn(short p)
{
	int isgn = 0;

	if (p > 0)
	{
		isgn = 1;
	}
	else if (p < 0)
	{
		isgn = -1;
	}
	else {
		isgn = 0;
	}

	int ibuf = abs(p * p);

	return isgn * ibuf;
}
void pcm_mix_auto_align(short *bufMix,short *buf,short *buf2,int T)
{
	
	for (int i = 0; i < T; i++)
	{
		//int ibuf = get_mix_auto_align_sgn(buf[i]);
		//int ibuf2 = get_mix_auto_align_sgn(buf2[i]);
		
		if (buf[i]==0 && buf2[i] == 0)
		{
			bufMix[i] = 0;
			continue;
		}

		int iT = (abs(buf[i])*buf[i] +abs(buf2[i])*buf2[i]) / (abs(buf[i]) + abs(buf2[i]));
		
		if (iT > DEF_MAX)
		{
			printf("pcm_mix_auto_align DEF_MAX\n");
		}
		if (iT < DEF_MIN)
		{
			printf("pcm_mix_auto_align DEF_MIN\n");
		}

		bufMix[i] = iT;
	}
	
}

//���Ե�����ƽ��.������������������С������˥������Ӱ��ͨ��������
void pcm_mix_avg(short *bufMix,short *buf,short *buf2 ,int T)
{
	for (int i = 0; i < T; i++)
	{
		int iT = buf[i] + buf2[i];

		bufMix[i] = (short)(iT / 2);
		
		if (bufMix[i] > DEF_MAX)
		{
			bufMix[i] = DEF_MAX;
			printf("pcm_mix_avg DEF_MAX\n");
		}
		if (bufMix[i] < DEF_MIN)
		{
			bufMix[i] = DEF_MIN;
			printf("pcm_mix_avg DEF_MIN\n");
		}
	}
}
//����Ӧ������Ȩ,ʹ�ÿɱ��˥�����Ӷ���������˥������˥�����Ӵ�����������Ȩ�أ�
//˥������������Ƶ���ݵı仯���仯�������ʱ��˥�����ӱ�С��ʹ�ú�����������˥�������ٽ�ֵ����,
//û�����ʱ������˥�������������������ݽ�Ϊƽ���ı仯.
void pcm_mix_Normalization(short *bufMix, short *buf, short *buf2, int T)
{
	double f = 1;
	int ioutput = 0;

	for (int i = 0; i < T; i++)
	{

		int iT = buf[i] + buf2[i];
		
		ioutput = iT * f;

		if (ioutput > DEF_MAX)
		{			
			f = (double)DEF_MAX / (double)(ioutput);
			ioutput = DEF_MAX;

			printf("pcm_mix_Normalization DEF_MAX f=%f\n",f);
		}

		if (ioutput < DEF_MIN)
		{
			f = (double)DEF_MIN / (double)(ioutput);
			ioutput = DEF_MIN;

			printf("pcm_mix_Normalization DEF_MIN f=%f\n", f);
		}

		if (f < 1)
		{
			f += (double)(1 - f) / (double)32;
		}
		bufMix[i] = (short)ioutput;
	}
}
int main(int argc, char *argv[])
{
	FILE *pPcm1 = nullptr, *pPcm2 = nullptr,*pPcmMix = nullptr;

	errno_t er = fopen_s(&pPcm1,"../Pcm/1.pcm","rb+");
	if (er != 0)
	{
		printf("");
	}
	er = fopen_s(&pPcm2,"../Pcm/2.pcm","rb+");
	if (er != 0)
	{
		printf("");
	}
	//er = fopen_s();
	//{

	//}

	er = fopen_s(&pPcmMix, "Mix.pcm", "wb+");
	if (er != 0)
	{
		printf("");
	}


	short buf[1024] = {0};
	short buf2[1024] = {0};
	short buf3[1024] = {0};

	int ir = fread(buf, 2, 1024, pPcm1);
	int ir2 = fread(buf2, 2, 1024, pPcm2);

	while (ir > 0 && ir2 > 0)
	{
		
		//pcm_mix_avg(buf3, buf, buf2, 1024);
		//pcm_mix_Normalization(buf3,buf,buf2,1024);
		pcm_mix_auto_align(buf3,buf,buf2,1024);
		//pcm_mix_auto_newlc(buf3, buf, buf2, 1024);

		fwrite(buf3, 2, 1024, pPcmMix);

		ir = fread(buf, 2, 1024, pPcm1);
		ir2 = fread(buf2, 2, 1024, pPcm2);
	}

	fclose(pPcm1);
	fclose(pPcm2);
	fclose(pPcmMix);
	
	printf("end.\n");

	for (;;);

	return 0;
}