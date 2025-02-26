#include "GameTime.h"

GameTime::GameTime() : mSencondsPerCount(0.0), mDeltaTime(-1.0), mBaseTime(0),
mPauseTime(0), mStopTime(0), mPrevTime(0), mCurrentTime(0), isStoped(false)
{
	//���������ÿ����ٴΣ�������countsPerSec�з���
	//ע�⣬�˴�ΪQueryPerformanceFrequency����
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSencondsPerCount = 1.0 / (double)countsPerSec;
}
void GameTime::Tick()
{
	if (isStoped)
	{
		//�����ǰ��ֹͣ״̬����֡���ʱ��Ϊ0
		mDeltaTime = 0.0;
		return;
	}
	//���㵱ǰʱ�̵ļ���ֵ
	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	mCurrentTime = currentTime;
	//���㵱ǰ֡��ǰһ֡��ʱ���(������*ÿ�ζ�����)
	mDeltaTime = (mCurrentTime - mPrevTime) * mSencondsPerCount;
	//׼�����㵱ǰ֡����һ֡��ʱ���
	mPrevTime = mCurrentTime;
	//�ų�ʱ���Ϊ��ֵ
	if (mDeltaTime < 0)
	{
		mDeltaTime = 0;
	}
}
float GameTime::DeltaTime()const
{
	return (float)mDeltaTime;
}
void GameTime::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mBaseTime = currTime;	//��ǰʱ����Ϊ��׼ʱ��
	mPrevTime = currTime;	//��ǰʱ����Ϊ��һ֡ʱ�䣬��Ϊ�����ˣ���ʱû����һ֡
	mStopTime = 0;			//����ֹͣ��һ��ʱ��Ϊ0
	isStoped = false;		//���ú��״̬Ϊ��ֹͣ
}
void GameTime::Stop()
{
	if (!isStoped)//���û��ֹͣ��������ֹͣ�����ֹͣ��ʲô��������
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		mStopTime = currTime;	//����ǰʱ����Ϊֹͣ��һ�̵�ʱ��(����)
		isStoped = true;	//�޸�Ϊֹͣ״̬
	}
}
void GameTime::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
	if (isStoped)//�����ֹͣ״̬����������ֹͣ��������ͣʱ�䣬�޸�ֹͣ״̬
	{
		//�������ͣ����ʱ�䣨����ֹͣʱ�䣩
		mPauseTime += (startTime - mStopTime);
		//�޸�ֹͣ״̬
		mPrevTime = startTime;//�൱��������һ֡ʱ��
		mStopTime = 0;	//�൱������ֹͣ��ʱ��
		isStoped = false;	//ֹͣ״̬Ϊ��
	}
	//�������ֹͣ״̬����ʲô������
}
float GameTime::TotalTime()const
{
	if (isStoped)	//�����ʱ����ͣ״̬������ֹͣʱ�̵�ʱ��ȥ��֮ǰ��ͣ����ʱ��
	{
		return (float)((mStopTime - mPauseTime - mBaseTime) * mSencondsPerCount);
	}
	else
	{
		//���������ͣ״̬�����õ�ǰʱ�̵�ʱ��ȥ����ͣ��ʱ��
		return (float)((mCurrentTime - mPauseTime - mBaseTime) * mSencondsPerCount);
	}
}
bool GameTime::IsStoped()
{
	return isStoped;
}
