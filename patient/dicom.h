//DICOM INTERFACE DEVELOPED BY FUYONG
#include <memory.h>
#ifndef _DICOM_H
#define	_DICOM_H

#ifdef DICOM_EXPORT
#define DICOM_DECL_SPEC __declspec(dllexport)
#elif DICOM_IMPORT
#define DICOM_DECL_SPEC __declspec(dllimport)
#else
#define DICOM_DECL_SPEC
#endif

/*
*	ERROR DEFINED
*/
#ifndef BYTE
#define BYTE unsigned char
#endif
#ifndef WORD
#define WORD unsigned short
#endif
#ifndef DWORD
#define DWORD unsigned long
#endif
#ifndef BOOL
#define BOOL bool
#endif
#ifndef USHORT
#define USHORT unsigned short
#endif
#ifndef UINT
#define UINT unsigned int
#endif
#ifndef ULONG
#define ULONG unsigned long
#endif
#ifndef UCHAR
#define UCHAR unsigned char
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE  1
#endif
#define	DICOM_ERROR_SUCCESS						0
#define	DICOM_ERROR_UNEXPECTERROR				1
#define	DICOM_ERROR_DATAINPUTERROR				2
#define	DICOM_ERROR_OUTOFMEMORY					3

#define	DICOM_ERROR_ASSOCTYPEERROR				1000
#define	DICOM_ERROR_DIMSETYPEERROR				1001
#define	DICOM_ERROR_ELEMLISTTYPEERROR			1002
#define	DICOM_ERROR_ELEMTYPEERROR				1003
#define	DICOM_ERROR_SOCKETTYPEERROR				1004
#define	DICOM_ERROR_FEMETAERROR					1005

#define	DICOM_ERROR_UNABLEPUSHELEMLIST			1050
#define	DICOM_ERROR_COMMANDSETWRITEERROR			1051
#define	DICOM_ERROR_NODATASET					1052
#define	DICOM_ERROR_OBJNOTCREATED				1053

#define	DICOM_ERROR_UIDERROR						1100
#define	DICOM_ERROR_TRANSYNTAXERROR				1101
#define	DICOM_ERROR_ELEMLISTTORAWVRERROR			1102
#define	DICOM_ERROR_RAWVRTOELEMLISTERROR			1103
#define DICOM_ERROR_NO_ACCEPTED_PRESENTATION		1104
#define	DICOM_ERROR_NODCMFILE					1105
#define	DICOM_ERROR_DCMDICTNOTLOADED				1106
#define	DICOM_ERROR_VRNOTFOUNDEDINDICT			1107
#define	DICOM_ERROR_VRNOTMATCHVALUE				1109
#define	DICOM_ERROR_VMNOTMATCH					1110
#define	DICOM_ERROR_ELEMPUSHFAILD				1111
#define	DICOM_ERROR_ELEMSQPUSHFAILD				1112
#define	DICOM_ERROR_NOELEMIN						1113
#define	DICOM_ERROR_NOSQELEM						1114
#define	DICOM_ERROR_SOPERROR						1115
#define DICOM_ERROR_NOFREEDISKSPACE				1116
#define DICOM_ERROR_CANNOTOPENFILE				1117
#define DICOM_ERROR_CANNOTDECODEFILE            1118

#define	DICOM_ERROR_DIMSE_NOAFFECSOPCLASUID		2000
#define	DICOM_ERROR_DIMSE_NOAFFECSOPINSTUID		2001
#define	DICOM_ERROR_DIMSE_NOMOVEDSTINATION		2002
#define	DICOM_ERROR_DIMSE_NOREQUESOPCLASUID		2003
#define	DICOM_ERROR_DIMSE_NOREQUESOPINSTUID		2004
#define	DICOM_ERROR_DIMSE_NOPRIORITY				2005
#define	DICOM_ERROR_DIMSE_NOATTRIBIDLIST			2006

#define	DICOM_ERRORT_ELEMNOTINLIST				2500

#define DICOM_ERROR_NOCALLEDAETITLE				2600
#define DICOM_ERROR_NOCALLINGAETITLE				2601
#define DICOM_ERROR_NOABSSYNTAXADDED				2602

#define DICOM_ERROR_CANNOTREADASSOCRQ			2700
#define DICOM_ERROR_CANNOTWRITEASSOCAC			2701
#define DICOM_ERROR_CANNOTWRITEASSOCRJ			2702
#define DICOM_ERROR_CANNOTWRITEASSOCAB			2703
#define DICOM_ERROR_CANNOTWRITEDICOMDATASET		2704
#define DICOM_ERROR_CANNOTREADASSOCAC			2705
#define DICOM_ERROR_CANNOTREADASSOCRJ			2706
#define DICOM_ERROR_CANNOTREADRELESRQ			2707
#define DICOM_ERROR_CANNOTREADRELESRP			2708
#define DICOM_ERROR_CANNOTREADABORTRQ			2709
#define DICOM_ERROR_CANNOTREADPDATATF			2710

#define DICOM_ERROR_TRANSYNTAXNOTSUPPORTED		2800

#define DICOM_ERROR_UNEXPECTED_AASSOCIATERQ_READ	2900
#define DICOM_ERROR_UNEXPECTED_AASSOCIATEAC_READ	2901
#define DICOM_ERROR_UNEXPECTED_AASSOCIATERJ_READ	2902
#define DICOM_ERROR_UNEXPECTED_ARELEASERQ_READ	2903
#define DICOM_ERROR_UNEXPECTED_ARELEASERP_READ	2904
#define	DICOM_ERROR_UNEXPECTED_AABORTRQ_READ		2905
#define	DICOM_ERROR_UNEXPECTED_NORECOGNIZED_READ	2906
#define	DICOM_ERROR_UNEXPECTED_NODATA_READ		2907

//zjy added
#define DICOM_ERROR_SOCKET_LINK_ERROR												3000
#define DICOM_ERROR_PDU_CONNECT_CORRUPT												3001
#define DICOM_ERROR_PDU_CONNECT_NOINTEROGATEAASSOCIATEAC								3002
#define DICOM_ERROR_PDU_CONNECT_REJECTPERMANENT_NOREASONGIVEN						3003
#define DICOM_ERROR_PDU_CONNECT_REJECTPERMANENT_LOCALLIMITEXCEED						3004
#define DICOM_ERROR_PDU_CONNECT_REJECTPERMANENT_TEMPORARYCONGESTION					3005
#define DICOM_ERROR_PDU_CONNECT_REJECTPERMANENT_CALLEDAETITLENOTRECOGNIZED			3006
#define DICOM_ERROR_PDU_CONNECT_REJECTPERMANENT_CALLINGAETITLENOTRECOGNIZED			3007
#define DICOM_ERROR_PDU_CONNECT_REJECTPERMANENT_PROTOCOLVERSIONNOTSUPPORTED			3008
#define DICOM_ERROR_PDU_CONNECT_REJECTPERMANENT_APPLICATIONCONTEXNAMENOTSUPPORTED	3009
#define DICOM_ERROR_PDU_CONNECT_REJECTTRANSIENT_NOREASONGIVEN						3010
#define DICOM_ERROR_PDU_CONNECT_REJECTTRANSIENT_APPLICATIONCONTEXNAMENOTSUPPORTED	3011
#define DICOM_ERROR_PDU_CONNECT_REJECTTRANSIENT_CALLINGAETITLENOTRECOGNIZED			3012
#define DICOM_ERROR_PDU_CONNECT_REJECTTRANSIENT_CALLEDAETITLENOTRECOGNIZED			3013
#define DICOM_ERROR_PDU_CONNECT_REJECTTRANSIENT_PROTOCOLVERSIONNOTSUPPORTED			3014
#define DICOM_ERROR_PDU_CONNECT_REJECTTRANSIENT_TEMPORARYCONGESTION					3015
#define DICOM_ERROR_PDU_CONNECT_REJECTTRANSIENT_LOCALLIMITEXCEED						3016
#define DICOM_ERROR_RELEASE_WRITE_ERROR												3017
#define DICOM_ERROR_ABORT_WRITE_ERROR												3018

#define DICOM_ERROR_JPGERROR															4000

#define	DICOM_ERROR_STR_SUCCESS						"Success"
#define	DICOM_ERROR_STR_UNEXPECTERROR				"Unexpected error"
#define	DICOM_ERROR_STR_DATAINPUTERROR				"Data input error"
#define	DICOM_ERROR_STR_OUTOFMEMORY					"Out of memory"

#define	DICOM_ERROR_STR_ASSOCTYPEERROR				"Association type error"
#define	DICOM_ERROR_STR_DIMSETYPEERROR				"DIMSE type error"
#define	DICOM_ERROR_STR_ELEMLISTTYPEERROR			"Elemlist type error"
#define	DICOM_ERROR_STR_ELEMTYPEERROR				"Elem type error"
#define	DICOM_ERROR_STR_SOCKETTYPEERROR				"Socket type error"

#define	DICOM_ERROR_STR_UNABLEPUSHELEMLIST			"Unable push elemlist"
#define	DICOM_ERROR_STR_COMMANDSETWRITEERROR			"Command set write error"
#define	DICOM_ERROR_STR_NODATASET					"No data set"
#define	DICOM_ERROR_STR_OBJNOTCREATED				"Object not created"

#define	DICOM_ERROR_STR_UIDERROR						"UID error"
#define	DICOM_ERROR_STR_TRANSYNTAXERROR				"Transfer syntax error"
#define	DICOM_ERROR_STR_ELEMLISTTORAWVRERROR			"Elemlist to raw error"
#define	DICOM_ERROR_STR_RAWVRTOELEMLISTERROR			"Raw to elemlist error"
#define DICOM_ERROR_STR_NO_ACCEPTED_PRESENTATION		"No accepted presentation"
#define	DICOM_ERROR_STR_NODCMFILE					"No DICOM file"
#define	DICOM_ERROR_STR_DCMDICTNOTLOADED				"DICOM dictionary not loaded"
#define	DICOM_ERROR_STR_VRNOTFOUNDEDINDICT			"VR not found in Dictionary"
#define	DICOM_ERROR_STR_VRNOTMATCHVALUE				"VR not match value"
#define	DICOM_ERROR_STR_VMNOTMATCH					"VR not match"
#define	DICOM_ERROR_STR_ELEMPUSHFAILD				"Elem push failed"
#define	DICOM_ERROR_STR_ELEMSQPUSHFAILD				"Elem SQ pushed failed"
#define	DICOM_ERROR_STR_NOELEMIN						"No Elem in"
#define	DICOM_ERROR_STR_NOSQELEM						"No SQ Elem"
#define	DICOM_ERROR_STR_SOPERROR						"Sop error"
#define DICOM_ERROR_STR_NOFREEDISKSPACE				"No Free Disk Space"
#define DICOM_ERROR_STR_CANNOTOPENFILE				"Cannot open file"
#define DICOM_ERROR_STR_CANNOTDECODEFILE           "Cannot decode file"

#define	DICOM_ERROR_STR_DIMSE_NOAFFECSOPCLASUID		"DIMSE No Affected SOP Class UID"
#define	DICOM_ERROR_STR_DIMSE_NOAFFECSOPINSTUID		"DIMSE No Affected SOP Instance UID"
#define	DICOM_ERROR_STR_DIMSE_NOMOVEDSTINATION		"DIMSE No Move Destination"
#define	DICOM_ERROR_STR_DIMSE_NOREQUESOPCLASUID		"DIMSE No Request SOP Class UID"
#define	DICOM_ERROR_STR_DIMSE_NOREQUESOPINSTUID		"DIMSE No Request SOP Instance UID"
#define	DICOM_ERROR_STR_DIMSE_NOPRIORITY				"No Priority"
#define	DICOM_ERROR_STR_DIMSE_NOATTRIBIDLIST			"No Attribute List"

#define	DICOM_ERROR_STR_ELEMNOTINLIST				"Elem not in list"

#define DICOM_ERROR_STR_NOCALLEDAETITLE				"No Called AE Title"
#define DICOM_ERROR_STR_NOCALLINGAETITLE				"No Calling AE Title"
#define DICOM_ERROR_STR_NOABSSYNTAXADDED				"No Abstract Syntax Added"

#define DICOM_ERROR_STR_CANNOTREADASSOCAC			"Can not read AAssociationAC"
#define DICOM_ERROR_STR_CANNOTREADASSOCRJ			"Can not read AAssociationRJ"
#define DICOM_ERROR_STR_CANNOTREADRELESRQ			"Can not read AReleaseRQ"
#define DICOM_ERROR_STR_CANNOTREADRELESRP			"Can not read AReleaseRP"
#define DICOM_ERROR_STR_CANNOTREADASSOCRQ			"Can not read AAssociationRQ"
#define DICOM_ERROR_STR_CANNOTREADASSOCRP			"Can not read AAssociationRP"
#define DICOM_ERROR_STR_CANNOTREADASSOCRQ			"Can not read AAssociationRQ"
#define DICOM_ERROR_STR_CANNOTREADABORTRQ			"Can not read AAbortRQ"
#define DICOM_ERROR_STR_CANNOTREADPDATATF			"Can not read PDataTF"
#define DICOM_ERROR_STR_CANNOTWRITEASSOCAC			"Can not write AAssociationAC"
#define DICOM_ERROR_STR_CANNOTWRITEASSOCRJ			"Can not write AAssociationRJ"
#define DICOM_ERROR_STR_CANNOTWRITEASSOCAB			"Can not write AAssociationAB"
#define DICOM_ERROR_STR_CANNOTWRITEDICOMDATASET		"Can not write DICOM data set"

