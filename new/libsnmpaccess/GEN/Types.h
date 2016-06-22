#ifndef __TYPES_H__
#define __TYPES_H__

typedef enum
{
	CardMode_UNDEFINED = 0,
	CardMode_TRANS = 1,
	CardMode_TRANSE = 2,
	CardMode_TRANSW = 3,
	CardMode_TRANSHST = 4,
	CardMode_MUX = 5,
	CardMode_MUXE = 6,
	CardMode_MUXW = 7,
	CardMode_MUXHST = 8,
	CardMode_REGEN1WAY = 9,
	CardMode_REGEN2WAY = 10,
	CardMode_ADM = 12,
	CardMode_TRANSDUAL = 13,
	CardMode_MUXDUAL = 14,
	CardMode_NFIXED = 15,
	CardMode_CSELECT = 16,
	CardMode_DUALCLIENT = 17,
	CardMode_MAX,
} CardMode_t;

typedef enum
{
	Channel_UNDEFINED = 0,
	Channel_D01 = 1,
	Channel_D02 = 2,
	Channel_D03 = 3,
	Channel_D04 = 4,
	Channel_D05 = 5,
	Channel_D06 = 6,
	Channel_D07 = 7,
	Channel_D08 = 8,
	Channel_D09 = 9,
	Channel_D10 = 10,
	Channel_D11 = 11,
	Channel_D12 = 12,
	Channel_D13 = 13,
	Channel_D14 = 14,
	Channel_D15 = 15,
	Channel_D16 = 16,
	Channel_D17 = 17,
	Channel_D18 = 18,
	Channel_D19 = 19,
	Channel_D20 = 20,
	Channel_D21 = 21,
	Channel_D22 = 22,
	Channel_D23 = 23,
	Channel_D24 = 24,
	Channel_D25 = 25,
	Channel_D26 = 26,
	Channel_D27 = 27,
	Channel_D28 = 28,
	Channel_D29 = 29,
	Channel_D30 = 30,
	Channel_D31 = 31,
	Channel_D32 = 32,
	Channel_D33 = 33,
	Channel_D34 = 34,
	Channel_D35 = 35,
	Channel_D36 = 36,
	Channel_D37 = 37,
	Channel_D38 = 38,
	Channel_D39 = 39,
	Channel_D40 = 40,
	Channel_D41 = 41,
	Channel_D42 = 42,
	Channel_D43 = 43,
	Channel_D44 = 44,
	Channel_D45 = 45,
	Channel_D46 = 46,
	Channel_D47 = 47,
	Channel_D48 = 48,
	Channel_D49 = 49,
	Channel_D50 = 50,
	Channel_D51 = 51,
	Channel_D52 = 52,
	Channel_D53 = 53,
	Channel_D54 = 54,
	Channel_D55 = 55,
	Channel_D56 = 56,
	Channel_D57 = 57,
	Channel_D58 = 58,
	Channel_D59 = 59,
	Channel_D60 = 60,
	Channel_D61 = 61,
	Channel_D62 = 62,
	Channel_D63 = 63,
	Channel_D64 = 64,
	Channel_DC1 = 65,
	Channel_DC2 = 66,
	Channel_DC3 = 67,
	Channel_DC4 = 68,
	Channel_DC5 = 69,
	Channel_DC6 = 70,
	Channel_DC7 = 71,
	Channel_DC8 = 72,
	Channel_DL1 = 73,
	Channel_DL2 = 74,
	Channel_DL3 = 75,
	Channel_DL4 = 76,
	Channel_DL5 = 77,
	Channel_DL6 = 78,
	Channel_DL7 = 79,
	Channel_DL8 = 80,
	Channel_DC9 = 81,
	Channel_DL9 = 82,
	Channel_C1470 = 91,
	Channel_C1490 = 92,
	Channel_C1510 = 93,
	Channel_C1530 = 94,
	Channel_C1550 = 95,
	Channel_C1570 = 96,
	Channel_C1590 = 97,
	Channel_C1610 = 98,
	Channel_S1310 = 99,
	Channel_S1630 = 100,
	Channel_G850 = 101,
	Channel_G1310 = 102,
	Channel_G1550 = 103,
	Channel_NOTINGRID = 105,
	Channel_S1510 = 106,
	Channel_F19610 = 107,
	Channel_F19595 = 108,
	Channel_F19585 = 109,
	Channel_F19575 = 110,
	Channel_F19565 = 111,
	Channel_F19555 = 112,
	Channel_F19545 = 113,
	Channel_F19535 = 114,
	Channel_F19525 = 115,
	Channel_F19515 = 116,
	Channel_F19505 = 117,
	Channel_F19495 = 118,
	Channel_F19485 = 119,
	Channel_F19475 = 120,
	Channel_F19465 = 121,
	Channel_F19455 = 122,
	Channel_F19445 = 123,
	Channel_F19435 = 124,
	Channel_F19425 = 125,
	Channel_F19415 = 126,
	Channel_F19405 = 127,
	Channel_F19395 = 128,
	Channel_F19385 = 129,
	Channel_F19375 = 130,
	Channel_F19365 = 131,
	Channel_F19355 = 132,
	Channel_F19345 = 133,
	Channel_F19335 = 134,
	Channel_F19325 = 135,
	Channel_F19315 = 136,
	Channel_F19305 = 137,
	Channel_F19295 = 138,
	Channel_F19285 = 139,
	Channel_F19275 = 140,
	Channel_F19265 = 141,
	Channel_F19255 = 142,
	Channel_F19245 = 143,
	Channel_F19235 = 144,
	Channel_F19225 = 145,
	Channel_F19215 = 146,
	Channel_F19205 = 147,
	Channel_F19600 = 148,
	Channel_F19590 = 149,
	Channel_F19580 = 150,
	Channel_F19570 = 151,
	Channel_F19560 = 152,
	Channel_F19550 = 153,
	Channel_F19540 = 154,
	Channel_F19530 = 155,
	Channel_F19520 = 156,
	Channel_F19510 = 157,
	Channel_F19500 = 158,
	Channel_F19490 = 159,
	Channel_F19480 = 160,
	Channel_F19470 = 161,
	Channel_F19460 = 162,
	Channel_F19450 = 163,
	Channel_F19440 = 164,
	Channel_F19430 = 165,
	Channel_F19420 = 166,
	Channel_F19410 = 167,
	Channel_F19400 = 168,
	Channel_F19390 = 169,
	Channel_F19380 = 170,
	Channel_F19370 = 171,
	Channel_F19360 = 172,
	Channel_F19350 = 173,
	Channel_F19340 = 174,
	Channel_F19330 = 175,
	Channel_F19320 = 176,
	Channel_F19310 = 177,
	Channel_F19300 = 178,
	Channel_F19290 = 179,
	Channel_F19280 = 180,
	Channel_F19270 = 181,
	Channel_F19260 = 182,
	Channel_F19250 = 183,
	Channel_F19240 = 184,
	Channel_F19230 = 185,
	Channel_F19220 = 186,
	Channel_F19210 = 187,
	Channel_F19200 = 188,
	Channel_C1270 = 189,
	Channel_C1290 = 190,
	Channel_C1310 = 191,
	Channel_C1330 = 192,
	Channel_C1350 = 193,
	Channel_C1370 = 194,
	Channel_C1430 = 195,
	Channel_C1450 = 196,
	Channel_S1610 = 197,
	Channel_T1650 = 198,
	Channel_S1490 = 199,
	Channel_F19195 = 443,
	Channel_F19190 = 447,
	Channel_F19185 = 451,
	Channel_F19180 = 455,
	Channel_F19175 = 459,
	Channel_F19170 = 463,
	Channel_F19165 = 467,
	Channel_F19160 = 471,
	Channel_F19155 = 475,
	Channel_F19150 = 479,
	Channel_F19145 = 483,
	Channel_F19140 = 487,
	Channel_F19135 = 491,
	Channel_F19130 = 495,
	Channel_F19125 = 499,
	Channel_NOTDEFINED = 500,
	Channel_MAX,
} Channel_t;

