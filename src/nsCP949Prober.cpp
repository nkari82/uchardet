#include "nsCP949Prober.h"

void  nsCP949Prober::Reset(void)
{
	mCodingSM->Reset();
	mState = eDetecting;
	mDistributionAnalyser.Reset(mIsPreferredLanguage);
	//mContextAnalyser.Reset();
}

nsProbingState nsCP949Prober::HandleData(const char* aBuf, PRUint32 aLen)
{
	PRUint32 codingState;

	for (PRUint32 i = 0; i < aLen; i++)
	{
		codingState = mCodingSM->NextState(aBuf[i]);
		if (codingState == eItsMe)
		{
			mState = eFoundIt;
			break;
		}
		if (codingState == eStart)
		{
			PRUint32 charLen = mCodingSM->GetCurrentCharLen();

			if (i == 0)
			{
				mLastChar[1] = aBuf[0];
				mDistributionAnalyser.HandleOneChar(mLastChar, charLen);
			}
			else
				mDistributionAnalyser.HandleOneChar(aBuf + i - 1, charLen);
		}
	}

	mLastChar[0] = aBuf[aLen - 1];

	if (mState == eDetecting)
		if (mDistributionAnalyser.GotEnoughData() && GetConfidence() > SHORTCUT_THRESHOLD)
			mState = eFoundIt;
	//    else
	//      mDistributionAnalyser.HandleData(aBuf, aLen);

	return mState;
}

float nsCP949Prober::GetConfidence(void)
{
	float distribCf = mDistributionAnalyser.GetConfidence();

	return (float)distribCf;
}