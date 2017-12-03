#ifndef USER_DEFINE_KEY_H_ 
#define USER_DEFINE_KEY_H_

#include <string>
using std::string;

/*
 * @brief manage user define key
 */
class UserDefineKey 
{
	public:
		UserDefineKey();
		~UserDefineKey();
#if defined(EMP_322)
		static const int MAX_KEY = 6;//6
#else
		static const int MAX_KEY = 10;//8;//9;
#endif
		static const string KeyFunctionList[MAX_KEY];
#if 0
        void PressKeyGain(void);
		void PressKeyValue(void);
#endif
		void PressKeyP1(void);
		void PressKeyP2(void);
		void PressKeyP3(void);

	private:
#if defined(EMP_322)
        //enum EKeyFunc {NONE, THI, TSI, SAVE_CINE, PRINT, PIP};
        enum EKeyFunc {NONE, TSI, SAVE_CINE, PRINT, PIP, BIOPSY};
#else
		enum EKeyFunc {NONE, TSI, SAVE_CINE, PRINT, STEER, PIP, PW_SOUND, HPRF, IMAGE_PRESET, BIOPSY};
		//enum EKeyFunc {NONE, THI, TSI, SAVE_CINE, PRINT, STEER, PIP, PW_SOUND, HPRF, BIOPSY};
#endif
		void FactoryCreate(EKeyFunc type);
};
#endif