#define DICOM_ERROR_STR_TRANSYNTAXNOTSUPPORTED		"Transfer syntax not supported"

#define DICOM_ERROR_STR_UNEXPECTED_AASSOCIATERQ_READ	"Unexpected AAssociationRQ read"
#define DICOM_ERROR_STR_UNEXPECTED_AASSOCIATEAC_READ	"Unexpected AAssociationAC read"
#define DICOM_ERROR_STR_UNEXPECTED_AASSOCIATERJ_READ	"Unexpected AAssociationRJ read"
#define DICOM_ERROR_STR_UNEXPECTED_ARELEASERQ_READ	"Unexpected AReleaseRQ read"
#define DICOM_ERROR_STR_UNEXPECTED_ARELEASERP_READ	"Unexpected AReleaseRP read"
#define	DICOM_ERROR_STR_UNEXPECTED_AABORTRQ_READ		"Unexpected AAbortRQ read"
#define	DICOM_ERROR_STR_UNEXPECTED_NORECOGNIZED_READ	"Unexpected no recognized read"
#define	DICOM_ERROR_STR_UNEXPECTED_NODATA_READ		"Unexpected no data read"

//zjy added
#define DICOM_ERROR_STR_SOCKET_LINK_ERROR												"Socket link error"
#define DICOM_ERROR_STR_PDU_CONNECT_CORRUPT												"PDU connect corrupt"
#define DICOM_ERROR_STR_PDU_CONNECT_NOINTEROGATEAASSOCIATEAC								"PDU no Interogate AAssociationAC"
#define DICOM_ERROR_STR_PDU_CONNECT_REJECTPERMANENT_NOREASONGIVEN						"PDU reject permanent no reason given"
#define DICOM_ERROR_STR_PDU_CONNECT_REJECTPERMANENT_LOCALLIMITEXCEED						"PDU reject permanent local limit exceed"
#define DICOM_ERROR_STR_PDU_CONNECT_REJECTPERMANENT_TEMPORARYCONGESTION					"PDU reject permanent temporary congestion"
#define DICOM_ERROR_STR_PDU_CONNECT_REJECTPERMANENT_CALLEDAETITLENOTRECOGNIZED			"PDU reject permanent Called AE Title not recognized"
#define DICOM_ERROR_STR_PDU_CONNECT_REJECTPERMANENT_CALLINGAETITLENOTRECOGNIZED			"PDU reject permanent Calling AE Title not recognized"
#define DICOM_ERROR_STR_PDU_CONNECT_REJECTPERMANENT_PROTOCOLVERSIONNOTSUPPORTED			"PDU reject permanent protocol version not supported"
#define DICOM_ERROR_STR_PDU_CONNECT_REJECTPERMANENT_APPLICATIONCONTEXNAMENOTSUPPORTED	"PDU reject permanent application context name not supported"
#define DICOM_ERROR_STR_PDU_CONNECT_REJECTTRANSIENT_NOREASONGIVEN						"PDU reject transient no reason given"
#define DICOM_ERROR_STR_PDU_CONNECT_REJECTTRANSIENT_APPLICATIONCONTEXNAMENOTSUPPORTED	"PDU reject transient application context name not supported"
#define DICOM_ERROR_STR_PDU_CONNECT_REJECTTRANSIENT_CALLINGAETITLENOTRECOGNIZED			"PDU reject transient Calling AE Title not recognized"
#define DICOM_ERROR_STR_PDU_CONNECT_REJECTTRANSIENT_CALLEDAETITLENOTRECOGNIZED			"PDU reject transient Called AE Title not recognized"
#define DICOM_ERROR_STR_PDU_CONNECT_REJECTTRANSIENT_PROTOCOLVERSIONNOTSUPPORTED			"PDU reject transient protocol version not supported"
#define DICOM_ERROR_STR_PDU_CONNECT_REJECTTRANSIENT_TEMPORARYCONGESTION					"PDU reject transient temporary congestion"
#define DICOM_ERROR_STR_PDU_CONNECT_REJECTTRANSIENT_LOCALLIMITEXCEED						"PDU reject transient local limit exceed"
#define DICOM_ERROR_STR_RELEASE_WRITE_ERROR												"Release write error"
#define DICOM_ERROR_STR_ABORT_WRITE_ERROR												"Abort write error"

#define DICOM_ERROR_STR_JPGERROR															"JPG Error"

#define DICOM_ERROR_END -9999

//END ERROR DEFINED

/***************************************************************************/
/* DICOM publiclly registered UIDs														*/
/***************************************************************************/
#define	DCM_UID_NULL								-1
#define	DCM_UID_UNKNOWN								-1

#define	DCM_UID_1_2_840_10008_1_2				0	 /*Implicit VR Little Endian: Default Transfer Syntax for DICOM	Transfer Syntax	Part 5*/
#define	DCM_UID_1_2_840_10008_1_2_1				2	 /*Explicit VR Little Endian	Transfer Syntax	Part 5*/
#define	DCM_UID_1_2_840_10008_1_2_2				1	 /*Explicit VR Big Endian	Transfer Syntax	Part 5*/

#define	DCM_UID_1_2_840_10008_1_2_4_50			4	 /*JPEG Baseline (Process 1): Default Transfer Syntax for Lossy JPEG 8 Bit Image Compression Transfer Syntax	Part 5*/
#define	DCM_UID_1_2_840_10008_1_2_4_51			5	 /*JPEG Extended (Process 2 & 4): Default Transfer Syntax for Lossy JPEG 12 Bit Image Compression [Process 4 only]	Transfer Syntax	Part 5*/
#define	DCM_UID_1_2_840_10008_1_2_4_52			6	 /*JPEG Extended (Process 3 & 5) Transfer Syntax	Part 5*/
#define	DCM_UID_1_2_840_10008_1_2_4_53			7	 /*JPEG Spectral Selection, Non-Hierarchical (Process 6 & 8)	Transfer Syntax	Part 5*/
#define	DCM_UID_1_2_840_10008_1_2_4_54			8	 /*JPEG Spectral Selection, Non-Hierarchical (Process 7 & 9)	Transfer Syntax	Part 5*/
#define	DCM_UID_1_2_840_10008_1_2_4_55			9  /*JPEG Full Progression, Non-Hierarchical (Process 10 & 12)	Transfer Syntax	Part 5*/
#define	DCM_UID_1_2_840_10008_1_2_4_56			10  /*JPEG Full Progression, Non-Hierarchical (Process 11 & 13)	Transfer Syntax	Part 5*/
#define	DCM_UID_1_2_840_10008_1_2_4_57			11  /*JPEG Lossless, Non-Hierarchical (Process 14) Transfer Syntax	Part 5*/
#define	DCM_UID_1_2_840_10008_1_2_4_58			12  /*JPEG Lossless, Non-Hierarchical (Process 15) Transfer Syntax	Part 5*/
#define	DCM_UID_1_2_840_10008_1_2_4_59			13  /*JPEG Extended, Hierarchical (Process 16 & 18)	Transfer Syntax	Part 5*/
#define	DCM_UID_1_2_840_10008_1_2_4_60			14  /*JPEG Extended, Hierarchical (Process 17 & 19)	Transfer Syntax	Part 5*/
#define	DCM_UID_1_2_840_10008_1_2_4_61			15  /*JPEG Spectral Selection, Hierarchical (Process 20 & 22)	Transfer Syntax	Part 5*/
#define	DCM_UID_1_2_840_10008_1_2_4_62			16  /*JPEG Spectral Selection, Hierarchical (Process 21 & 23)	Transfer Syntax	Part 5*/
#define	DCM_UID_1_2_840_10008_1_2_4_63			17  /*JPEG Full Progression, Hierarchical (Process 24 & 26) Transfer Syntax	Part 5*/
#define	DCM_UID_1_2_840_10008_1_2_4_64			18  /*JPEG Full Progression, Hierarchical (Process 25 & 27) Transfer Syntax	Part 5*/
#define	DCM_UID_1_2_840_10008_1_2_4_65			19  /*JPEG Lossless, Hierarchical (Process 28)	Transfer Syntax	Part 5*/
#define	DCM_UID_1_2_840_10008_1_2_4_66			20  /*JPEG Lossless, Hierarchical (Process 29)	Transfer Syntax	Part 5*/
#define	DCM_UID_1_2_840_10008_1_2_4_70			21  /*JPEG Lossless, Hierarchical, First-Order Prediction (Process 14 [Selection Value 1]): Default Transfer Syntax for Lossless JPEG Image Compression	Transfer Syntax	Part 5*/

#define	DCM_UID_1_2_840_10008_1_2_5				22 /*RLE Compression PS3.5-1996 p47 */

#define	DCM_UID_1_2_840_10008_1_1					3	 /*Verification SOP Class	SOP Class	Part 4	*/
#define	DCM_UID_1_2_840_10008_1_9					23  /*Basic Study Content Notification SOP Class	SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_3_1_1_1			24  /*DICOM Application Context Name	Application Context Name	Part 7*/
#define	DCM_UID_1_2_840_10008_3_1_2_1_1			25  /*Detached Patient Management SOP Class	SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_3_1_2_1_4			26  /*Detached Patient Management Meta SOP Class	Meta SOP Class Part 4*/
#define	DCM_UID_1_2_840_10008_3_1_2_2_1			27  /*Detached Visit Management SOP Class SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_3_1_2_3_1			28  /*Detached Study Management SOP Class SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_3_1_2_3_2			39  /*Study Component Management SOP Class	SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_3_1_2_5_1			40  /*Detached Results Management SOP Class	SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_3_1_2_5_4			41  /*Detached Results Management Meta SOP Class	Meta SOP Class Part 4*/
#define	DCM_UID_1_2_840_10008_3_1_2_5_5			42  /*Detached Study Management Meta SOP Class	Meta SOP Class Part 4*/
#define	DCM_UID_1_2_840_10008_3_1_2_6_1			43  /*Detached Interpretation Management SOP Class SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_1_1			44  /*Basic Film Session SOP Class	SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_1_2			45  /*Basic Film Box SOP Class	SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_1_4			46  /*Basic Grayscale Image Box SOP Class SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_1_4_1			47  /*Basic Color Image Box SOP Class	SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_1_4_2			48  /*Referenced Image Box SOP Class	SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_1_9			49  /*Basic Grayscale Print Management Meta SOP Class Meta SOP Class Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_1_9_1			50  /*Referenced Grayscale Print Management Meta SOP Class	Meta SOP Class Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_1_14			51  /*Print Job SOP Class	SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_1_15			52  /*Basic Annotation Box SOP Class	SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_1_16			53  /*Printer SOP Class SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_1_17			54  /*Printer SOP Instance Well-known Printer SOP Instance	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_1_18			55  /*Basic Color Print Management Meta SOP Class	Meta SOP Class Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_1_18_1		56  /*Referenced Color Print Management Meta SOP Class	Meta SOP Class Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_1_22			57  /*VOI LUT Box SOP Class	SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_1_24			58  /*Image Overlay Box SOP Class	SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_4_1_1_1		59  /*Computed Radiography Image Storage	SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_4_1_1_2		60  /*CT Image Storage	SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_4_1_1_3		61  /*Ultrasound Multi-frame Image Storage	SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_4_1_1_4		62  /*MR Image Storage	SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_4_1_1_5		63  /*Nuclear medicine Image	Storage	SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_4_1_1_6		64  /*Ultrasound Image Storage	SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_4_1_1_7		65  /*Secondary Capture Image Storage	SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_4_1_1_8		66  /*Standalone Overlay  Storage	SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_4_1_1_9		67  /*Standalone Curve Storage	SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_4_1_1_10		68  /*Standalone Modality LUT Storage	SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_4_1_1_11		69  /*Standalone VOI LUT Storage SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_4_1_2_1_1	70  /*Patient Root Query/Retrieve Information Model - FIND	SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_4_1_2_1_2	71  /*Patient Root Query/Retrieve Information Model - MOVE	SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_4_1_2_1_3	72  /*Patient Root Query/Retrieve Information Model - GET	SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_4_1_2_2_1	73  /*Study Root Query/Retrieve Information Model - FIND SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_4_1_2_2_2	74  /*Study Root Query/Retrieve Information Model - MOVE SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_4_1_2_2_3	75  /*Study Root Query/Retrieve Information Model - GET	SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_4_1_2_3_1	76  /*Patient/Study Only Query/Retrieve Information Model  - FIND SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_4_1_2_3_2	77  /*Patient/Study Only Query/Retrieve Information Model  - MOVE SOP Class	Part 4*/
#define	DCM_UID_1_2_840_10008_5_1_4_1_2_3_3	78  /*Patient/Study Only Query/Retrieve Information Model  - GET	SOP Class	Part 4*/
#define DCM_UID_1_2_840_10008_5_1_4_1_1_20    79 /* Nuclear medicine Image Storage SOP Class - new */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_6_1   80 /* Ultrasound Image SOP Class - new  */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_3_1   81 /* Ultrasound Multi-frame Image Storage SOP Class - new  */