typedef enum
{
	Band_UNDEFINED = 0,
	Band_BANDC = 1,
	Band_BANDL = 2,
	Band_BANDA = 3,
	Band_BANDB = 4,
	Band_BANDCI = 5,
	Band_BANDCANDCI = 6,
	Band_MAX,
} Band_t;

typedef enum
{
	FacilityType_UNDEFINED = 0,
	FacilityType_IFTYPEOTU1 = 1,
	FacilityType_IFTYPEOTU2 = 2,
	FacilityType_IFTYPE10GBE = 3,
	FacilityType_IFTYPEOC192 = 4,
	FacilityType_IFTYPEOC48 = 5,
	FacilityType_IFTYPESTM16 = 6,
	FacilityType_IFTYPESTM64 = 7,
	FacilityType_IFTYPE10GFC = 8,
	FacilityType_IFTYPEF1062 = 10,
	FacilityType_IFTYPEF1250 = 11,
	FacilityType_IFTYPEFC = 12,
	FacilityType_IFTYPEF125 = 13,
	FacilityType_IFTYPEF200 = 14,
	FacilityType_IFTYPEF9953 = 15,
	FacilityType_IFTYPEF10312 = 16,
	FacilityType_IFTYPEF10518 = 17,
	FacilityType_IFTYPEF2488 = 18,
	FacilityType_IFTYPEGFPF = 19,
	FacilityType_IFTYPEGFPT = 20,
	FacilityType_IFTYPEDCCL = 21,
	FacilityType_IFTYPEDCCS = 22,
	FacilityType_IFTYPEDCCP = 23,
	FacilityType_IFTYPEODU1 = 24,
	FacilityType_IFTYPEGCC0 = 25,
	FacilityType_IFTYPEGCC1 = 26,
	FacilityType_IFTYPEGCC2 = 27,
	FacilityType_IFTYPEOCH = 28,
	FacilityType_IFTYPEOM = 29,
	FacilityType_IFTYPEOT = 34,
	FacilityType_IFTYPEE10OR100BT = 36,
	FacilityType_IFTYPEE100FX = 37,
	FacilityType_IFTYPECL = 38,
	FacilityType_IFTYPE2GFC = 39,
	FacilityType_IFTYPE2GCL = 40,
	FacilityType_IFTYPE1GBE = 41,
	FacilityType_IFTYPEEOC = 42,
	FacilityType_IFTYPESWITCH = 43,
	FacilityType_IFTYPEPASSIVE = 44,
	FacilityType_IFTYPEF2500 = 45,
	FacilityType_IFTYPESC = 46,
	FacilityType_IFTYPEUCH = 47,
	FacilityType_IFTYPEF155 = 48,
	FacilityType_IFTYPEF622 = 49,
	FacilityType_IFTYPEF2125 = 50,
	FacilityType_IFTYPEF2666 = 51,
	FacilityType_IFTYPEF4250 = 52,
	FacilityType_IFTYPEF10709 = 54,
	FacilityType_IFTYPEF11095 = 55,
	FacilityType_IFTYPEF11318 = 56,
	FacilityType_IFTYPELS = 57,
	FacilityType_IFTYPE4GFC = 58,
	FacilityType_IFTYPEGCC0S = 59,
	FacilityType_IFTYPE2R = 60,
	FacilityType_IFTYPEPPPIP = 61,
	FacilityType_IFTYPELANIP = 62,
	FacilityType_IFTYPESERIAL = 64,
	FacilityType_IFTYPEMODEM = 65,
	FacilityType_IFTYPEADAPT = 66,
	FacilityType_IFTYPEADAPTD = 67,
	FacilityType_IFTYPEGBEFR = 69,
	FacilityType_IFTYPEVC4 = 70,
	FacilityType_IFTYPEVC3 = 71,
	FacilityType_IFTYPESTS1 = 72,
	FacilityType_IFTYPEEDFA = 73,
	FacilityType_IFTYPEEDFAMID = 74,
	FacilityType_IFTYPE10GDW = 75,
	FacilityType_IFTYPEOTU2LAN = 76,
	FacilityType_IFTYPEOTU1LAN = 77,
	FacilityType_IFTYPEOTU1FC = 78,
	FacilityType_IFTYPEOTU1FC2G = 79,
	FacilityType_IFTYPEF197 = 80,
	FacilityType_IFTYPETIF = 81,
	FacilityType_IFTYPESTS3C = 82,
	FacilityType_IFTYPEVS1 = 83,
	FacilityType_IFTYPE1GBETH = 84,
	FacilityType_IFTYPESTM1 = 85,
	FacilityType_IFTYPESTM4 = 86,
	FacilityType_IFTYPEOC3 = 87,
	FacilityType_IFTYPEOC12 = 88,
	FacilityType_IFTYPEOTU1STM1 = 89,
	FacilityType_IFTYPEOTU1STM4 = 90,
	FacilityType_IFTYPEF166 = 91,
	FacilityType_IFTYPEF666 = 92,
	FacilityType_IFTYPEI2C = 93,
	FacilityType_IFTYPELIFIP = 94,
	FacilityType_IFTYPESTS24C = 95,
	FacilityType_IFTYPESTS48C = 96,
	FacilityType_IFTYPELIFTE = 97,
	FacilityType_IFTYPERAMAN = 98,
	FacilityType_IFTYPEIPWHITELIST = 99,
	FacilityType_IFTYPEOSPFIP = 100,
	FacilityType_IFTYPEENCAPIP = 101,
	FacilityType_IFTYPEOTU3 = 102,
	FacilityType_IFTYPESTM256 = 103,
	FacilityType_IFTYPEOC768 = 104,
	FacilityType_IFTYPEF10664 = 105,
	FacilityType_IFTYPEODU2 = 106,
	FacilityType_IFTYPEF39813 = 107,
	FacilityType_IFTYPEVC4C8 = 108,
	FacilityType_IFTYPEVC4C16 = 109,
	FacilityType_IFTYPEF8500 = 110,
	FacilityType_IFTYPEPB = 111,
	FacilityType_IFTYPEPOLICER = 112,
	FacilityType_IFTYPEQUEUE = 113,
	FacilityType_IFTYPEFLOWPOINT = 114,
	FacilityType_IFTYPELAG = 115,
	FacilityType_IFTYPEELINEPPP = 116,
	FacilityType_IFTYPEETREE = 117,
	FacilityType_IFTYPEELINE = 118,
	FacilityType_IFTYPEELAN = 119,
	FacilityType_IFTYPECTRANS = 120,
	FacilityType_IFTYPEVS0 = 121,
	FacilityType_IFTYPETUG = 122,
	FacilityType_IFTYPEMD = 123,
	FacilityType_IFTYPEMA = 124,
	FacilityType_IFTYPEDOWNMEP = 125,
	FacilityType_IFTYPEFMEP = 126,
	FacilityType_IFTYPE8GFC = 127,
	FacilityType_IFTYPE10GIB = 128,
	FacilityType_IFTYPEF10000 = 129,
	FacilityType_IFTYPEOTU2PFC8 = 130,
	FacilityType_IFTYPEOTU2PIB = 131,
	FacilityType_IFTYPEOTU2PFC = 132,
	FacilityType_IFTYPEOTU2PLAN = 133,
	FacilityType_IFTYPEOTU2P = 134,
	FacilityType_IFTYPEUPMEP = 135,
	FacilityType_IFTYPEOTDRCH = 136,
	FacilityType_IFTYPEMANET = 137,
	FacilityType_IFTYPEMACOMP = 138,
	FacilityType_IFTYPEBRIDGE = 139,
	FacilityType_IFTYPE1000BASET = 140,
	FacilityType_IFTYPEOTU2E = 141,
	FacilityType_IFTYPEOTU1E = 142,
	FacilityType_IFTYPEOTU2F = 143,
	FacilityType_IFTYPEF11049 = 144,
	FacilityType_IFTYPELIFTENUM = 145,
	FacilityType_IFTYPELIFTEUNN = 146,
	FacilityType_IFTYPELIFPBNUM = 147,
	FacilityType_IFTYPELIFSUBUNN = 148,
	FacilityType_IFTYPE5GIB = 149,
	FacilityType_IFTYPEF5000 = 150,
	FacilityType_IFTYPEOTU2EETH = 151,
	FacilityType_IFTYPEODU0 = 152,
	FacilityType_IFTYPEFCU = 153,
	FacilityType_IFTYPEOTU2PS = 167,
	FacilityType_MAX,
} FacilityType_t;

