#include "display/FakeXEvent.h"
#include "display/gui_func.h"
#include "keyboard/KeyFunc.h"
#include "keyboard/KeyDef.h"

void FakeXEvent::KeyEvent(unsigned char keyValue)
{
#if defined(CT_322)
    if(keyValue == KEY_CTRL_SHIFT_SPACE)
    {
        KeySwitchIM ksim;
        ksim.ExcuteChange(TRUE);
        return;
    }
#elif defined(CT_313)
    {
        if (keyValue == KEY_ONE)
        {
            KeySwitchIM ksim;
            ksim.ExcuteChange(TRUE);
            return;
        }
    }
#else
    if(keyValue==KEY_SHIFT_CTRL)
    {
        KeySwitchIM ksim;
        ksim.ExcuteChange(TRUE);
        return;
    }
#endif
    if(FakeMouseButton(keyValue))
        return;
    if(FakeNumKey(keyValue))
        return;
    if(FakeAlphabet(keyValue))
        return;
    if(FakePunctuation(keyValue))
        return;
}

void FakeXEvent::MouseEvent(char offsetX, char offsetY)
{
    fakeXMotionEventFullRange(offsetX, offsetY);
}