#define DCM_UID_1_2_840_10008_5_1_4_1_1_12_1  82 /* Xray Angio Image Storage SOP Class  */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_12_2  83 /* Xray RF Image Storage SOP Class  */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_12_3  84 /* Xray Angio Bi Plane Image Storage SOP Class  */

#define DCM_UID_1_2_840_10008_1_3_10          85 /* Media Directory Storage SOP  */
#define DCM_UID_1_2_840_10008_1_20_1          86 /* Storage Commitment Push Class SOP */
#define DCM_UID_1_2_840_10008_1_20_1_1        87 /* Storage Commitment Push Instance SOP */
#define DCM_UID_1_2_840_10008_1_20_2          88 /* Storage Commitment Pull Class SOP */
#define DCM_UID_1_2_840_10008_1_20_2_1        89 /* Storage Commitment Pull Instance SOP */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_481_1 90 /* RT Image Storage SOP */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_481_2 91 /* RT Dose Storage SOP */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_481_3 92 /* RT Structure Set Storage SOP */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_481_4 93 /* RT Treatment Record Storage SOP */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_481_5 94 /* RT Plan Storage SOP */

/* DICOM 98 additions */
#define DCM_UID_1_2_840_10008_5_1_1_27        95 /* Stored Print Image Storage SOP Class  */
#define DCM_UID_1_2_840_10008_5_1_1_29        96 /* Hardcopy Grayscale Image Storage SOP Class  */
#define DCM_UID_1_2_840_10008_5_1_1_30        97 /* Hardcopy Color Image Storage SOP Class  */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_128   98 /* PET Image Storage SOP Class  */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_129   99 /* Standalone PET Curve Image Storage SOP Class  */

/* additions from DICOM Supplement 32 */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_1_1     100 /* Digital Xray Pres Image Storage SOP */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_1_1_1   101 /* Digital Xray Proc Img Storage SOP */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_1_2     102 /* Digital Mammo Pres Img Storage SOP */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_1_2_1   103 /* Digital Mammo Proc Img Storage SOP */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_1_3     104 /* Digital Intra Oral Pres Img Storage SOP */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_1_3_1   105 /* Digital Intra Oral Proc Img Storage SOP */
/* additions from DICOM Supplement 30 */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_9_1     106 /* Waveform Storage SOP */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_9_1_1   107 /* Waveform Ecg Storage SOP */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_9_1_2   108 /* Waveform Audio Storage SOP */
/* additions from DICOM Supplement 15 */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_77_1_1  109 /* VL Endo Img Storage SOP */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_77_1_2  110 /* VL Micro Img Storage SOP */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_77_1_3  111 /* VL Slide Micro Img Storage SOP */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_77_1_4  112 /* VL Photo Img Storage SOP */
/* additions from DICOM Supplement 17 */
#define DCM_UID_1_2_840_10008_3_1_2_3_3         113 /* Mod Perf Proc Step SOP */
#define DCM_UID_1_2_840_10008_3_1_2_3_4         114 /* Mod Perf Proc Step Retrieve SOP */
#define DCM_UID_1_2_840_10008_3_1_2_3_5         115 /* Mod Perf Proc Step Notify SOP */
/* other additions */
#define DCM_UID_1_2_840_10008_5_1_4_31          116 /* Modality Worklist Find SOP */
/* additions from DICOM Supplement 23 */
#define DCM_UID_1_2_840_10008_5_1_1_23          117 /* Presentation Lut SOP */
/* other additions */
#define DCM_UID_1_2_840_10008_5_1_1_25          118 /* Print Queue Instance SOP */
#define DCM_UID_1_2_840_10008_5_1_1_26          119 /* Print Queue Mgmt SOP */
/* additions from DICOM Supplement 24 */
#define DCM_UID_1_2_840_10008_5_1_1_31          120 /* Pull Print Request SOP */
#define DCM_UID_1_2_840_10008_5_1_1_32          121 /* Pull Stored Print Mgmt Meta SOP */

//Additional added according 2000
#define DCM_UID_1_2_840_10008_5_1_4_1_1_481_6	122 /* RT Brachy Treatment Record Storage */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_481_7	123 /* RT Treatment Summary Record Storage */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_11_1	124 /* Grayscale Softcopy Presentation State Storage */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_9_1_3	125 /* Ambulatory ECG Waveform Storage */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_9_2_1	126 /* Hemodynamic Waveform Storage */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_9_3_1	127 /* Cardiac Electrophysiology Waveform Storage */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_9_4_1	128 /* Basic Voice Audio Waveform Storage */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_88_11	129 /* Basic Text SR */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_88_22	130 /* Enhanced SR */
#define DCM_UID_1_2_840_10008_5_1_4_1_1_88_33	131 /* Comprehensive SR */
#define DCM_UID_1_2_840_10008_5_1_1_24_1		132 /* Basic Print Image Overlay Box */
#define DCM_UID_1_2_840_10008_5_1_1_16_376		133 /* Basic Print Image Overlay Box */
//2003
#define DCM_UID_1_2_840_10008_5_1_4_32_1		134 /* General purpose worklist find sop*/
#define DCM_UID_1_2_840_10008_5_1_4_32_2		135	/* General Purpose Scheduled Procedure Step */
#define	DCM_UID_1_2_840_10008_5_1_4_32_3		136	/* General Purpose Performed Procedure Step */

/***************************************************************************/
/* SOP Classes																					*/
/***************************************************************************/

#define DCM_SOP_NULL								0

#define	DCM_SOP_VERIFICATION		DCM_UID_1_2_840_10008_1_1					/*Verification SOP Class	SOP Class	Part 4	*/
#define	DCM_SOP_CR_STORAGE			DCM_UID_1_2_840_10008_5_1_4_1_1_1		/*Computed Radiography Image Storage	SOP Class	Part 4*/
#define	DCM_SOP_CT_STORAGE			DCM_UID_1_2_840_10008_5_1_4_1_1_2		/*CT Image Storage	SOP Class	Part 4*/
/* #define	DCM_SOP_USMF_STORAGE	DCM_UID_1_2_840_10008_5_1_4_1_1_3		Retired: Ultrasound Multi-frame Image Storage	SOP Class	Part 4*/
#define	DCM_SOP_USMF_STORAGE		DCM_UID_1_2_840_10008_5_1_4_1_1_3_1		/*Ultrasound Multi-frame Image Storage	SOP Class	Part 4*/
#define	DCM_SOP_MR_STORAGE			DCM_UID_1_2_840_10008_5_1_4_1_1_4		/*MR Image Storage	SOP Class	Part 4*/
/* #define	DCM_SOP_NM_STORAGE		DCM_UID_1_2_840_10008_5_1_4_1_1_5		Retired: Nuclear medicine Image	Storage	SOP Class	Part 4*/
/* #define	DCM_SOP_US_STORAGE		DCM_UID_1_2_840_10008_5_1_4_1_1_6		Retired: Ultrasound Image Storage	SOP Class	Part 4*/
#define	DCM_SOP_US_STORAGE			DCM_UID_1_2_840_10008_5_1_4_1_1_6_1		/* Ultrasound Image Storage	SOP Class	Part 4*/
#define	DCM_SOP_SC_STORAGE			DCM_UID_1_2_840_10008_5_1_4_1_1_7		/*Secondary Capture Image Storage	SOP Class	Part 4*/
#define	DCM_SOP_OVERLAY_STORAGE		DCM_UID_1_2_840_10008_5_1_4_1_1_8		/*Standalone Overlay  Storage	SOP Class	Part 4*/
#define	DCM_SOP_CURVE_STORAGE		DCM_UID_1_2_840_10008_5_1_4_1_1_9		/*Standalone Curve Storage	SOP Class	Part 4*/
#define	DCM_SOP_MOD_LUT_STORAGE		DCM_UID_1_2_840_10008_5_1_4_1_1_10		/*Standalone Modality LUT Storage	SOP Class	Part 4*/
#define	DCM_SOP_VOI_LUT_STORAGE		DCM_UID_1_2_840_10008_5_1_4_1_1_11		/*Standalone VOI LUT Storage SOP Class	Part 4*/
#define DCM_SOP_XRAY_ANGIO_STORAGE               DCM_UID_1_2_840_10008_5_1_4_1_1_12_1
#define DCM_SOP_XRAY_RF_STORAGE                  DCM_UID_1_2_840_10008_5_1_4_1_1_12_2
#define DCM_SOP_XRAY_ANGIO_BI_PLANE_STORAGE      DCM_UID_1_2_840_10008_5_1_4_1_1_12_3
#define	DCM_SOP_NM_STORAGE			DCM_UID_1_2_840_10008_5_1_4_1_1_20		/*Nuclear medicine Image	Storage	SOP Class	Part 4*/
#define DCM_SOP_PAT_ROOT_QR_FIND                 DCM_UID_1_2_840_10008_5_1_4_1_2_1_1
#define DCM_SOP_PAT_ROOT_QR_MOVE                 DCM_UID_1_2_840_10008_5_1_4_1_2_1_2
#define DCM_SOP_PAT_ROOT_QR_GET                  DCM_UID_1_2_840_10008_5_1_4_1_2_1_3
#define DCM_SOP_STUDY_ROOT_QR_FIND               DCM_UID_1_2_840_10008_5_1_4_1_2_2_1
#define DCM_SOP_STUDY_ROOT_QR_MOVE               DCM_UID_1_2_840_10008_5_1_4_1_2_2_2
#define DCM_SOP_STUDY_ROOT_QR_GET                DCM_UID_1_2_840_10008_5_1_4_1_2_2_3
#define DCM_SOP_PAT_STUDY_ROOT_QR_FIND           DCM_UID_1_2_840_10008_5_1_4_1_2_3_1
#define DCM_SOP_PAT_STUDY_ROOT_QR_MOVE           DCM_UID_1_2_840_10008_5_1_4_1_2_3_2
#define DCM_SOP_PAT_STUDY_ROOT_QR_GET            DCM_UID_1_2_840_10008_5_1_4_1_2_3_3
#define DCM_SOP_PAT_MGMT_DET                     DCM_UID_1_2_840_10008_3_1_2_1_1
#define DCM_SOP_VISIT_MGMT_DET                   DCM_UID_1_2_840_10008_3_1_2_2_1
#define DCM_SOP_PAT_MGMT_META                    DCM_UID_1_2_840_10008_3_1_2_1_4
#define DCM_SOP_RESULT_MGMT_DET                  DCM_UID_1_2_840_10008_3_1_2_5_1
#define DCM_SOP_INTERP_MGMT_DET                  DCM_UID_1_2_840_10008_3_1_2_6_1
#define DCM_SOP_RESULT_MGMT_META                 DCM_UID_1_2_840_10008_3_1_2_5_4
#define DCM_SOP_STUDY_MGMT_DET                   DCM_UID_1_2_840_10008_3_1_2_3_1
#define DCM_SOP_STUDY_MGMT_COMP                  DCM_UID_1_2_840_10008_3_1_2_3_2
#define DCM_SOP_STUDY_MGMT_META                  DCM_UID_1_2_840_10008_3_1_2_5_5
#define DCM_SOP_BASIC_FILM_SESSION               DCM_UID_1_2_840_10008_5_1_1_1
#define DCM_SOP_BASIC_FILM_BOX                   DCM_UID_1_2_840_10008_5_1_1_2
#define DCM_SOP_BASIC_GRAY_IMG_BOX               DCM_UID_1_2_840_10008_5_1_1_4
#define DCM_SOP_BASIC_COLOR_IMG_BOX              DCM_UID_1_2_840_10008_5_1_1_4_1
#define DCM_SOP_BASIC_ANNOTATION_BOX             DCM_UID_1_2_840_10008_5_1_1_15
#define DCM_SOP_REF_IMG_BOX                      DCM_UID_1_2_840_10008_5_1_1_4_2
#define DCM_SOP_PRINT_JOB                        DCM_UID_1_2_840_10008_5_1_1_14
#define DCM_SOP_PRINTER                          DCM_UID_1_2_840_10008_5_1_1_16
#define DCM_SOP_PRINTER_INSTANCE                 DCM_UID_1_2_840_10008_5_1_1_17
#define DCM_SOP_VOI_LUT_BOX                      DCM_UID_1_2_840_10008_5_1_1_22
#define DCM_SOP_IMG_OVLY_BOX                     DCM_UID_1_2_840_10008_5_1_1_24
#define DCM_SOP_BASIC_GRAY_PRINT_MGMT_META       DCM_UID_1_2_840_10008_5_1_1_9
#define DCM_SOP_BASIC_COLOR_PRINT_MGMT_META      DCM_UID_1_2_840_10008_5_1_1_18
#define DCM_SOP_REF_GRAY_PRINT_MGMT_META         DCM_UID_1_2_840_10008_5_1_1_9_1
#define DCM_SOP_REF_COLOR_PRINT_MGMT_META        DCM_UID_1_2_840_10008_5_1_1_18_1
#define DCM_SOP_VICOIA_STORAGE_DIR_STORAGE        DCM_UID_1_2_840_10008_1_3_10
#define DCM_SOP_BASIC_STUDY_CONTENT_NOTIFICATION DCM_UID_1_2_840_10008_1_9
#define DCM_SOP_STORAGE_COMMITMENT_PUSH_CLASS    DCM_UID_1_2_840_10008_1_20_1
#define DCM_SOP_STORAGE_COMMITMENT_PUSH_INSTANCE DCM_UID_1_2_840_10008_1_20_1_1
#define DCM_SOP_STORAGE_COMMITMENT_PULL_CLASS    DCM_UID_1_2_840_10008_1_20_2
#define DCM_SOP_STORAGE_COMMITMENT_PULL_INSTANCE DCM_UID_1_2_840_10008_1_20_2_1
#define DCM_SOP_RT_IMG_STORAGE                   DCM_UID_1_2_840_10008_5_1_4_1_1_481_1
#define DCM_SOP_RT_DOSE_STORAGE                  DCM_UID_1_2_840_10008_5_1_4_1_1_481_2
#define DCM_SOP_RT_STRUCTURE_SET_STORAGE         DCM_UID_1_2_840_10008_5_1_4_1_1_481_3
#define DCM_SOP_RT_BEAMS_TREATMENT_RECORD_STORAGE      DCM_UID_1_2_840_10008_5_1_4_1_1_481_4
#define DCM_SOP_RT_PLAN_STORAGE                  DCM_UID_1_2_840_10008_5_1_4_1_1_481_5