typedef enum
{
	EntityAssignmentState_UNDEFINED = 0,
	EntityAssignmentState_ASSIGNED = 1,
	EntityAssignmentState_UNASSIGNED = 2,
	EntityAssignmentState_NOTASSIGNABLE = 3,
	EntityAssignmentState_MAX,
} EntityAssignmentState_t;

typedef enum
{
	PwrEqState_UNDEFINED = 0,
	PwrEqState_IDLE = 1,
	PwrEqState_FAIL = 2,
	PwrEqState_INPROGS = 3,
	PwrEqState_ABT = 4,
	PwrEqState_COMPL = 5,
	PwrEqState_MAX,
} PwrEqState_t;

typedef enum
{
	EqType_UNDEFINED = 0,
	EqType_EQPSH1HU = 1,
	EqType_EQPSH1HUDC = 2,
	EqType_EQPSH3HU = 3,
	EqType_EQPSH7HU = 4,
	EqType_EQPF2KSH5HU = 5,
	EqType_EQPF2KSH6HU = 6,
	EqType_EQPDCM = 7,
	EqType_EQPSH9HU = 8,
	EqType_EQPUNKNOWN = 19,
	EqType_EQPNCU = 20,
	EqType_EQPNCUTIF = 21,
	EqType_EQPSCU = 22,
	EqType_EQPSCUE = 23,
	EqType_EQPR6CU = 24,
	EqType_EQPPSU1HUAC = 25,
	EqType_EQPPSU7HUAC = 26,
	EqType_EQPPSU7HUDC = 27,
	EqType_EQPFCU7HU = 28,
	EqType_EQP2CLSMD = 29,
	EqType_EQP2ABSMC = 30,
	EqType_EQP2BSMD = 31,
	EqType_EQP1GSMUD = 32,
	EqType_EQP4GSMD = 33,
	EqType_EQP8GSMD = 34,
	EqType_EQP1CSMUC = 35,
	EqType_EQP1CSMUEWC = 36,
	EqType_EQP4CSMD = 37,
	EqType_EQP4CSMUD = 38,
	EqType_EQP4CSMC = 39,
	EqType_EQPOSFM = 40,
	EqType_EQP1PM = 41,
	EqType_EQP2PM = 42,
	EqType_EQP40CSMD = 43,
	EqType_EQPDCF = 44,
	EqType_EQPEDFAS = 45,
	EqType_EQPEDFASGC = 46,
	EqType_EQPEDFADGC = 47,
	EqType_EQPRAMAN = 48,
	EqType_EQP4TCC2G5 = 49,
	EqType_EQP4TCC2G5D = 50,
	EqType_EQP4TCC10GD = 51,
	EqType_EQP4TCC10GC = 52,
	EqType_EQPWCC10GD = 53,
	EqType_EQPWCC10GC = 54,
	EqType_EQPWCC2G71N = 55,
	EqType_EQPWCC2G7D = 56,
	EqType_EQP2TCM2G5 = 57,
	EqType_EQP2TCA2G5 = 58,
	EqType_EQP8TCA10GD = 59,
	EqType_EQP8TCA10GC = 60,
	EqType_EQPWCA10GD = 61,
	EqType_EQPWCA10GC = 62,
	EqType_EQP4TCA4GD = 63,
	EqType_EQP4TCA4GC = 64,
	EqType_EQPWCA2G5 = 65,
	EqType_EQP4TCA1G3D = 66,
	EqType_EQP4TCA1G3C = 67,
	EqType_EQP8TCE2G5D = 68,
	EqType_EQP8TCE2G5C = 69,
	EqType_EQPWCELSD = 70,
	EqType_EQPWCELSC = 71,
	EqType_EQPVSM = 72,
	EqType_EQPRSMOLM = 73,
	EqType_EQPRSMSF = 74,
	EqType_EQPOSCM = 75,
	EqType_EQP2OSCM = 76,
	EqType_EQPDRM = 77,
	EqType_EQPXFPG = 78,
	EqType_EQPSFPD = 79,
	EqType_EQPSFPC = 80,
	EqType_EQPSFPG = 81,
	EqType_EQPSFPE = 82,
	EqType_EQPSH1HUDCM = 83,
	EqType_EQPCUSTOMC = 84,
	EqType_EQPCUSTOMD = 85,
	EqType_EQPPSU1HUDC = 86,
	EqType_EQPWCC2G7C = 87,
	EqType_EQP1CSMUEWD = 88,
	EqType_EQP1CSMUG = 89,
	EqType_EQP3BSMC = 90,
	EqType_EQP2TCA2G5S = 91,
	EqType_EQP8CSMUC = 92,
	EqType_EQPEDFADGCB = 93,
	EqType_EQPOSCMPN = 94,
	EqType_EQP4TCC10GTD = 95,
	EqType_EQP4TCA4G = 96,
	EqType_EQPDCG = 97,
	EqType_EQP2TCM2G5D = 98,
	EqType_EQP2TCM2G5C = 99,
	EqType_EQPWCM2G5D = 100,
	EqType_EQPWCM2G5C = 101,
	EqType_EQPEDFMSGC = 102,
	EqType_EQPF2KDEMIV2 = 103,
	EqType_EQPPSM = 104,
	EqType_EQPNCU2E = 105,
	EqType_EQP8TCEGL2G5D = 106,
	EqType_EQP8TCEGL2G5C = 107,
	EqType_EQPDCF1HU = 108,
	EqType_EQP10TCC10GTD = 109,
	EqType_EQP10TCC10GD = 110,
	EqType_EQP10TCC10GC = 111,
	EqType_EQP16CSMSFD = 112,
	EqType_EQPOSFMSF = 113,
	EqType_EQP2CLSMSFD = 114,
	EqType_EQP3BSMEWC = 115,
	EqType_EQPEDFASGCB = 116,
	EqType_EQPEDFADGCV = 117,
	EqType_EQPWCC10GTD = 118,
	EqType_EQP2CSMUEWC = 119,
	EqType_EQPEROADMDC = 120,
	EqType_EQPSCUS = 122,
	EqType_EQP4OPCM = 123,
	EqType_EQPUTM = 124,
	EqType_EQPPSCU = 125,
	EqType_EQP40CSM2HU = 126,
	EqType_EQP2TWCC2G7 = 127,
	EqType_EQP2WCA10G = 130,
	EqType_EQPNCUHP = 131,
	EqType_EQPNCU20085HU = 132,
	EqType_EQPXFPC = 137,
	EqType_EQPXFPD = 138,
	EqType_EQPWCC40GTD = 140,
	EqType_EQPILM = 141,
	EqType_EQPNCUII = 142,
	EqType_EQPCEM9HU = 143,
	EqType_EQP8ROADMC40 = 148,
	EqType_EQP4TCC40GTD = 149,
	EqType_EQP2PCA10G = 150,
	EqType_EQP10PCA10G = 151,
	EqType_EQP1CSMUD = 152,
	EqType_EQPSFPOSC = 153,
	EqType_EQPSFPODC = 154,
	EqType_EQPSFPOSG = 155,
	EqType_EQPSFPODG = 156,
	EqType_EQPROADMC80 = 157,
	EqType_EQPCCM8 = 158,
	EqType_EQPPSU9HUDC = 159,
	EqType_EQP4TCA4GUS = 160,
	EqType_EQP40CSM3HUD = 161,
	EqType_EQP5PSM = 162,
	EqType_EQPFAN9HU = 163,
	EqType_EQP5TCE10GTD = 164,
	EqType_EQP10TCCS10GTD = 165,
	EqType_EQP40CSM3HUDCD = 166,
	EqType_EQP40CSM3HUDCDI = 167,
	EqType_EQP5GSMD = 169,
	EqType_EQP8CSMD = 170,
	EqType_EQP2OTFM = 171,
	EqType_EQP8OTDR3HU = 172,
	EqType_EQPXFPTD = 173,
	EqType_EQP40CSM3HUDI = 174,
	EqType_EQP8CCMC80 = 175,
	EqType_EQPEDFAD27 = 176,
	EqType_EQP2WCC10G = 177,
	EqType_EQP8ROADMC80 = 178,
	EqType_EQP2WCC10GAES = 180,
	EqType_EQP5TCE10GTAESD = 182,
	EqType_EQPSH1HUPF = 183,
	EqType_EQPFAN1HU = 185,
	EqType_EQP10TCC10G = 186,
	EqType_EQPXFPOTND = 187,
	EqType_EQP2RAMAN = 192,
	EqType_EQPEDFAS26 = 193,
	EqType_EQP5TCES10GTD = 194,
	EqType_EQPXFPTLND = 213,
	EqType_EQP5TCES10GTAESD = 214,
	EqType_EQP2PSM = 218,
	EqType_MAX,
} EqType_t;

