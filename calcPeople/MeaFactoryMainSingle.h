#ifndef MEA_FACTORY_MAIN_SINGLE_H
#define MEA_FACTORY_MAIN_SINGLE_H

#include "MeasureDef.h"
#include "AbsMeasure.h"
#include "Measure2D.h"
#include "MeasureFactory.h"

class MeaFactoryMainSingle
{
	public:
		~MeaFactoryMainSingle();
		static MeaFactoryMainSingle* GetInstance();

		AbsMeasure* Create(int type);

		int GetType();

	private:
		MeaFactoryMainSingle();
		
		static MeaFactoryMainSingle* m_ptrInstance;
		int m_type;
		unsigned int NewSingleMeasure(const SingleItemInfo *ptrSingleItemInfo, MeasureFactory::EMeasureMode currMeaMode);
		unsigned int NewMultiMeasure(const MultiItemInfo *ptrMultiItemInfo, MeasureFactory::EMeasureMode currMeaMode);
		int SectionItem(int item, int *itemStart, const SingleItemInfo **ptrSingleItemInfo, const MultiItemInfo **ptrMultiItemInfo);
};

#endif