/* DICOM 98 additions */
#define DCM_SOP_STORED_PRINT_STORAGE             DCM_UID_1_2_840_10008_5_1_1_27
#define DCM_SOP_HARDCOPY_GRAYSCALE_STORAGE       DCM_UID_1_2_840_10008_5_1_1_29
#define DCM_SOP_HARDCOPY_COLOR_STORAGE           DCM_UID_1_2_840_10008_5_1_1_30
#define DCM_SOP_PET_STORAGE                      DCM_UID_1_2_840_10008_5_1_4_1_1_128
#define DCM_SOP_PET_CURVE_STORAGE                DCM_UID_1_2_840_10008_5_1_4_1_1_129

/* various supplement additions */
#define DCM_SOP_DIGI_XRAY_PRES_IMG_STORAGE       DCM_UID_1_2_840_10008_5_1_4_1_1_1_1
#define DCM_SOP_DIGI_XRAY_PROC_IMG_STORAGE       DCM_UID_1_2_840_10008_5_1_4_1_1_1_1_1
#define DCM_SOP_DIGI_MAMMO_PRES_IMG_STORAGE      DCM_UID_1_2_840_10008_5_1_4_1_1_1_2
#define DCM_SOP_DIGI_MAMMO_PROC_IMG_STORAGE      DCM_UID_1_2_840_10008_5_1_4_1_1_1_2_1
#define DCM_SOP_DIGI_INTRA_ORAL_PRES_IMG_STORAGE DCM_UID_1_2_840_10008_5_1_4_1_1_1_3
#define DCM_SOP_DIGI_INTRA_ORAL_PROC_IMG_STORAGE DCM_UID_1_2_840_10008_5_1_4_1_1_1_3_1
#define DCM_SOP_WAVEFORM_STORAGE                 DCM_UID_1_2_840_10008_5_1_4_1_1_9_1
#define DCM_SOP_12_LEAD_ECG_WAVEFORM_STORAGE     DCM_UID_1_2_840_10008_5_1_4_1_1_9_1_1
#define DCM_SOP_GENERAL_ECG_WAVEFORM_STORAGE     DCM_UID_1_2_840_10008_5_1_4_1_1_9_1_2
#define DCM_SOP_VL_ENDO_IMG_STORAGE              DCM_UID_1_2_840_10008_5_1_4_1_1_77_1_1
#define DCM_SOP_VL_MICRO_IMG_STORAGE             DCM_UID_1_2_840_10008_5_1_4_1_1_77_1_2
#define DCM_SOP_VL_SLIDE_MICRO_IMG_STORAGE       DCM_UID_1_2_840_10008_5_1_4_1_1_77_1_3
#define DCM_SOP_VL_PHOTO_IMG_STORAGE             DCM_UID_1_2_840_10008_5_1_4_1_1_77_1_4
#define DCM_SOP_MOD_PERF_PROC_STEP               DCM_UID_1_2_840_10008_3_1_2_3_3
#define DCM_SOP_MOD_PERF_PROC_STEP_RETRIEVE      DCM_UID_1_2_840_10008_3_1_2_3_4
#define DCM_SOP_MOD_PERF_PROC_STEP_NOTIFY        DCM_UID_1_2_840_10008_3_1_2_3_5
#define DCM_SOP_MODALITY_WORKLIST_FIND           DCM_UID_1_2_840_10008_5_1_4_31
#define DCM_SOP_PRESENTATION_LUT                 DCM_UID_1_2_840_10008_5_1_1_23
#define DCM_SOP_PRINT_QUEUE_INSTANCE             DCM_UID_1_2_840_10008_5_1_1_25
#define DCM_SOP_PRINT_QUEUE_MGMT                 DCM_UID_1_2_840_10008_5_1_1_26
#define DCM_SOP_PULL_PRINT_REQUEST               DCM_UID_1_2_840_10008_5_1_1_31
#define DCM_SOP_PULL_STORED_PRINT_MGMT_META      DCM_UID_1_2_840_10008_5_1_1_32

//Additional added according to 2000
#define DCM_SOP_RT_BRACHY_TREATMENT_RECORD_STORAGE     DCM_UID_1_2_840_10008_5_1_4_1_1_481_6
#define DCM_SOP_RT_TREATMENT_SUMMARY_RECORD_STORAGE    DCM_UID_1_2_840_10008_5_1_4_1_1_481_7
#define DCM_SOP_GRAYSCALE_SOFTCOPY_PRESENTATION_STATE_STORAGE	DCM_UID_1_2_840_10008_5_1_4_1_1_11_1
#define	DCM_SOP_AMBULATORY_ECG_WAVEFORM_STORAGE		DCM_UID_1_2_840_10008_5_1_4_1_1_9_1_3
#define	DCM_SOP_HEMODYNAMIC_WAVEFORM_STORAGE			DCM_UID_1_2_840_10008_5_1_4_1_1_9_2_1
#define	DCM_SOP_CARDIAC_ELECTROPHYSIOLOGY_WAVEFORM_STORAGE		DCM_UID_1_2_840_10008_5_1_4_1_1_9_3_1
#define	DCM_SOP_BASIC_VOICE_AUDIO_WAVEFORM_STORAGE		DCM_UID_1_2_840_10008_5_1_4_1_1_9_4_1
#define	DCM_SOP_BASIC_TEXT_SR							DCM_UID_1_2_840_10008_5_1_4_1_1_88_11
#define	DCM_SOP_ENHANCED_SR							DCM_UID_1_2_840_10008_5_1_4_1_1_88_22
#define	DCM_SOP_COMPREHENSIVE_SR						DCM_UID_1_2_840_10008_5_1_4_1_1_88_33
#define	DCM_SOP_BASIC_PRINT_IMAGE_OVERLAY_BOX			DCM_UID_1_2_840_10008_5_1_1_24_1
#define	DCM_SOP_PRINTER_CONFIGURATION_RETRIEVAL		DCM_UID_1_2_840_10008_5_1_1_16_376

//2003
#define	DCM_SOP_GENERAL_PURPOSE_WORKLIST_FIND			DCM_UID_1_2_840_10008_5_1_4_32_1
#define	DCM_SOP_GENERAL_PURPOSE_SCHEDULED_PROC_STEP	DCM_UID_1_2_840_10008_5_1_4_32_2
#define	DCM_SOP_GENERAL_PURPOSE_PERFORMED_PROC_STEP	DCM_UID_1_2_840_10008_5_1_4_32_3

#define DCM_SOP_STRG_VERIFICATION                     "1.2.840.10008.1.1"
#define DCM_SOP_STRG_CR_STORAGE                       "1.2.840.10008.5.1.4.1.1.1"
#define DCM_SOP_STRG_CT_STORAGE                       "1.2.840.10008.5.1.4.1.1.2"
#define DCM_SOP_STRG_MR_STORAGE                       "1.2.840.10008.5.1.4.1.1.4"
#define DCM_SOP_STRG_NM_STORAGE                       "1.2.840.10008.5.1.4.1.1.20"
#define DCM_SOP_STRG_US_STORAGE                       "1.2.840.10008.5.1.4.1.1.6.1"
#define DCM_SOP_STRG_USMF_STORAGE                     "1.2.840.10008.5.1.4.1.1.3.1"
#define DCM_SOP_STRG_SC_STORAGE                       "1.2.840.10008.5.1.4.1.1.7"
#define DCM_SOP_STRG_XRAY_ANGIO_STORAGE               "1.2.840.10008.5.1.4.1.1.12.1"
#define DCM_SOP_STRG_XRAY_RF_STORAGE                  "1.2.840.10008.5.1.4.1.1.12.2"
#define DCM_SOP_STRG_XRAY_ANGIO_BI_PLANE_STORAGE      "1.2.840.10008.5.1.4.1.1.12.3"
#define DCM_SOP_STRG_OVERLAY_STORAGE                  "1.2.840.10008.5.1.4.1.1.8"
#define DCM_SOP_STRG_CURVE_STORAGE                    "1.2.840.10008.5.1.4.1.1.9"
#define DCM_SOP_STRG_MOD_LUT_STORAGE                  "1.2.840.10008.5.1.4.1.1.10"
#define DCM_SOP_STRG_VOI_LUT_STORAGE                  "1.2.840.10008.5.1.4.1.1.11"
#define DCM_SOP_STRG_PAT_ROOT_QR_FIND                 "1.2.840.10008.5.1.4.1.2.1.1"
#define DCM_SOP_STRG_PAT_ROOT_QR_MOVE                 "1.2.840.10008.5.1.4.1.2.1.2"
#define DCM_SOP_STRG_PAT_ROOT_QR_GET                  "1.2.840.10008.5.1.4.1.2.1.3"
#define DCM_SOP_STRG_STUDY_ROOT_QR_FIND               "1.2.840.10008.5.1.4.1.2.2.1"
#define DCM_SOP_STRG_STUDY_ROOT_QR_MOVE               "1.2.840.10008.5.1.4.1.2.2.2"
#define DCM_SOP_STRG_STUDY_ROOT_QR_GET                "1.2.840.10008.5.1.4.1.2.2.3"
#define DCM_SOP_STRG_PAT_STUDY_ROOT_QR_FIND           "1.2.840.10008.5.1.4.1.2.3.1"
#define DCM_SOP_STRG_PAT_STUDY_ROOT_QR_MOVE           "1.2.840.10008.5.1.4.1.2.3.2"
#define DCM_SOP_STRG_PAT_STUDY_ROOT_QR_GET            "1.2.840.10008.5.1.4.1.2.3.3"
#define DCM_SOP_STRG_PAT_MGMT_DET                     "1.2.840.10008.3.1.2.1.1"
#define DCM_SOP_STRG_VISIT_MGMT_DET                   "1.2.840.10008.3.1.2.2.1"
#define DCM_SOP_STRG_PAT_MGMT_META                    "1.2.840.10008.3.1.2.1.4"
#define DCM_SOP_STRG_RESULT_MGMT_DET                  "1.2.840.10008.3.1.2.5.1"
#define DCM_SOP_STRG_INTERP_MGMT_DET                  "1.2.840.10008.3.1.2.6.1"
#define DCM_SOP_STRG_RESULT_MGMT_META                 "1.2.840.10008.3.1.2.5.4"
#define DCM_SOP_STRG_STUDY_MGMT_DET                   "1.2.840.10008.3.1.2.3.1"
#define DCM_SOP_STRG_STUDY_MGMT_COMP                  "1.2.840.10008.3.1.2.3.2"
#define DCM_SOP_STRG_STUDY_MGMT_META                  "1.2.840.10008.3.1.2.5.5"
#define DCM_SOP_STRG_BASIC_FILM_SESSION               "1.2.840.10008.5.1.1.1"
#define DCM_SOP_STRG_BASIC_FILM_BOX                   "1.2.840.10008.5.1.1.2"
#define DCM_SOP_STRG_BASIC_GRAY_IMG_BOX               "1.2.840.10008.5.1.1.4"
#define DCM_SOP_STRG_BASIC_COLOR_IMG_BOX              "1.2.840.10008.5.1.1.4.1"
#define DCM_SOP_STRG_BASIC_ANNOTATION_BOX             "1.2.840.10008.5.1.1.15"
#define DCM_SOP_STRG_REF_IMG_BOX                      "1.2.840.10008.5.1.1.4.2"
#define DCM_SOP_STRG_PRINT_JOB                        "1.2.840.10008.5.1.1.14"
#define DCM_SOP_STRG_PRINTER                          "1.2.840.10008.5.1.1.16"
#define DCM_SOP_STRG_PRINTER_INSTANCE                 "1.2.840.10008.5.1.1.17"
#define DCM_SOP_STRG_VOI_LUT_BOX                      "1.2.840.10008.5.1.1.22"
#define DCM_SOP_STRG_IMG_OVLY_BOX                     "1.2.840.10008.5.1.1.24"
#define DCM_SOP_STRG_BASIC_GRAY_PRINT_MGMT_META       "1.2.840.10008.5.1.1.9"
#define DCM_SOP_STRG_BASIC_COLOR_PRINT_MGMT_META      "1.2.840.10008.5.1.1.18"
#define DCM_SOP_STRG_REF_GRAY_PRINT_MGMT_META         "1.2.840.10008.5.1.1.9.1"
#define DCM_SOP_STRG_REF_COLOR_PRINT_MGMT_META        "1.2.840.10008.5.1.1.18.1"
#define DCM_SOP_STRG_VICOIA_STORAGE_DIR_STORAGE        "1.2.840.10008.1.3.10"
#define DCM_SOP_STRG_BASIC_STUDY_CONTENT_NOTIFICATION "1.2.840.10008.1.9"
#define DCM_SOP_STRG_STORAGE_COMMITMENT_PUSH_CLASS    "1.2.840.10008.1.20.1"
#define DCM_SOP_STRG_STORAGE_COMMITMENT_PUSH_INSTANCE "1.2.840.10008.1.20.1.1"
#define DCM_SOP_STRG_STORAGE_COMMITMENT_PULL_CLASS    "1.2.840.10008.1.20.2"
#define DCM_SOP_STRG_STORAGE_COMMITMENT_PULL_INSTANCE "1.2.840.10008.1.20.2.1"
#define DCM_SOP_STRG_RT_IMG_STORAGE                   "1.2.840.10008.5.1.4.1.1.481.1"
#define DCM_SOP_STRG_RT_DOSE_STORAGE                  "1.2.840.10008.5.1.4.1.1.481.2"
#define DCM_SOP_STRG_RT_STRUCTURE_SET_STORAGE         "1.2.840.10008.5.1.4.1.1.481.3"
#define DCM_SOP_STRG_RT_BEAMS_TREATMENT_RECORD_STORAGE      "1.2.840.10008.5.1.4.1.1.481.4"
#define DCM_SOP_STRG_RT_PLAN_STORAGE                  "1.2.840.10008.5.1.4.1.1.481.5"