typedef enum
{
	AdminState_UNDEFINED = 0,
	AdminState_UAS = 1,
	AdminState_IS = 2,
	AdminState_AINS = 3,
	AdminState_MGT = 4,
	AdminState_MT = 5,
	AdminState_DSBLD = 6,
	AdminState_PPS = 7,
	AdminState_MAX,
} AdminState_t;

typedef enum
{
	PathComputationState_UNDEFINED = 0,
	PathComputationState_IDLE = 1,
	PathComputationState_FAIL = 2,
	PathComputationState_INPROGS = 3,
	PathComputationState_ABT = 4,
	PathComputationState_COMPL = 5,
	PathComputationState_MAX,
} PathComputationState_t;

typedef enum
{
	OperState_UNDEFINED = 0,
	OperState_NR = 1,
	OperState_ANR = 2,
	OperState_OUT = 3,
	OperState_UN = 4,
	OperState_MAX,
} OperState_t;

typedef enum
{
	PathFollow_UNDEFINED = 0,
	PathFollow_STRICT = 1,
	PathFollow_LOOSE = 2,
	PathFollow_MAX,
} PathFollow_t;

typedef enum
{
	CPEntityClass_UNDEFINED = 0,
	CPEntityClass_CP = 1,
	CPEntityClass_TUNNEL = 2,
	CPEntityClass_CONNECTION = 3,
	CPEntityClass_PATH = 4,
	CPEntityClass_PATHELEMENT = 5,
	CPEntityClass_LOGICALINTERFACE = 6,
	CPEntityClass_REMOTEALARM = 7,
	CPEntityClass_PORTBINDING = 8,
	CPEntityClass_MAX,
} CPEntityClass_t;

