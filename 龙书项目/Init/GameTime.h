#pragma once
#include <Windows.h>

class GameTime
{
public:
	GameTime();

	float TotalTime()const;	//��Ϸ���е���ʱ�䣨��������ͣ��
	float DeltaTime()const;	//��ȡmDeltaTime����
	bool IsStoped();	//��ȡisStoped����

	void Reset();	//���ü�ʱ��
	void Start();	//��ʼ��ʱ��
	void Stop();	//ֹͣ��ʱ��
	void Tick();	//����ÿ֡ʱ����

private:
	double mSencondsPerCount;	//������ÿһ����Ҫ������
	double mDeltaTime;			//ÿ֡ʱ�䣨ǰһ֡�͵�ǰ֡��ʱ��

	__int64 mBaseTime;		//���ú�Ļ�׼ʱ��
	__int64 mPauseTime;		//��ͣ����ʱ��
	__int64 mStopTime;		//ֹͣ��һ�̵�ʱ��
	__int64 mPrevTime;		//��һ֡ʱ��
	__int64 mCurrentTime;	//��֡ʱ��

	bool isStoped;		//�Ƿ�Ϊֹͣ��״̬
};