/* DICOM 98 additions */
#define DCM_SOP_STRG_STORED_PRINT_STORAGE             "1.2.840.10008.5.1.1.27"
#define DCM_SOP_STRG_HARDCOPY_GRAYSCALE_STORAGE       "1.2.840.10008.5.1.1.29"
#define DCM_SOP_STRG_HARDCOPY_COLOR_STORAGE           "1.2.840.10008.5.1.1.30"
#define DCM_SOP_STRG_PET_STORAGE                      "1.2.840.10008.5.1.4.1.1.128"
#define DCM_SOP_STRG_PET_CURVE_STORAGE                "1.2.840.10008.5.1.4.1.1.129"

/* various supplement additions */
#define DCM_SOP_STRG_DIGI_XRAY_PRES_IMG_STORAGE       "1.2.840.10008.5.1.4.1.1.1.1"
#define DCM_SOP_STRG_DIGI_XRAY_PROC_IMG_STORAGE       "1.2.840.10008.5.1.4.1.1.1.1.1"
#define DCM_SOP_STRG_DIGI_MAMMO_PRES_IMG_STORAGE      "1.2.840.10008.5.1.4.1.1.1.2"
#define DCM_SOP_STRG_DIGI_MAMMO_PROC_IMG_STORAGE      "1.2.840.10008.5.1.4.1.1.1.2.1"
#define DCM_SOP_STRG_DIGI_INTRA_ORAL_PRES_IMG_STORAGE "1.2.840.10008.5.1.4.1.1.1.3"
#define DCM_SOP_STRG_DIGI_INTRA_ORAL_PROC_IMG_STORAGE "1.2.840.10008.5.1.4.1.1.1.3.1"
#define DCM_SOP_STRG_WAVEFORM_STORAGE                 "1.2.840.10008.5.1.4.1.1.9.1"
#define DCM_SOP_STRG_12_LEAD_ECG_WAVEFORM_STORAGE     "1.2.840.10008.5.1.4.1.1.9.1.1"
#define DCM_SOP_STRG_GENERAL_ECG_WAVEFORM_STORAGE     "1.2.840.10008.5.1.4.1.1.9.1.2"
#define DCM_SOP_STRG_VL_ENDO_IMG_STORAGE              "1.2.840.10008.5.1.4.1.1.77.1.1"
#define DCM_SOP_STRG_VL_MICRO_IMG_STORAGE             "1.2.840.10008.5.1.4.1.1.77.1.2"
#define DCM_SOP_STRG_VL_SLIDE_MICRO_IMG_STORAGE       "1.2.840.10008.5.1.4.1.1.77.1.3"
#define DCM_SOP_STRG_VL_PHOTO_IMG_STORAGE             "1.2.840.10008.5.1.4.1.1.77.1.4"
#define DCM_SOP_STRG_MOD_PERF_PROC_STEP               "1.2.840.10008.3.1.2.3.3"
#define DCM_SOP_STRG_MOD_PERF_PROC_STEP_RETRIEVE      "1.2.840.10008.3.1.2.3.4"
#define DCM_SOP_STRG_MOD_PERF_PROC_STEP_NOTIFY        "1.2.840.10008.3.1.2.3.5"
#define DCM_SOP_STRG_MODALITY_WORKLIST_FIND           "1.2.840.10008.5.1.4.31"
#define DCM_SOP_STRG_PRESENTATION_LUT                 "1.2.840.10008.5.1.1.23"
#define DCM_SOP_STRG_PRINT_QUEUE_INSTANCE             "1.2.840.10008.5.1.1.25"
#define DCM_SOP_STRG_PRINT_QUEUE_MGMT                 "1.2.840.10008.5.1.1.26"
#define DCM_SOP_STRG_PULL_PRINT_REQUEST               "1.2.840.10008.5.1.1.31"
#define DCM_SOP_STRG_PULL_STORED_PRINT_MGMT_META      "1.2.840.10008.5.1.1.32"

//Additional added according to 2000
#define	DCM_SOP_STRG_RT_BRACHY_TREATMENT_RECORD_STORAGE	"1.2.840.10008.5.1.4.1.1.481.6"
#define	DCM_SOP_STRG_RT_TREATMENT_SUMMARY_RECORD_STORAGE	"1.2.840.10008.5.1.4.1.1.481.7"
#define	DCM_SOP_STRG_GRAYSCALE_SOFTCOPY_PRESENTATION_STATE_STORAGE	"1.2.840.10008.5.1.4.1.1.11.1"
#define	DCM_SOP_STRG_AMBULATORY_ECG_WAVEFORM_STORAGE		"1.2.840.10008.5.1.4.1.1.9.1.3"
#define	DCM_SOP_STRG_HEMODYNAMIC_WAVEFORM_STORAGE			"1.2.840.10008.5.1.4.1.1.9.2.1"
#define	DCM_SOP_STRG_CARDIAC_ELECTROPHYSIOLOGY_WAVEFORM_STORAGE	"1.2.840.10008.5.1.4.1.1.9.3.1"
#define	DCM_SOP_STRG_BASIC_VOICE_AUDIO_WAVEFORM_STORAGE	"1.2.840.10008.5.1.4.1.1.9.4.1"
#define	DCM_SOP_STRG_BASIC_TEXT_SR							"1.2.840.10008.5.1.4.1.1.88.11"
#define	DCM_SOP_STRG_ENHANCED_SR							"1.2.840.10008.5.1.4.1.1.88.22"
#define	DCM_SOP_STRG_COMPREHENSIVE_SR						"1.2.840.10008.5.1.4.1.1.88.33"
#define	DCM_SOP_STRG_BASIC_PRINT_IMAGE_OVERLAY_BOX			"1.2.840.10008.5.1.1.24.1"
#define	DCM_SOP_STRG_PRINTER_CONFIGURATION_RETRIEVAL		"1.2.840.10008.5.1.1.16.376"

//2003
#define	DCM_SOP_STRG_GENERAL_PURPOSE_WORKLIST_FIND			"1.2.840.10008.5.1.4.32.1"
#define	DCM_SOP_STRG_GENERAL_PURPOSE_SCHEDULED_PROC_STEP	"1.2.840.10008.5.1.4.32.2"
#define	DCM_SOP_STRG_GENERAL_PURPOSE_PERFORMED_PROC_STEP	"1.2.840.10008.5.1.4.32.3"
//...
#define	DCM_TS_STRG_IMPLICIT_VR_LE					   "1.2.840.10008.1.2"
#define	DCM_TS_STRG_EXPLICIT_VR_LE					   "1.2.840.10008.1.2.1"
#define	DCM_TS_STRG_EXPLICIT_VR_BE					   "1.2.840.10008.1.2.2"
#define	DCM_TS_STRG_DEFAULT						   DCM_TS_STRG_IMPLICIT_VR_LE
#define	DCM_TS_STRG_RLE								"1.2.840.10008.1.2.5"
#define	DCM_TS_STRG_JPEG_LOSSY							"1.2.840.10008.1.2.4.50"
#define	DCM_TS_STRG_JPEG_LOSSY_8						"1.2.840.10008.1.2.4.50"
#define	DCM_TS_STRG_JPEG_LOSSY_12						"1.2.840.10008.1.2.4.50"
#define	DCM_TS_STRG_JPEG_LOSSY_EXTENDED				"1.2.840.10008.1.2.4.51"
#define	DCM_TS_STRG_JPEG_LOSSLESS_HUFFMAN				"1.2.840.10008.1.2.4.57"
#define	DCM_TS_STRG_JPEG_LOSSLESS_FIRSTORDER			"1.2.840.10008.1.2.4.70"

#define	DCM_TS_STRG_JPEG_LOSSY_SPECTRAL_SELECTION		"1.2.840.10008.1.2.4.53" /* JPEG process 6 & 8				*/
#define	DCM_TS_STRG_JPEG_LOSSY_FULL_PROGRESSION		"1.2.840.10008.1.2.4.55" /* JPEG process 10 & 12				*/

/***************************************************************************/
/* Transfer Syntaxes																			*/
/***************************************************************************/
#define	DCM_TS_IMPLICIT_VR_LE				DCM_UID_1_2_840_10008_1_2			/*Implicit VR Little Endian: Default Transfer Syntax					*/
#define	DCM_TS_EXPLICIT_VR_LE				DCM_UID_1_2_840_10008_1_2_1		/*Explicit VR Little Endian: This is used for the Part 10 Header	*/
#define	DCM_TS_EXPLICIT_VR_BE				DCM_UID_1_2_840_10008_1_2_2		/*Explicit VR Big Endian														*/

#define	DCM_TS_DEFAULT						DCM_TS_IMPLICIT_VR_LE
#define	DCM_TS_PART_10						9997
#define	DCM_TS_AUTODETECT					9998

#define	DCM_TS_UNKNOWN						DCM_UID_UNKNOWN

#define	DCM_TS_RLE								DCM_UID_1_2_840_10008_1_2_5
#define	DCM_TS_JPEG_LOSSY						DCM_UID_1_2_840_10008_1_2_4_50	/* JPEG process 01	baseline		*/
#define	DCM_TS_JPEG_LOSSY_8						DCM_UID_1_2_840_10008_1_2_4_50	/* JPEG process 01	baseline		*/
#define	DCM_TS_JPEG_LOSSY_12						DCM_UID_1_2_840_10008_1_2_4_50	/* JPEG process 01	baseline		*/
#define	DCM_TS_JPEG_LOSSY_EXTENDED				DCM_UID_1_2_840_10008_1_2_4_51	 /*JPEG Process 2 & 4				*/
#define	DCM_TS_JPEG_LOSSY_SPECTRAL_SELECTION	DCM_UID_1_2_840_10008_1_2_4_53 /* JPEG process 6 & 8				*/
#define	DCM_TS_JPEG_LOSSY_FULL_PROGRESSION		DCM_UID_1_2_840_10008_1_2_4_55 /* JPEG process 10 & 12				*/
#define	DCM_TS_JPEG_LOSSLESS_HUFFMAN				DCM_UID_1_2_840_10008_1_2_4_57	/* JPEG process 14	lossless non-hierachical Huffman*/
#define	DCM_TS_JPEG_LOSSLESS_FIRSTORDER			DCM_UID_1_2_840_10008_1_2_4_70  /*JPEG Lossless, Hierarchical, First-Order Prediction	*/

#define	DCM_VR_TAG_MAKE(gn,en)		  (((DWORD)(gn)<<16)|((DWORD)(en)))
#define	DCM_VR_TAG_GROUP(tag)		  ((WORD)((tag>>16)&0xFFFF))
#define	DCM_VR_TAG_ELEMENT(tag)	      ((WORD)(tag&0xFFFF))

#define DCM_MAX_ALLOWED_TS	10 // Max allowed transfer syntax
#define DCM_MAX_ALLOWED_AS	1000 // Max allowed Abstract syntax
#define DCM_MAX_ALLOWED_AT	1000 // Max allowed atribute list length