typedef enum
{
	EntityClass_UNDEFINED = 0,
	EntityClass_OTHER = 1,
	EntityClass_UNKNOWN = 2,
	EntityClass_CHASSIS = 3,
	EntityClass_BACKPLANE = 4,
	EntityClass_CONTAINER = 5,
	EntityClass_POWERSUPPLY = 6,
	EntityClass_FAN = 7,
	EntityClass_SENSOR = 8,
	EntityClass_MODULE = 9,
	EntityClass_PLUG = 10,
	EntityClass_STACK = 11,
	EntityClass_GROUP = 12,
	EntityClass_CLIENTPORT = 13,
	EntityClass_NETWORKPORT = 14,
	EntityClass_VIRTUALCHANNEL = 15,
	EntityClass_CONNECTION = 16,
	EntityClass_VC4CONTAINER = 17,
	EntityClass_VC3STS1CONTAINER = 18,
	EntityClass_VC12VT15CONTAINER = 19,
	EntityClass_DCNCHANNEL = 20,
	EntityClass_ROUTERCONFIG = 21,
	EntityClass_ENVIRONMENTPORT = 22,
	EntityClass_INTERNALPORT = 23,
	EntityClass_UPGRADEPORT = 24,
	EntityClass_MIDSTAGEPORT = 25,
	EntityClass_SERIALPORT = 26,
	EntityClass_PPPIPINTERFACE = 27,
	EntityClass_LANIP = 28,
	EntityClass_VS1CONTAINER = 29,
	EntityClass_STS3CCONTAINER = 30,
	EntityClass_PAYLOADCHANNEL = 31,
	EntityClass_PASSIVESHELF = 32,
	EntityClass_STS24CCONTAINER = 33,
	EntityClass_STS48CCONTAINER = 34,
	EntityClass_VS2CCONTAINER = 35,
	EntityClass_VS4CCONTAINER = 36,
	EntityClass_TIFINPUTPORT = 37,
	EntityClass_TIFOUTPUTPORT = 38,
	EntityClass_OPTICALLINK = 39,
	EntityClass_VIRTUALOPTICALCHANNEL = 40,
	EntityClass_LOGICALINTERFACE = 41,
	EntityClass_PHYSICALTERMINATIONPOINT = 42,
	EntityClass_ETHCLIENT = 43,
	EntityClass_ETHNETWORK = 44,
	EntityClass_VETH = 45,
	EntityClass_FLOW = 47,
	EntityClass_CTRANS = 48,
	EntityClass_POLICERONFLOW = 50,
	EntityClass_QUEUEONPORT = 51,
	EntityClass_QUEUEONFLOW = 52,
	EntityClass_FARENDPLUG = 53,
	EntityClass_FARENDCHANNEL = 54,
	EntityClass_VC4C8CONTAINER = 55,
	EntityClass_VC4C16CONTAINER = 56,
	EntityClass_VS0CONTAINER = 57,
	EntityClass_VIRTUALSUBCHANNEL = 58,
	EntityClass_BRIDGE = 59,
	EntityClass_QUEUEONBRIDGE = 60,
	EntityClass_BACKWARDVIRTUALOPTMUX = 61,
	EntityClass_FORWARDVIRTUALOPTMUX = 62,
	EntityClass_VIRTUALCHANNELN = 64,
	EntityClass_EXTERNALCHANNEL = 65,
	EntityClass_VIRTUALTERMINATIONPOINT = 66,
	EntityClass_VIRTUALCONNECTION = 67,
	EntityClass_MAX,
} EntityClass_t;

typedef enum
{
	TeType_UNDEFINED = 0,
	TeType_NODE = 1,
	TeType_LINKUN = 2,
	TeType_LINKNR = 3,
	TeType_MAX,
} TeType_t;

typedef enum
{
	ErrorCategory_UNDEFINED = 0,
	ErrorCategory_SUCCESS = 1,
	ErrorCategory_NONOPTIMAL = 2,
	ErrorCategory_PROVISION = 3,
	ErrorCategory_PATHCOMPUTATION = 4,
	ErrorCategory_COMMUNICATIONCONNECTION = 5,
	ErrorCategory_NETWORKCOMMUNICATION = 6,
	ErrorCategory_RESOURCEUNAVAILABLE = 7,
	ErrorCategory_PROGRESS = 8,
	ErrorCategory_NOPATH = 9,
	ErrorCategory_MAX,
} ErrorCategory_t;

#endif
