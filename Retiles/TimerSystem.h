#pragma once
class TimerSystem
{
private:
	float tick;
	float tickStart;
	bool isActive;

public:
	TimerSystem();
	TimerSystem(float);
	void Tick(float);
	bool TimerEnd();
	void TimerReset();
	void TimerReset(float);

	bool GetActive() { return isActive; }
	void SetActive(bool f) { isActive = f; }
};