#define MAX_HOSTNAME (15 + 1)
#define MAX_VRAE 16
#define MAX_VRAS 4
#define MAX_VRAT 4
#define MAX_VRCS 16
#define MAX_VRDA 8
#define MAX_VRDS 16
#define MAX_VRDT 26
#define MAX_VRFL 4
#define MAX_VRFD 8
#define MAX_VRIS 12
#define MAX_VRLO 64
#define MAX_VRLT 10240
#define MAX_VRPN 320
#define MAX_VRSH 16
#define MAX_VRSL 4
#define MAX_VRSS 2
#define MAX_VRST 1024
#define MAX_VRTM 16
#define MAX_VRUI 64
#define MAX_VRUL 4
#define MAX_VRUS 2

#define DEF_OUR_IMPVERNAME "VisunExceed_V1.0"
#define DEF_OUR_IMPCLSUID "1.2.803.116.3"

typedef enum {
	DCM_SC_ROLE_DEFAULT,
	DCM_SC_ROLE_SCU,
	DCM_SC_ROLE_SCP,
	DCM_SC_ROLE_SCUSCP
}   DCM_SC_ROLE;

typedef enum {
	PHOTMETR_INTEPR_NONE,
	PHOTMETR_INTEPR_MONOCHROME1,
	PHOTMETR_INTEPR_MONOCHROME2,
	PHOTMETR_INTEPR_PALETTECOLOR,
	PHOTMETR_INTEPR_RGB,
	PHOTMETR_INTEPR_HSV,
	PHOTMETR_INTEPR_ARGB,
	PHOTMETR_INTEPR_CMYK,
	PHOTMETR_INTEPR_YBR_FULL,
	PHOTMETR_INTEPR_YBR_FULL_422,
	PHOTMETR_INTEPR_YBR_PARTIAL_422

}   PHOTMETR_INTEPR;

struct DICOM_DECL_SPEC DCMImgPixInfo {
	int				nNumbofFram;		//0028,0008) Number of Frames
	unsigned short	sSampperPix;		//Samples per Pixel (0028,0002)
	PHOTMETR_INTEPR ePhotmetrIntepr;	//Photometric Interpretation (0028,0004)
	unsigned short  sRows;				//Rows (0028,0010)
	unsigned short  sColumns;			//Columns (0028,0011)
	unsigned short  sBitsAllocated;		//Bits Allocated (0028,0100)
	unsigned short  sBitsStored;		//Bits Stored (0028,0101)
	unsigned short  sHighBit;			//High Bit (0028,0102)
	bool			bPixRepr;			//Pixel Representation (0028,0103)
	bool			bPlanConf;			//Planar Configuration (0028,0006)
	int				nVPixAspRat;		//Pixel Aspect Ratio (0028,0034)
	int				nHPixAspRat;		//
	short			sSmallestImgPixValue;//Smallest Image Pixel Value (0028,0106)
	short			sLargestImgPixValue;//Largest Image Pixel Value (0028,0107)
	short			sRedPalColLUTDes[3];//Red Palette Color Lookup Table Descriptor	(0028,1101)
	short			sGreenPalColLUTDes[3];//Green Palette Color Lookup Table Descriptor(0028,1102)
	short			sBluePalColLUTDes[3];//Blue Palette Color Lookup Table	Descriptor(0028,1103)
	WORD *			pRedPalColLUTData;	//Red Palette Color Lookup Table Data (0028,1201)
	WORD *			pGreenPalColLUTData;//Green Palette Color Lookup Table Data	(0028,1202)
	WORD *			pBluePalColLUTData;	//Blue Palette Color Lookup Table Data (0028,1203)
	int				nXferSyn;

	DCMImgPixInfo();
	void clear();
};

struct DICOM_DECL_SPEC DCMUSRegions {				//Sequence of Ultrasound Regions (0018,6011)
	unsigned long	lRegionLocaMinx0;			//Region Location Min x0 (0018,6018)
	unsigned long	lRegionLocaMiny0;			//Region Location Min y0 (0018,601A)
	unsigned long	lRegionLocaMaxx1;			//Region Location Max x1 (0018,601C)
	unsigned long	lRegionLocaMaxy1;			//Region Location Max y1 (0018,601E)
	unsigned short  sPhysicalUnitsX;			//Physical Units X Direction (0018,6024)
	unsigned short  sPhysicalUnitsY;			//Physical Units Y Direction (0018,6026)
	double			dPhysicalDeltaX;			//Physical Delta X (0018,602C)
	double			dPhysicalDeltaY;			//Physical Delta Y (0018,602E)
	long			lRefPixelx0;				//Reference Pixel x0 (0018,6020)
	long			lRefPixely0;				//Reference Pixel y0 (0018,6022)
	double			dRefPixelPhysX;				//Ref. Pixel Physical Value X (0018,6028)
	double			dRefPixelPhysY;				//Ref. Pixel Physical Value Y (0018,602A)
	unsigned short	sRegSpatFormat;				//Region Spatial Format (0018,6012)
	unsigned short	sRegDataType;				//Region Data Type (0018,6014)
	unsigned long	lRegionFlags;				//Region Flags (0018,6016)
	unsigned short	sPixelCompOrgan;			//Pixel Component Organization (0018,6044)
	unsigned long	lPixelCompMask;				//Pixel Component Mask (0018,6046)
	unsigned long	lPixelCompStart;			//Pixel Component Range Start (0018,6048)
	unsigned long	lPixelCompStop;				//Pixel Component Range Stop (0018,604A)
	unsigned short	sPixelCompPhysUnits;		//Pixel Component Physical	Units		(0018,604C)
	unsigned short	sPixelCompDataType;			//Pixel Component Data Type (0018,604E)
	unsigned long	lNumofTabBreakPoints;		//Number of Table Break Points (0018,6050)
	unsigned long * pTabofXBreakPoints;			//Table of X Break Points (0018,6052)
	double *		pTableofYBreakPoints;		//Table of Y Break Points (0018,6054)
	unsigned long	lNumofTabEntries;			//Number of Table Entries (0018,6056)
	unsigned long *	pTabofPixelValues;			//Table of Pixel Values (0018,6058)
	float *			pTabofParamValues;			//Table of Parameter Values (0018,605A)
	unsigned long	lTranducerFreq;				//Tranducer Frequency (0018,6030)
	unsigned long	lPulseRepFreq;				//Pulse Repetition Frequency (0018,6032)
	double			dDoppCorrAngle;				//Doppler Correction Angle (0018,6034)
	double			dSteeringAngle;				//Steering Angle (0018,6036)
	unsigned long	lDoppSampVolXPos;			//Doppler Sample Volume X Position (0018,6039)
	unsigned long	lDoppSampVolYPos;			//Doppler Sample Volume Y Position (0018,603B)
	unsigned long	lTMLinePosx0;				//TM-Line Position x0 (0018,603D)
	unsigned long	lTMLinePosy0;				//TM-Line Position y0 (0018,603F)
	unsigned long	lTMLinePosx1;				//TM-Line Position x1 (0018,6041)
	unsigned long	lTMLinePosy1;				//TM-Line Position y1 (0018,6043)
	DCMUSRegions * pNext;
};

struct DICOM_DECL_SPEC DCMOverlayInfo {
	int				nRows;		// (60xx,0010) 1 Number of Rows in Overlay.
	int				nColumns;	// (60xx,0011) 1 Number of Columns in Overlay.
	char			szType[MAX_VRCS+1];// (60xx,0040) 1 Indicates whether this overlay representsa region of interest or other graphics. Enumerated Values: G = Graphics R = ROI.
	int				nOriginRows;	// (60xx,0050) 1 Location of first overlay point with respect to pixels in the image, given as row\column. The upper left pixel of the image has the coordinate 1\1. Column values greater than 1 indicate the overlay plane origin is to the right of the image origin. Row values greater than 1 indicate the overlay plane origin is below the image origin. Values less than 1 indicate the overlay plane origin is above or to the left of the image origin. Note: Values of 0\0 indicate that the overlay pixels start 1 row above and one column to the left of the image pixels.
	int				nOriginColumns;	// (60xx,0050) 1 Location of first overlay point with respect to pixels in the image, given as row\column. The upper left pixel of the image has the coordinate 1\1. Column values greater than 1 indicate the overlay plane origin is to the right of the image origin. Row values greater than 1 indicate the overlay plane origin is below the image origin. Values less than 1 indicate the overlay plane origin is above or to the left of the image origin. Note: Values of 0\0 indicate that the overlay pixels start 1 row above and one column to the left of the image pixels.
	int				nBitsAllocated; //(60xx,0100) 1 Number of Bits Allocated in the Overlay. If the overlay data are embedded in the Image Pixel Data (7FE0,0010), the value of this Attribute shall be the same as Bits Allocated (0028,0100). If the overlay data are stored in the Overlay Data (60xx,3000) Attribute, the value of this Attribute shall be 1.
	int				nBitPosition; //(60xx,0102) 1 Bit in which Overlay is stored. See PS 3.5 for further explanation. If the overlay data are stored in the Overlay Data (60xx,3000) Attribute, the value of this Attribute shall be 0.
	char *			pData; //(60xx,3000) 1C Overlay pixel data. The order of pixels sent for each overlay is left to right, top to bottom, i.e., the upper left pixel is sent first followed by the remainder of the first row , followed by the first pixel of the 2nd row, then the remainder of the 2nd row and so on. Overlay data shall be contained in this Attribute or imbedded with the image pixel data in Group 7FE0. Required if overlay data are in this Group. See C.9.2.1.1 for further explanation.
	char			szDescription[MAX_VRLO+1]; //(60xx,0022) 3 User-defined comments about the overlay.
	char			szSubtype[MAX_VRLO+1]; // (60xx,0045) 3 Defined term which identifies the intended purpose of the Overlay Type. See C.9.2.1.3 for further explanation.
	char			szLabel[MAX_VRLO+1]; // (60xx,1500) 3 A user defined text string which may be used to label or name this overlay.
	int				nROIArea;				// (60xx,1301) 3 Number of pixels in ROI area. See C.9.2.1.2 for further explanation.
	double			dROIMean; // (60xx,1302) 3 ROI Mean. See C.9.2.1.2 for further explanation.
	double			dROIStandardDeviation; // (60xx,1303) 3 ROI standard deviation.See C.9.2.1.2 for further explanation.
};

struct DICOM_DECL_SPEC DCMModLUTInfo {
	double dRescIntercept;				//Rescale Intercept (0028,1052)
	double dRescSlope;					//Rescale Slope (0028,1053)
	BOOL bLUT;							//When the transformation is linear, the Modality LUT is described by
										//Rescale Slope (0028,1053) and Rescale Intercept (0028,1052). When the transformation is non-linear,
										//the Modality LUT is described by Modality LUT Sequence (0028,3000).
	BOOL bRescale;						//
	unsigned short sLUTDescriptor[3];	//LUT Descriptor (0028,3002)
	unsigned short * pLUTData;			//LUT Data (0028,3006)
	char szRescOrLUTType[MAX_VRLO+1];	//Modality LUT Type (0028,3004)
	char szLUTExplanation[MAX_VRLO+1];	//LUT Explanation (0028,3003)

	BOOL bRelease;						//added by shape 20070205 for 控制是否释放pLUTData内存
	DCMModLUTInfo();
	void clear();
	const DCMModLUTInfo & operator=(const DCMModLUTInfo &dcmModLUTInfo);
};

struct DICOM_DECL_SPEC DCMCineInfo {
	int nNumbofFram;			//Number of Frames (0028,0008)
	DWORD * pFramIncrPnt;		//Frame Increment Pointer (0028,0009)
	short sPrefPlaybackSeq;		//Preferred Playback Sequencing (0018,1244)
	double dFrameTime;			//Frame Time (0018,1063)
	char * pFrameTimeVect;		//Frame Time Vector (0018,1065)
	int nStartTrim;				//Start Trim (0008,2142)
	int nStopTrim;				//Stop Trim (0008,2143)
	int nRecomDispFramRat;		//Recommended Display Frame Rate (0008,2144)
	int nCineRate;				//Cine Rate (0018,0040)
	double dFrameDelay;			//Frame Delay (0018,1066)
	double dImgTrigDelay;		//Image Trigger Delay (0018,1067)
	double dEffecDuration;		//Effective Duration (0018,0072)
	int nActFramDuration;		//Actual Frame Duration (0018,1242)

	BOOL bRelease;						//added by shape 20070205 for 控制是否释放pLUTData内存
	DCMCineInfo();
	void clear();
	const DCMCineInfo & operator=(const DCMCineInfo &dcmCineInfo);
};

