#include "KnobEvent.h"
#include "KnobMenu.h"
#include "KeyDef.h"
#include "KnobProjectMode.h"
#include "ViewSuperuser.h"
void CKnobEvent::KnobEvent(unsigned char keyValue, unsigned char offset)
{
	KnobMenu* ptrKnob = KnobMenu::GetInstance();

	switch (keyValue)
	{
		case KNOB_F1: //F1
			ptrKnob->Knob1_Screw(offset);
			break;
		case KNOB_F2: //F2
			ptrKnob->Knob2_Screw(offset);
			break;
		case KNOB_F3: //F3
			ptrKnob->Knob3_Screw(offset);
			break;
		case KNOB_F4: //F4
			ptrKnob->Knob4_Screw(offset);
			break;
		case KNOB_F5: // F5
			ptrKnob->Knob5_Screw(offset);
			break;
		default:
			break;
	}
}

void CKnobEvent::FEvent(unsigned char keyValue)
{

	KnobMenu* ptrKnob = KnobMenu::GetInstance();

#ifdef EMP_PROJECT
	KnobProjectMode* ptrKnobProjectMode = KnobProjectMode::GetInstance();

#if (defined(EMP_460) || defined(EMP_355))
	switch (keyValue)
	{
		case KEY_F1ADD: //F1
                    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&ptrKnobProjectMode->GetKnobStatus())
                        ptrKnobProjectMode->Knob1_Screw(1);
                    else
			ptrKnob->Knob1_Screw(1);
			break;
		case KEY_F1SUB: //F1
                    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&ptrKnobProjectMode->GetKnobStatus())
                        ptrKnobProjectMode->Knob1_Screw(0);
                    else
			ptrKnob->Knob1_Screw(0);
			break;
		case KEY_F2ADD: //F2
                    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&ptrKnobProjectMode->GetKnobStatus())
                        ptrKnobProjectMode->Knob2_Screw(1);
                    else
			ptrKnob->Knob2_Screw(1);
			break;
		case KEY_F2SUB: //F2
                    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&ptrKnobProjectMode->GetKnobStatus())
                        ptrKnobProjectMode->Knob2_Screw(0);
                    else
			ptrKnob->Knob2_Screw(0);
			break;
		case KEY_F3ADD: //F2
                    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&ptrKnobProjectMode->GetKnobStatus())
                        ptrKnobProjectMode->Knob3_Screw(1);
                    else
			ptrKnob->Knob3_Screw(1);
			break;
		case KEY_F3SUB: //F2
                    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&ptrKnobProjectMode->GetKnobStatus())
                        ptrKnobProjectMode->Knob3_Screw(0);
                    else
			ptrKnob->Knob3_Screw(0);
			break;
		case KEY_F4ADD: //F2
                    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&ptrKnobProjectMode->GetKnobStatus())
                        ptrKnobProjectMode->Knob4_Screw(1);
                    else
			ptrKnob->Knob4_Screw(1);
			break;
		case KEY_F4SUB: //F2
                    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&ptrKnobProjectMode->GetKnobStatus())
                        ptrKnobProjectMode->Knob4_Screw(0);
                    else
			ptrKnob->Knob4_Screw(0);
			break;
		case KEY_F5ADD: //F2
                    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&ptrKnobProjectMode->GetKnobStatus())
                        ptrKnobProjectMode->Knob5_Screw(1);
                    else
			ptrKnob->Knob5_Screw(1);
			break;
		case KEY_F5SUB: //F2
                    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&ptrKnobProjectMode->GetKnobStatus())
                        ptrKnobProjectMode->Knob5_Screw(0);
                    else
			ptrKnob->Knob5_Screw(0);
			break;
	}
#endif
#else
#if (defined(EMP_460) || defined(EMP_355))
		switch (keyValue)
		{
			case KEY_F1ADD: //F1
				ptrKnob->Knob1_Screw(1);
				break;
			case KEY_F1SUB: //F1
				ptrKnob->Knob1_Screw(0);
				break;
			case KEY_F2ADD: //F2
				ptrKnob->Knob2_Screw(1);
				break;
			case KEY_F2SUB: //F2
				ptrKnob->Knob2_Screw(0);
				break;
			case KEY_F3ADD: //F2
				ptrKnob->Knob3_Screw(1);
				break;
			case KEY_F3SUB: //F2
				ptrKnob->Knob3_Screw(0);
				break;
			case KEY_F4ADD: //F2
				ptrKnob->Knob4_Screw(1);
					break;
			case KEY_F4SUB: //F2
				ptrKnob->Knob4_Screw(0);
				break;
			case KEY_F5ADD: //F2
				ptrKnob->Knob5_Screw(1);
				break;
			case KEY_F5SUB: //F2
				ptrKnob->Knob5_Screw(0);
				break;
		}
#endif
#endif
}