/************************************************************************/
/*
*      Object
*/
/************************************************************************/
enum DCM_OBJ{
	DCM_OBJ_None,/*defaultinitializerorerrcondition*/
	DCM_OBJ_Assoc=0X1001,		//edited by shape 20070706 for 和DCM_OBJ_CStoreRq冲突 1 -> 0X1001
	DCM_OBJ_Elemlist,
	DCM_OBJ_Elem,
	DCM_OBJ_CEchoRq=0x0030,
	DCM_OBJ_CEchoRsp=0x8030,
	DCM_OBJ_CFindRq=0x0020,
	DCM_OBJ_CFindRsp=0x8020,
	DCM_OBJ_CCancelRq=0x0fff,
	DCM_OBJ_CGetRq=0x0010,
	DCM_OBJ_CGetRsp=0x8010,
	DCM_OBJ_CMoveRq=0x0021,
	DCM_OBJ_CMoveRsp=0x8021,
	DCM_OBJ_CStoreRq=0x0001,
	DCM_OBJ_CStoreRsp=0x8001,
	DCM_OBJ_NActionRq=0x0130,
	DCM_OBJ_NActionRsp=0x8130,
	DCM_OBJ_NCreateRq=0x0140,
	DCM_OBJ_NCreateRsp=0x8140,
	DCM_OBJ_NDeleteRq=0x0150,
	DCM_OBJ_NDeleteRsp=0x8150,
	DCM_OBJ_NEventRq=0x0100,
	DCM_OBJ_NEventRsp=0x8100,
	DCM_OBJ_NGetRq=0x0110,
	DCM_OBJ_NGetRsp=0x8110,
	DCM_OBJ_NSetRq=0x0120,
	DCM_OBJ_NSetRsp=0x8120,
	DCM_OBJ_NetWork,
	DCM_OBJ_DicomDir,
};

typedef DWORD DCMElemTag;
class DICOM_DECL_SPEC DCMObj
{
public:
	bool m_bAutoRelease;
    USHORT type;    /* identifies class object type */
    void *ptr;      /* pointer to class object */

	DCMObj(bool bAutoRelease=false);
	virtual ~DCMObj();
};
typedef DCMObj *DCMObjPtr;

typedef struct PreContextItemRQ_ {	//A structure used for Presentation Context
	BYTE btPreContextID;
	int nSop;	//SOP class, e.g. DCM_SOP_CR_STORAGE
	int nTranSyntax[DCM_MAX_ALLOWED_TS]; //Aarry of Transfer Syntax, e.g. DCM_TS_IMPLICIT_VR_LE.
	int nValidTS;
	DCM_SC_ROLE proSCRole;
} PreContextItemRQ;

typedef struct PreContextItemAC_ {	//A structure used for Presentation Context
	BYTE btPreContextID;
	BYTE btResultReason; // used for AC.
	int nSop;	//SOP class, e.g. DCM_SOP_CR_STORAGE
	int nTranSyntax; //Aarry of Transfer Syntax, e.g. DCM_TS_IMPLICIT_VR_LE.
	BOOL bValid;
	DCM_SC_ROLE accSCRole;
} PreContextItemAC;

typedef struct SCPSCURoleSelection_ {	//A structure used for SCP SCU Role Selection
	char			szSOPClassUID[MAX_VRUI+1];
	BYTE			bySCURole;
	BYTE			bySCPRole;
} SCPSCURoleSelection;

typedef struct SOPClassExtNegotiation_ {	//A structure used for
	char			szSOPClassUID[MAX_VRUI+1];
	BYTE			byServClassAppinformation[100];
	unsigned short	sServClsAppInfoLen;
} SOPClassExtNegotiation;

typedef struct RelaGenSOPClsIdentification_ {	//A structure used for
	char			szSOPClassUID[MAX_VRUI+1];
} RelaGenSOPClsIdentification;

typedef struct SOPClassComExtNegotiation_ {	//A structure used for
	char			szSOPClassUID[MAX_VRUI+1];
	char			szServiceClassUID[MAX_VRUI+1];
	RelaGenSOPClsIdentification RelaGenSOPClsIdentifications[100];
	int nValidRelaGenSOPClsIdentification;
} SOPClassComExtNegotiation;

struct DCMScuInfo {
	unsigned short sProtocolversion;
    char szCallingAEtitle[MAX_VRAE+1];/* calling AE title */
    char szCalledAEtitle[MAX_VRAE+1];/* called AE title */
    PreContextItemRQ * pPreContextItemRQs;
	int nValidPC;
    char szAppContextName[MAX_VRUI+1];   /* application context name */
    char szImpClassUid[MAX_VRUI+1]; /* implementation class uid */
    char szimpVerName[MAX_VRAE+1];  /* implementation version name */
    unsigned long lMaxPduLenth;                /* maximum pdu length */
	unsigned short	sMaxNumOperInvoked;	/*0xffff for invalidate*/
	unsigned short	sMaxNumOperPerformed; /*0xffff for invalidate*/
//	SCPSCURoleSelection * pSCPSCURoleSelections;
// 	int nValidRoleSelect;
	SOPClassExtNegotiation * pSOPClsExtNegotiations;
 	int nValidClsExtNegotiation;
	SOPClassComExtNegotiation * pSOPClsComExtNegotiations;
  	int nValidClsComExtNegotiation;
	int nTimeout;                 /* timeout in seconds */

	DCMScuInfo()
	{
		memset(this, 0, sizeof(DCMScuInfo));
	}
};

typedef struct DCMScpInfo_ {
	unsigned short sProtocolversion;
    char szCallingAEtitle[MAX_VRAE+1];/* calling AE title */
    char szCalledAEtitle[MAX_VRAE+1];/* called AE title */
    char szAppContextName[MAX_VRUI+1];   /* application context name */
    PreContextItemAC * pPreContextItemACs;
	int nValidPC;
    char szImpClassUid[MAX_VRUI+1]; /* implementation class uid */
    char szimpVerName[MAX_VRAE+1];  /* implementation version name */
    unsigned long lMaxPduLenth;                /* maximum pdu length */
	unsigned short	sMaxNumOperInvoked;	/*0xffff for invalidate*/
	unsigned short	sMaxNumOperPerformed; /*0xffff for invalidate*/
//	SCPSCURoleSelection * pSCPSCURoleSelections;
// 	int nValidRoleSelect;
	SOPClassExtNegotiation * pSOPClsExtNegotiations;
 	int nValidClsExtNegotiation;
}   DCMScpInfo;

typedef enum {
	DCMPRIT_MEDIUM,
	DCMPRIT_HIGH,
	DCMPRIT_LOW,
	DCMPRIT_NONE
}   DCMPRIT;

typedef struct AttribIdList_ {
	DWORD dwAttribIdList[DCM_MAX_ALLOWED_AT];
	unsigned long lVMCount;
} AttribIdList;

typedef struct DCMDIMSERQInfo_ {
	int nAffecSOP;
	char szAffecSOPInstUID[MAX_VRUI+1];
	int nRequeSOP;
	char szRequeSOPInstUID[MAX_VRUI+1];
	char szMoveOrigAETitle[MAX_VRAE+1];
	char szMoveDestination[MAX_VRAE+1];
	AttribIdList aAttribIdList;
	BOOL bDataSetType;
	unsigned short sMessID;
	unsigned short sMoveOrigMessID;
	unsigned short sMessIDBeRespTo;
	unsigned short sEventTypeID;
	unsigned short sActionTypeID;
	DCMPRIT ePriority;
}   DCMDIMSERQInfo;

typedef struct DCMDIMSERSPInfo_ {
	int nTranSyntax;
	int nAffecSOP;
	BOOL bDataSetType;
	char szAffecSOPInstUID[MAX_VRUI+1];
	unsigned short sStatus;
	unsigned short sNumofRemaSuboper;
	unsigned short sNumofCompSuboper;
	unsigned short sNumofFailSuboper;
	unsigned short sNumofWarnSuboper;

	unsigned short sMessID;
	unsigned short sMessIDBeRespTo;
	unsigned short sEventTypeID;
	unsigned short sActionTypeID;
}   DCMDIMSERSPInfo;

#define JPG_BIT_AUTO		0
#define JPG_BIT_FORCE_8		1
#define JPG_BIT_FORCE_12	2
#define JPG_BIT_FORCE_16	3

#define JPG_COLOR_YBR		0
#define JPG_COLOR_RGB		1
#define JPG_MONOCHROME		2

#define JPG_SAMPLE_444		0
#define JPG_SAMPLE_422		1
#define JPG_NONSTD_422_FULL	2
#define JPG_NONSTD_411_FULL	3
#define JPG_NONSTD_411		4

#define JPG_SCALING_PIXEL	0
#define JPG_SCALING_RANGE	1

#define JPG_RESCLALE_IDENTITY	0
#define JPG_RESCLALE_MAP		1

#define JPG_CLASS_DEFUALT	0
#define JPG_CLASS_SC		1

#define JPG_UID_DEFUALT		0
#define JPG_UID_ALWAYS		1
#define JPG_UID_NEVER		2

#define JPG_HUFFMAN_OPTIMIZE	1
#define JPG_HUFFMAN_STANDARD	0

typedef enum {
	COMP_TYPE_NONE,
	COMP_TYPE_JPEG,
	COMP_TYPE_RLE
}COMP_TYPE;

typedef struct DCMCJpgOpt_ {
	int nTranSyntax;
	int nSelValue;	//lossless JPEG representation options: --selection-value      [sv]: integer (1..7, default: 6) use selection value only with --encode-lossless
	int nPntTranf;	//lossless JPEG representation options: point-transform      [pt]: integer (0..15, default: 0) use point transform
	int nQuality;	//lossy JPEG representation options: --quality              [q]: integer (0..100, default: 90) use quality factor q
	int nHuffOptim;	//other JPEG options: --huffman-optimize     1: optimize huffman tables (default) 0:use standard huffman tables if 8 bits/sample
	int nSmooth;	//other JPEG options: --smooth               [s]: integer (0..100, default: 0)
	//	int nBitPerSampOpt;	// compressed bits per sample options:  0: --bits-auto            choose bits/sample automatically (default)
	//										1: --bits-force-8         force 8 bits/sample
	//										2: --bits-force-12        force 12 bits/sample (not with baseline)
	//										3: --bits-force-16        force 16 bits/sample (lossless only)

	int nColorSpaceOpt;	//compression color space conversion options:   0: --color-ybr            use YCbCr for color images if lossy (default)
	//												1: --color-rgb            use RGB for color images if lossy
	//												2: --monochrome           convert color images to monochrome
	int nCompSubsampOpt;//standard YCbCr component subsampling options:		0: --sample-444           4:4:4 sampling with YBR_FULL (default)
	//													1: --sample-422           4:2:2 subsampling with YBR_FULL_422
	//non-standard YCbCr component subsampling options:	2: --nonstd-422-full      4:2:2 subsampling with YBR_FULL
	//													3: --nonstd-411-full      4:1:1 subsampling with YBR_FULL
	//													4: --nonstd-411           4:1:1 subsampling with YBR_FULL_422
	//	int nScalMono;		//pixel scaling for monochrome images:	0: --scaling-pixel        scale using min/max pixel value (default)
	//										1: --scaling-range        scale using min/max range
	//	int nRescalOpt;		//rescale slope/intercept encoding for monochrome (--no-windowing): 0: --rescale-identity     encode identity modality rescale (default) Never used for CT images
	//																	1: --rescale-map          use modality rescale to scale pixel range Never used for XA/RF/XA Biplane images

	int nSopClsUIDOpt;	// SOP Class UID options:	0: --class-default        keep SOP Class UID (default)
	//							1: --class-sc             convert to Secondary Capture Image(implies --uid-always)
	int nSopInstUIDOpt;	// SOP Instance UID options:0: --uid-default          assign new UID if lossy compression (default)
	//							1: --uid-always           always assign new UID
	//							2: --uid-never            never assign new UID
	char szSopInstUID[MAX_VRUI + 1];// SOP Instance UID
}	DCMCJpgOpt;

#define JPG_COLORCONV_PHOTOMETRIC		0
#define JPG_COLORCONV_LOSSY				1
#define JPG_COLORCONV_ALWAYS			2
#define JPG_COLORCONV_NEVER				3

#define JPG_PLANARCONF_AUTO				0
#define JPG_PLANARCONF_BYPIXEL			1
#define JPG_PLANARCONF_BYPLANE			2

typedef struct DCMDJpgOpt_ {
	int nColorSpaceConv;	//  color space conversion options:
	// 0   --conv-photometric     convert if YCbCr photom. interpr. (default)
	// 1   --conv-lossy           convert YCbCr to RGB if lossy JPEG
	// 2   --conv-always          always convert YCbCr to RGB
	// 3   --conv-never           never convert color space
	int nPlanarConfig;		// planar configuration options:
	// 0   --planar-auto          automatically determine planar configuration
	//							  from SOP class and color space (default)
	// 1   --color-by-pixel       always store color-by-pixel
	// 2   --color-by-plane       always store color-by-plane
	int nSopInstUIDOpt;		// SOP Instance UID options:
	//	0   --uid-default          keep same SOP Instance UID (default)
	//  1 --uid-always           always assign new UID

	char szSopInstUID[MAX_VRUI + 1];// SOP Instance UID

}DCMDJpgOpt;

void DICOM_DECL_SPEC DCMRegisterCodecs(DCMCJpgOpt *pDCMCJpgOpt=NULL);
void DICOM_DECL_SPEC DCMCleanUpCodecs();
BOOL DICOM_DECL_SPEC DCMIsString(WORD evr);

/***************************************************************************/
/*
*      DIMSE
*/
/***************************************************************************/
BOOL DICOM_DECL_SPEC DCMDIMSECreate (DCMObjPtr pDIMSE, DCMObjPtr pDCMAssoc, DCM_OBJ ObjType, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMDIMSEDestroy (DCMObjPtr pDIMSE, char* szErrorMsg=NULL);
BOOL DICOM_DECL_SPEC DCMDIMSERQSend (DCMObjPtr pDIMSE, DCMObjPtr elemList, DCMDIMSERQInfo * pDCMDIMSERQInfo, PreContextItemAC * pPreContextItemAC, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMDIMSERQReceive (DCMObjPtr pDIMSE, DCMDIMSERQInfo * pDCMDIMSERQInfo, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMDIMSERSPSend (DCMObjPtr pDIMSE, DCMObjPtr elemList, DCMDIMSERSPInfo * pDCMDIMSERSPInfo, PreContextItemAC * pPreContextItemAC, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMDIMSERSPReceive (DCMObjPtr pDIMSE, DCMDIMSERSPInfo * pDCMDIMSERSPInfo, char* szErrorMsg);

/***************************************************************************/
/*
*      DICOM COMM
*/
/***************************************************************************/
BOOL DICOM_DECL_SPEC DCMCreateAssocSCU(DCMObjPtr pDCMAssoc,DCMScuInfo * pScuInfo, char * szIP, int nPort, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMDestroyAssoc(DCMObjPtr pDCMAssoc, char* szErrorMsg=NULL);
//BOOL DICOM_DECL_SPEC DCMCreateAssocSCP(DCMObjPtr pDCMAssoc, int nPort, int nTimeout, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMReceiveAssociation(DCMObjPtr pDCMAssoc, DCMObjPtr pDCMNetWork, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMCreateNetWork(DCMObjPtr pDCMNetWork, int nPort, int nTimeout, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMCloseNetWork(DCMObjPtr pDCMNetWork, char* szErrorMsg=NULL);
BOOL DICOM_DECL_SPEC DCMAssocReadElemList(DCMObjPtr pDIMSE, DCMObjPtr elemList, PreContextItemAC * pPreContextItemAC, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMReadAssocRQ (DCMObjPtr pDCMAssoc,DCMScuInfo * pDCMScuInfo, char* szErrorMsg);
//BOOL DICOM_DECL_SPEC DCMRelConnection (DCMObjPtr pDCMAssoc, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMAbortConnection (DCMObjPtr pDCMAssoc,int nSource,int nReason, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMAcceptConnection (DCMObjPtr pDCMAssoc,DCMScpInfo * pDCMScpInfo, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMRejectConnection (DCMObjPtr pDCMAssoc,int nResult,int nSource,int nReason, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMGetACPreContexts (DCMObjPtr pDCMAssoc,PreContextItemAC ** pPreContextItemAC,unsigned int * pSize, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMGetACSCPInfo (DCMObjPtr pDCMAssoc,DCMScpInfo * pDCMScpInfo, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMAssocGetTimeout (DCMObjPtr pDCMAssoc,int * pTimeout, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMAssocSetTimeout (DCMObjPtr pDCMAssoc,int nTimeout, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMClearPreContextItemACs(PreContextItemAC ** pPreContextItemAC, unsigned int nSize, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMClearSCUInfo(DCMScuInfo * pScuInfo, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMClearSCPInfo(DCMScpInfo * pScpInfo, char* szErrorMsg);

/***************************************************************************/
/*
 *      Socket
 */
/***************************************************************************/
//BOOL DICOM_DECL_SPEC DCMSockCreate (DCMObjPtr sock, char* szErrorMsg);
//BOOL DICOM_DECL_SPEC DCMSockDestroy (DCMObjPtr sock, char* szErrorMsg);
//BOOL DICOM_DECL_SPEC DCMSockListen (DCMObjPtr sock, char * szPort, char* szErrorMsg);
//BOOL DICOM_DECL_SPEC DCMSockAccept (DCMObjPtr sock, char* szErrorMsg, struct sockaddr_in * pSockAddr = NULL);
//BOOL DICOM_DECL_SPEC DCMSockHandle (DCMObjPtr sock, int * pSocketfd, char* szErrorMsg);
//BOOL DICOM_DECL_SPEC DCMSockReset (DCMObjPtr sock, char* szErrorMsg);
//BOOL DICOM_DECL_SPEC DCMSockClose (DCMObjPtr sock, char* szErrorMsg);

/***************************************************************************/
/*
*      DLL Import/Export
*/
/***************************************************************************/
BOOL DICOM_DECL_SPEC DCMSymboToUID(int nNumber,  char * pUID);
BOOL DICOM_DECL_SPEC DCMUIDToSymbo(int* nNumber, const char * pUID);
//BOOL DICOM_DECL_SPEC DCMGetErrorMessage(int nError, char* szMessage);

/***************************************************************************/
/*
*      DICOM Dictionary
*/
/***************************************************************************/
//BOOL DICOM_DECL_SPEC DCMLoadDCMDict(char * filename, BOOL bCarryDescriptions = FALSE);
BOOL DICOM_DECL_SPEC DCMDictGetNameAndVR(DCMElemTag tag, char * szName, int iBufLen, WORD *wVR, char* szErrorMsg );

/***************************************************************************/
/*
 *      DICOM File
 */
/***************************************************************************/
BOOL DICOM_DECL_SPEC DCMReadFile ( char* szFilename,	DCMObjPtr elemList, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMSaveFile ( char* szFilename,	DCMObjPtr elemList, char* szErrorMsg, int nFormat = DCM_TS_UNKNOWN, char * szAETitle = NULL, int nCompType = COMP_TYPE_NONE, DCMCJpgOpt * pDCMCJpgOpt = NULL);
BOOL DICOM_DECL_SPEC DCMSaveBMPFile ( char* szFilename,	DCMObjPtr elemList, char* szErrorMsg, unsigned long nIndex = 0);

/***************************************************************************/
/*
 *      DICOMDIR
 */
/***************************************************************************/
BOOL DICOM_DECL_SPEC DCMDICOMDIRCreate(DCMObjPtr pDICOMDIR, const char* szDICOMDirFileName, const char* szFileSetID, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMDICOMDIRAddFile(DCMObjPtr pDICOMDIR, const char* szFilePathName, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMDICOMDIRSave(DCMObjPtr pDICOMDIR, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMDICOMDIRClear(DCMObjPtr pDICOMDIR, char* szErrorMsg=NULL);
BOOL DICOM_DECL_SPEC DCMCreateDICOMDIRFromTree (DCMObjPtr elemList, char *szDICOMDIRRoot, char *szFileSetID, char *szFileSetUID, int nXferSyn, char* szErrorMsg, char * szAETitle = NULL, int nNativeEndian = 0);

/***************************************************************************/
/*
*      Data Element
*/
/***************************************************************************/
BOOL DICOM_DECL_SPEC DCMElemCreate (DCMObjPtr elem, DCMElemTag tag, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemDestroy (DCMObjPtr elem, char* szErrorMsg=NULL);
BOOL DICOM_DECL_SPEC DCMElemTagGet (DCMObjPtr elem, DCMElemTag *tag, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemLenGet (DCMObjPtr elem, ULONG *lLen, int nXferSyn, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemVRGet (DCMObjPtr elem, WORD *wVR, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemNameGet (DCMObjPtr elem, char* szName, int iBufLen, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemValueAddInt (DCMObjPtr elem, int nValue, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemValueAddFloat (DCMObjPtr elem, double dValue, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemValueAddString (DCMObjPtr elem, char *szValue, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemValueAddRaw (DCMObjPtr elem, void *pBuf, int nBufLen, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemValueAddPixel (DCMObjPtr elem, void * pBuf, unsigned long ulBufLen, DCMImgPixInfo * pDCMImgPixInfo, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemValueGetInt (DCMObjPtr elem, int *nValue, int nIndex, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemValueGetFloat (DCMObjPtr elem, double * dValue, int nIndex, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemValueGetString (DCMObjPtr elem, char *strValue, int nStrLen, int nIndex, char* szErrorMsg);
//BOOL DICOM_DECL_SPEC DCMElemValueGetPixel (DCMObjPtr elem, void *&pBuf, int nBufLen, DCMImgPixInfo * pDCMImgPixInfo, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemValueGetRaw (DCMObjPtr elem, void *pBuf, int iBufLen, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemValueCountGet (DCMObjPtr elem, int *nCount, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemValuesRemoveAll (DCMObjPtr elem, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemIsSQ (DCMObjPtr elem, BOOL * pIsSQ, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemSQItemCount (DCMObjPtr elem, int * pCount, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemSQGet (DCMObjPtr elem, int nIndex, DCMObjPtr elemlist, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemAddSQ (DCMObjPtr elem, DCMObjPtr elemlistSQ, char* szErrorMsg);

/***************************************************************************/
/*
 *      Data Element List
 */
/***************************************************************************/
BOOL DICOM_DECL_SPEC DCMElemListCreate (DCMObjPtr elemList, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemListDestroy (DCMObjPtr elemList, char* szErrorMsg=NULL);
BOOL DICOM_DECL_SPEC DCMElemListPush (DCMObjPtr elemList, DCMObjPtr elem, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemListPop (DCMObjPtr elemList, DCMObjPtr elem, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemListEndianSwitch (DCMObjPtr elemList, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemListElemAdd (DCMObjPtr elemList, DCMObjPtr elem, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemListLenGet (DCMObjPtr elemList, ULONG *lLen, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemListElemGet(DCMObjPtr elemList, DCMElemTag tag, DCMObjPtr elem, char* szErrorMsg, BOOL bSearchToSub = FALSE);
BOOL DICOM_DECL_SPEC DCMElemListCopy(DCMObjPtr elemListDes,DCMObjPtr elemListSrc, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemListValueGetString (DCMObjPtr elemList,DCMElemTag tag,char *szValue,int nStrLen,int nIndex, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemListValueGetInt (DCMObjPtr elemList, DCMElemTag tag, int * pValue, int nIndex, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemListValueGetFloat (DCMObjPtr elemList, DCMElemTag tag, double *pValue, int nIndex, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemListValueGetRaw (DCMObjPtr elemList, DCMElemTag tag, void *pBuf, int *pBufLen, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemListValueGetPixel (DCMObjPtr elemList, void *pBuf, unsigned long ulBufLen, unsigned long ulIndex, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemListElemAddSQ (DCMObjPtr elemList, DCMElemTag tag, DCMObjPtr elemListSQ, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemListElemGetSQ (DCMObjPtr elemList, DCMElemTag tag, int nIndex, DCMObjPtr elemListSQ, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemListElemGetSQItemCount (DCMObjPtr elemList, DCMElemTag tag, int* pCount, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemListGetUSRegions (DCMObjPtr elemList, DCMUSRegions ** pDCMUSRegions, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemListDestroyUSRegions (DCMUSRegions ** pDCMUSRegions, char* szErrorMsg=NULL);
BOOL DICOM_DECL_SPEC DCMElemListGetPixelInfo (DCMObjPtr elemList, DCMImgPixInfo * pDCMImgPixInfo, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemListMaxMinPixel (DCMObjPtr elemList, double& dMaxPixel, double& dMinPixel, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemListGetModLUTInfo (DCMObjPtr elemList, DCMModLUTInfo * pDCMModLUTInfo, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemListGetCineInfo (DCMObjPtr elemList, DCMCineInfo * pDCMCineInfo, char* szErrorMsg);
#define LUT_APPLY_RESCINTERCEPT							0	//Apply all Modality LUT thing
#define LUT_APPLY_RESCINTERCEPT_SLOPNOTEQUALONE			1	//Apply RescIntercept only when slop != 1
#define LUT_APPLY_RESCINTERCEPT_NO						2	//Do not apply RescIntercept
BOOL DICOM_DECL_SPEC DCMElemListModalityLUTApplyToPixel (DCMObjPtr elemList, char* szErrorMsg, int nApplyRescIntercept = LUT_APPLY_RESCINTERCEPT_SLOPNOTEQUALONE);
BOOL DICOM_DECL_SPEC DCMElemListGetOverlayInfo (DCMObjPtr elemList, DCMOverlayInfo * pDCMOverlayInfo, int nOverlayIndex, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemListOverlayEmbedToData (DCMObjPtr elemList, char* szErrorMsg);

/***************************************************************************/
/*
 *      Data Element List add-in
 */
/***************************************************************************/
BOOL DICOM_DECL_SPEC DCMElemListElemAdd (DCMObjPtr elemList, DCMElemTag tag, const char *pszValue, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemListElemAdd (DCMObjPtr elemList, DCMElemTag tag, int iValue, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemListElemAdd (DCMObjPtr elemList, DCMElemTag tag, double dblValue, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemListElemAdd (DCMObjPtr elemList, DCMElemTag tag, void *pBuf, int nBufLen, char* szErrorMsg);
BOOL DICOM_DECL_SPEC DCMElemListElemAdd (DCMObjPtr elemList, DCMElemTag tag, void * pBuf, unsigned long ulBufLen, DCMImgPixInfo * pDCMImgPixInfo, char* szErrorMsg);

#endif