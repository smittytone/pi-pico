/*
 * Phantom Slayer
 *
 * @version     1.0.2
 * @author      smittytone
 * @copyright   2021, Tony Smith
 * @licence     MIT
 *
 */
#include "main.h"


/*
 * Define some basic maps for testing
 */
char base_map_00[20] = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";
char base_map_01[20] = "\xFF\xEE\xEE\xFF\xEE\xEE\xFF\xEE\xEE\xEE\xEE\xFF\xEE\xEE\xFF\xEE\xEE\xFF\xEE\xEE";
char base_map_02[20] = "\xFF\xEE\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xEE";
char base_map_03[20] = "\xFF\xFF\xFF\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xEE\xFF\xFF\xFF\xEE\xEE\xEE\xFF\xEE\xFF";
char base_map_04[20] = "\xEE\xEE\xFF\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xEE\xEE\xEE\xEE\xFF\xFF\xFF";
char base_map_05[20] = "\xFF\xFF\xFF\xFF\xFF\xEE\xEE\xFF\xEE\xEE\xFF\xEE\xEE\xEE\xFF\xFF\xFF\xFF\xEE\xEE";
char base_map_06[20] = "\xEE\xEE\xEE\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xEE\xFF\xFF\xFF";
char base_map_07[20] = "\xFF\xFF\xFF\xEE\xEE\xEE\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xEE\xFF\xFF\xFF\xFF\xEE\xEE";
char base_map_08[20] = "\xFF\xEE\xFF\xFF\xFF\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xEE\xFF\xFF\xFF";
char base_map_09[20] = "\xFF\xFF\xFF\xEE\xFF\xFF\xFF\xEE\xEE\xEE\xFF\xEE\xEE\xEE\xEE\xEE\xEE\xEE\xFF\xEE";
char base_map_10[20] = "\xEE\xEE\xFF\xFF\xEE\xFF\xEE\xEE\xEE\xEE\xEE\xEE\xEE\xEE\xEE\xFF\xFF\xFF\xFF\xFF";
char base_map_11[20] = "\xFF\xFF\xFF\xEE\xFF\xFF\xFF\xFF\xEE\xFF\xEE\xEE\xFF\xFF\xFF\xFF\xEE\xFF\xEE\xEE";
char base_map_12[20] = "\xFF\xEE\xFF\xFF\xFF\xEE\xEE\xFF\xFF\xFF\xFF\xEE\xEE\xEE\xFF\xEE\xEE\xFF\xFF\xFF";
char base_map_13[20] = "\xEE\xEE\xEE\xEE\xEE\xEE\xFF\xFF\xEE\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xFF";
char base_map_14[20] = "\xFF\xFF\xFF\xFF\xEE\xEE\xFF\xEE\xEE\xEE\xFF\xEE\xEE\xEE\xFF\xEE\xEE\xFF\xFF\xFF";
char base_map_15[20] = "\xEE\xFF\xEE\xFF\xFF\xEE\xFF\xEE\xEE\xEE\xFF\xEE\xEE\xEE\xFF\xFF\xFF\xEE\xEE\xEE";
char base_map_16[20] = "\xFF\xEE\xEE\xFF\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xEE\xFF\xEE\xEE\xEE\xEE\xEE";
char base_map_17[20] = "\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xEE\xEE\xEE\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";
char base_map_18[20] = "\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xEE\xEE\xFF\xEE\xEE\xFF\xEE\xFF\xEE\xFF\xEE";
char base_map_19[20] = "\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEE";

char base_map_20[20] = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";
char base_map_21[20] = "\xEE\xEE\xFF\xEE\xEE\xEE\xEE\xFF\xEE\xEE\xFF\xEE\xFF\xEE\xEE\xFF\xEE\xFF\xEE\xFF";
char base_map_22[20] = "\xEE\xEE\xFF\xEE\xEE\xEE\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xFF\xFF\xFF\xFF\xFF\xFF";
char base_map_23[20] = "\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xEE\xEE\xEE\xFF\xEE\xFF\xFF\xFF\xEE\xEE\xEE\xFF\xEE";
char base_map_24[20] = "\xEE\xEE\xFF\xEE\xEE\xFF\xFF\xEE\xEE\xFF\xFF\xFF\xEE\xEE\xEE\xEE\xFF\xFF\xFF\xFF";
char base_map_25[20] = "\xEE\xFF\xFF\xFF\xFF\xEE\xFF\xFF\xEE\xFF\xEE\xEE\xEE\xEE\xEE\xEE\xEE\xEE\xFF\xEE";
char base_map_26[20] = "\xFF\xFF\xEE\xEE\xEE\xEE\xFF\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEE";
char base_map_27[20] = "\xEE\xFF\xFF\xFF\xEE\xFF\xFF\xFF\xFF\xEE\xEE\xEE\xEE\xEE\xEE\xEE\xEE\xEE\xFF\xFF";
char base_map_28[20] = "\xFF\xFF\xEE\xEE\xFF\xFF\xEE\xEE\xFF\xFF\xFF\xEE\xEE\xEE\xEE\xEE\xFF\xEE\xFF\xEE";
char base_map_29[20] = "\xEE\xFF\xFF\xFF\xFF\xEE\xEE\xEE\xFF\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";
char base_map_30[20] = "\xFF\xEE\xEE\xEE\xEE\xEE\xEE\xEE\xFF\xFF\xFF\xEE\xEE\xEE\xEE\xEE\xEE\xEE\xEE\xFF";
char base_map_31[20] = "\xFF\xFF\xFF\xEE\xEE\xFF\xFF\xFF\xEE\xEE\xFF\xFF\xFF\xFF\xEE\xFF\xFF\xFF\xEE\xFF";
char base_map_32[20] = "\xEE\xEE\xFF\xFF\xFF\xFF\xEE\xFF\xEE\xEE\xEE\xFF\xEE\xEE\xFF\xFF\xEE\xFF\xFF\xEE";
char base_map_33[20] = "\xEE\xEE\xEE\xEE\xEE\xEE\xEE\xFF\xFF\xFF\xEE\xFF\xFF\xFF\xFF\xEE\xEE\xEE\xFF\xFF";
char base_map_34[20] = "\xFF\xFF\xFF\xFF\xEE\xEE\xFF\xFF\xEE\xFF\xFF\xFF\xEE\xEE\xFF\xFF\xFF\xFF\xFF\xEE";
char base_map_35[20] = "\xFF\xEE\xEE\xFF\xFF\xFF\xFF\xEE\xEE\xEE\xEE\xFF\xFF\xFF\xEE\xEE\xEE\xEE\xFF\xFF";
char base_map_36[20] = "\xFF\xFF\xFF\xFF\xEE\xEE\xEE\xEE\xEE\xEE\xEE\xFF\xEE\xEE\xEE\xFF\xEE\xEE\xFF\xEE";
char base_map_37[20] = "\xEE\xEE\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";
char base_map_38[20] = "\xEE\xEE\xEE\xFF\xEE\xEE\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xEE\xFF\xEE";
char base_map_39[20] = "\xEE\xEE\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEE";

char base_map_40[20] = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";
char base_map_41[20] = "\xEE\xEE\xEE\xFF\xEE\xEE\xEE\xFF\xEE\xEE\xEE\xEE\xFF\xEE\xEE\xEE\xEE\xEE\xEE\xEE";
char base_map_42[20] = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xEE\xEE\xEE\xEE\xEE\xEE";
char base_map_43[20] = "\xEE\xEE\xEE\xFF\xEE\xEE\xEE\xFF\xEE\xEE\xEE\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";
char base_map_44[20] = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xEE\xEE\xFF\xEE\xEE\xEE\xEE\xFF\xEE";
char base_map_45[20] = "\xEE\xFF\xEE\xEE\xEE\xEE\xEE\xFF\xEE\xFF\xFF\xFF\xFF\xFF\xEE\xEE\xEE\xEE\xFF\xFF";
char base_map_46[20] = "\xFF\xFF\xEE\xEE\xEE\xEE\xEE\xFF\xEE\xEE\xFF\xEE\xFF\xFF\xFF\xEE\xEE\xFF\xEE\xFF";
char base_map_47[20] = "\xEE\xFF\xFF\xFF\xFF\xFF\xEE\xFF\xFF\xEE\xFF\xFF\xFF\xEE\xEE\xFF\xFF\xFF\xFF\xFF";
char base_map_48[20] = "\xEE\xFF\xEE\xFF\xEE\xFF\xFF\xEE\xFF\xFF\xFF\xEE\xFF\xFF\xFF\xFF\xEE\xEE\xFF\xEE";
char base_map_49[20] = "\xFF\xFF\xFF\xFF\xFF\xEE\xFF\xEE\xFF\xEE\xFF\xFF\xFF\xEE\xEE\xEE\xEE\xFF\xFF\xFF";
char base_map_50[20] = "\xEE\xEE\xEE\xFF\xEE\xEE\xFF\xFF\xFF\xFF\xEE\xEE\xEE\xFF\xFF\xFF\xFF\xFF\xEE\xFF";
char base_map_51[20] = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xEE\xFF\xFF\xFF\xFF\xFF\xEE\xEE\xEE\xFF\xEE\xFF";
char base_map_52[20] = "\xEE\xEE\xEE\xEE\xEE\xEE\xFF\xEE\xEE\xEE\xEE\xFF\xEE\xFF\xEE\xEE\xEE\xFF\xFF\xFF";
char base_map_53[20] = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xFF\xFF\xFF\xFF\xEE\xFF\xEE";
char base_map_54[20] = "\xFF\xEE\xEE\xFF\xEE\xEE\xFF\xEE\xFF\xEE\xEE\xFF\xFF\xEE\xEE\xEE\xFF\xEE\xFF\xFF";
char base_map_55[20] = "\xFF\xFF\xEE\xFF\xEE\xEE\xFF\xFF\xFF\xFF\xFF\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xFF";
char base_map_56[20] = "\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xEE\xEE\xEE\xEE\xFF\xEE\xEE\xEE\xEE\xFF\xEE\xFF";
char base_map_57[20] = "\xFF\xEE\xFF\xEE\xEE\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEE";
char base_map_58[20] = "\xFF\xEE\xFF\xEE\xEE\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xEE\xFF\xEE\xEE\xFF\xEE\xFF\xEE";
char base_map_59[20] = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";

char base_map_60[20] = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";
char base_map_61[20] = "\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xEE\xEE\xFF\xEE\xEE\xFF\xEE\xFF\xEE";
char base_map_62[20] = "\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xFF\xFF\xFF\xFF\xEE\xFF\xFF\xEE\xFF\xFF\xFF\xFF\xFF";
char base_map_63[20] = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xEE\xFF\xFF\xEE\xFF\xFF\xFF\xEE\xFF\xEE\xFF";
char base_map_64[20] = "\xEE\xEE\xFF\xEE\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xEE\xFF\xFF\xEE\xFF\xFF\xEE\xFF\xFF";
char base_map_65[20] = "\xFF\xFF\xEE\xFF\xEE\xEE\xFF\xFF\xFF\xFF\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xEE\xEE\xFF";
char base_map_66[20] = "\xEE\xFF\xEE\xFF\xFF\xFF\xEE\xEE\xFF\xEE\xFF\xFF\xFF\xFF\xEE\xEE\xFF\xEE\xFF\xFF";
char base_map_67[20] = "\xFF\xFF\xFF\xFF\xEE\xFF\xFF\xFF\xFF\xEE\xEE\xFF\xEE\xFF\xFF\xEE\xFF\xFF\xEE\xFF";
char base_map_68[20] = "\xFF\xEE\xEE\xFF\xEE\xFF\xEE\xEE\xFF\xFF\xFF\xFF\xEE\xEE\xFF\xFF\xFF\xEE\xFF\xFF";
char base_map_69[20] = "\xEE\xFF\xEE\xFF\xEE\xFF\xFF\xEE\xFF\xEE\xFF\xEE\xFF\xFF\xEE\xEE\xFF\xFF\xEE\xEE";
char base_map_70[20] = "\xEE\xFF\xFF\xEE\xFF\xEE\xFF\xEE\xEE\xEE\xEE\xEE\xFF\xEE\xFF\xFF\xEE\xEE\xFF\xFF";
char base_map_71[20] = "\xFF\xEE\xFF\xEE\xFF\xEE\xFF\xFF\xFF\xEE\xFF\xFF\xFF\xFF\xFF\xEE\xEE\xFF\xEE\xFF";
char base_map_72[20] = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xFF\xEE\xFF\xEE\xEE\xFF\xEE\xFF\xFF\xFF\xFF\xFF";
char base_map_73[20] = "\xFF\xEE\xFF\xEE\xEE\xFF\xEE\xEE\xFF\xFF\xFF\xFF\xFF\xEE\xEE\xFF\xEE\xEE\xEE\xFF";
char base_map_74[20] = "\xFF\xFF\xEE\xEE\xEE\xFF\xEE\xFF\xFF\xEE\xFF\xEE\xFF\xEE\xFF\xFF\xEE\xFF\xEE\xEE";
char base_map_75[20] = "\xEE\xFF\xFF\xEE\xFF\xFF\xFF\xEE\xFF\xEE\xFF\xEE\xEE\xFF\xEE\xFF\xFF\xFF\xFF\xFF";
char base_map_76[20] = "\xFF\xFF\xEE\xFF\xFF\xEE\xFF\xFF\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xFF\xEE\xEE";
char base_map_77[20] = "\xFF\xEE\xEE\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xEE\xFF\xEE\xFF\xFF\xFF";
char base_map_78[20] = "\xFF\xEE\xEE\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xEE\xEE\xEE\xFF\xEE\xEE\xEE\xFF";
char base_map_79[20] = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";

char base_map_80[20] = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";
char base_map_81[20] = "\xEE\xFF\xEE\xEE\xEE\xFF\xEE\xEE\xEE\xEE\xFF\xEE\xEE\xEE\xEE\xFF\xEE\xFF\xEE\xEE";
char base_map_82[20] = "\xEE\xFF\xEE\xEE\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xEE\xEE\xEE\xFF\xFF\xFF\xFF\xFF";
char base_map_83[20] = "\xFF\xFF\xFF\xEE\xEE\xFF\xEE\xEE\xEE\xEE\xFF\xFF\xEE\xEE\xEE\xFF\xEE\xEE\xFF\xEE";
char base_map_84[20] = "\xFF\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xEE\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";
char base_map_85[20] = "\xFF\xFF\xFF\xEE\xEE\xFF\xEE\xFF\xFF\xEE\xEE\xFF\xEE\xEE\xFF\xEE\xEE\xEE\xEE\xEE";
char base_map_86[20] = "\xEE\xEE\xEE\xEE\xEE\xFF\xFF\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xFF\xFF\xFF";
char base_map_87[20] = "\xEE\xFF\xFF\xFF\xEE\xFF\xEE\xEE\xEE\xEE\xFF\xEE\xEE\xEE\xEE\xFF\xFF\xFF\xEE\xEE";
char base_map_88[20] = "\xFF\xFF\xEE\xFF\xFF\xFF\xFF\xFF\xEE\xEE\xFF\xFF\xFF\xEE\xEE\xEE\xEE\xEE\xEE\xEE";
char base_map_89[20] = "\xFF\xEE\xEE\xEE\xEE\xEE\xEE\xFF\xFF\xFF\xFF\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";
char base_map_90[20] = "\xFF\xFF\xFF\xEE\xEE\xEE\xEE\xFF\xEE\xEE\xEE\xEE\xFF\xEE\xFF\xEE\xEE\xEE\xFF\xEE";
char base_map_91[20] = "\xFF\xEE\xFF\xFF\xFF\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xFF\xFF\xFF\xFF\xFF\xFF";
char base_map_92[20] = "\xFF\xFF\xFF\xEE\xFF\xFF\xFF\xEE\xEE\xFF\xEE\xEE\xFF\xFF\xEE\xEE\xEE\xEE\xEE\xFF";
char base_map_93[20] = "\xEE\xEE\xEE\xEE\xEE\xEE\xEE\xEE\xEE\xFF\xFF\xFF\xFF\xEE\xEE\xEE\xEE\xEE\xEE\xFF";
char base_map_94[20] = "\xFF\xEE\xFF\xFF\xFF\xEE\xEE\xEE\xEE\xFF\xEE\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xFF";
char base_map_95[20] = "\xFF\xFF\xFF\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xFF\xEE\xEE\xEE\xFF\xFF\xFF";
char base_map_96[20] = "\xEE\xFF\xEE\xFF\xFF\xEE\xEE\xEE\xEE\xFF\xEE\xEE\xEE\xFF\xFF\xEE\xEE\xEE\xEE\xFF";
char base_map_97[20] = "\xEE\xFF\xFF\xFF\xEE\xEE\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xFF\xFF\xFF\xFF\xFF\xEE";
char base_map_98[20] = "\xEE\xFF\xEE\xFF\xEE\xEE\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xEE\xFF\xEE\xFF\xEE\xFF\xEE";
char base_map_99[20] = "\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEE";

char base_map_100[20] = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";
char base_map_101[20] = "\xFF\xEE\xEE\xEE\xFF\xEE\xEE\xEE\xEE\xFF\xEE\xEE\xFF\xEE\xEE\xEE\xFF\xEE\xFF\xEE";
char base_map_102[20] = "\xFF\xFF\xEE\xEE\xFF\xEE\xEE\xFF\xFF\xFF\xEE\xEE\xFF\xEE\xEE\xFF\xFF\xEE\xFF\xFF";
char base_map_103[20] = "\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xFF\xEE\xFF\xFF\xFF\xEE\xFF\xEE\xFF\xEE\xFF";
char base_map_104[20] = "\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xEE\xEE\xFF\xFF\xFF\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xFF";
char base_map_105[20] = "\xFF\xFF\xEE\xFF\xEE\xFF\xFF\xFF\xFF\xFF\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xFF";
char base_map_106[20] = "\xFF\xEE\xEE\xFF\xEE\xFF\xEE\xEE\xFF\xEE\xFF\xFF\xEE\xFF\xEE\xFF\xFF\xFF\xEE\xFF";
char base_map_107[20] = "\xEE\xFF\xFF\xFF\xFF\xFF\xEE\xEE\xEE\xFF\xEE\xFF\xFF\xEE\xFF\xFF\xEE\xFF\xFF\xFF";
char base_map_108[20] = "\xFF\xFF\xEE\xFF\xEE\xFF\xFF\xEE\xFF\xFF\xEE\xEE\xFF\xFF\xEE\xEE\xEE\xFF\xEE\xFF";
char base_map_109[20] = "\xEE\xEE\xFF\xFF\xEE\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xEE\xFF\xEE\xFF\xEE\xFF\xFF\xFF";
char base_map_110[20] = "\xFF\xFF\xFF\xEE\xFF\xFF\xFF\xEE\xFF\xEE\xFF\xFF\xEE\xEE\xFF\xFF\xEE\xEE\xEE\xFF";
char base_map_111[20] = "\xFF\xEE\xEE\xFF\xFF\xEE\xFF\xFF\xFF\xFF\xFF\xEE\xFF\xFF\xFF\xEE\xFF\xEE\xFF\xFF";
char base_map_112[20] = "\xFF\xEE\xFF\xEE\xEE\xFF\xEE\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xEE\xFF\xFF\xFF\xFF\xEE";
char base_map_113[20] = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xEE\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xEE\xEE\xFF";
char base_map_114[20] = "\xFF\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xEE\xFF\xEE\xFF\xEE\xFF\xEE\xFF";
char base_map_115[20] = "\xEE\xFF\xFF\xFF\xEE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEE\xFF\xEE\xFF\xFF\xFF\xFF\xFF";
char base_map_116[20] = "\xFF\xFF\xEE\xFF\xFF\xEE\xFF\xEE\xEE\xEE\xFF\xEE\xFF\xEE\xFF\xFF\xEE\xFF\xEE\xFF";
char base_map_117[20] = "\xFF\xEE\xFF\xFF\xEE\xEE\xFF\xFF\xFF\xEE\xFF\xFF\xFF\xEE\xFF\xEE\xEE\xFF\xFF\xFF";
char base_map_118[20] = "\xFF\xEE\xEE\xFF\xEE\xEE\xFF\xEE\xEE\xEE\xFF\xEE\xFF\xEE\xEE\xFF\xEE\xEE\xEE\xFF";
char base_map_119[20] = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";


/*
 * Map Functions
 */
uint8_t map_init(uint8_t last_map) {
    // Load the current map randomly from the pre-made ones
    // TODO Randomly generate a map

    // FROM 1.0.2
    // Don't pick the same map as last time
    uint8_t map;
    do {
        map = irandom(0, NUMBER_OF_MAPS);
    } while (map != last_map);

    /* TEST VALUE
    map = 1;
    */

    switch(map) {
        case 0:
            current_map[0] = base_map_00;
            current_map[1] = base_map_01;
            current_map[2] = base_map_02;
            current_map[3] = base_map_03;
            current_map[4] = base_map_04;
            current_map[5] = base_map_05;
            current_map[6] = base_map_06;
            current_map[7] = base_map_07;
            current_map[8] = base_map_08;
            current_map[9] = base_map_09;
            current_map[10] = base_map_10;
            current_map[11] = base_map_11;
            current_map[12] = base_map_12;
            current_map[13] = base_map_13;
            current_map[14] = base_map_14;
            current_map[15] = base_map_15;
            current_map[16] = base_map_16;
            current_map[17] = base_map_17;
            current_map[18] = base_map_18;
            current_map[19] = base_map_19;
            break;
        case 1:
            current_map[0] = base_map_40;
            current_map[1] = base_map_41;
            current_map[2] = base_map_42;
            current_map[3] = base_map_43;
            current_map[4] = base_map_44;
            current_map[5] = base_map_45;
            current_map[6] = base_map_46;
            current_map[7] = base_map_47;
            current_map[8] = base_map_48;
            current_map[9] = base_map_49;
            current_map[10] = base_map_50;
            current_map[11] = base_map_51;
            current_map[12] = base_map_52;
            current_map[13] = base_map_53;
            current_map[14] = base_map_54;
            current_map[15] = base_map_55;
            current_map[16] = base_map_56;
            current_map[17] = base_map_57;
            current_map[18] = base_map_58;
            current_map[19] = base_map_59;
            break;
        case 2:
            current_map[0] = base_map_60;
            current_map[1] = base_map_61;
            current_map[2] = base_map_62;
            current_map[3] = base_map_63;
            current_map[4] = base_map_64;
            current_map[5] = base_map_65;
            current_map[6] = base_map_66;
            current_map[7] = base_map_67;
            current_map[8] = base_map_68;
            current_map[9] = base_map_69;
            current_map[10] = base_map_70;
            current_map[11] = base_map_71;
            current_map[12] = base_map_72;
            current_map[13] = base_map_73;
            current_map[14] = base_map_74;
            current_map[15] = base_map_75;
            current_map[16] = base_map_76;
            current_map[17] = base_map_77;
            current_map[18] = base_map_78;
            current_map[19] = base_map_79;
            break;
        case 3:
            current_map[0] = base_map_80;
            current_map[1] = base_map_81;
            current_map[2] = base_map_82;
            current_map[3] = base_map_83;
            current_map[4] = base_map_84;
            current_map[5] = base_map_85;
            current_map[6] = base_map_86;
            current_map[7] = base_map_87;
            current_map[8] = base_map_88;
            current_map[9] = base_map_89;
            current_map[10] = base_map_90;
            current_map[11] = base_map_91;
            current_map[12] = base_map_92;
            current_map[13] = base_map_93;
            current_map[14] = base_map_94;
            current_map[15] = base_map_95;
            current_map[16] = base_map_96;
            current_map[17] = base_map_97;
            current_map[18] = base_map_98;
            current_map[19] = base_map_99;
            break;
        case 4:
            current_map[0] = base_map_100;
            current_map[1] = base_map_101;
            current_map[2] = base_map_102;
            current_map[3] = base_map_103;
            current_map[4] = base_map_104;
            current_map[5] = base_map_105;
            current_map[6] = base_map_106;
            current_map[7] = base_map_107;
            current_map[8] = base_map_108;
            current_map[9] = base_map_109;
            current_map[10] = base_map_110;
            current_map[11] = base_map_111;
            current_map[12] = base_map_112;
            current_map[13] = base_map_113;
            current_map[14] = base_map_114;
            current_map[15] = base_map_115;
            current_map[16] = base_map_116;
            current_map[17] = base_map_117;
            current_map[18] = base_map_118;
            current_map[19] = base_map_119;
        default:
            current_map[0] = base_map_20;
            current_map[1] = base_map_21;
            current_map[2] = base_map_22;
            current_map[3] = base_map_23;
            current_map[4] = base_map_24;
            current_map[5] = base_map_25;
            current_map[6] = base_map_26;
            current_map[7] = base_map_27;
            current_map[8] = base_map_28;
            current_map[9] = base_map_29;
            current_map[10] = base_map_30;
            current_map[11] = base_map_31;
            current_map[12] = base_map_32;
            current_map[13] = base_map_33;
            current_map[14] = base_map_34;
            current_map[15] = base_map_35;
            current_map[16] = base_map_36;
            current_map[17] = base_map_37;
            current_map[18] = base_map_38;
            current_map[19] = base_map_39;
    }

    return map;
}


void show_map(uint8_t y_delta, bool show_entities) {
    // Draw the current map on the screen buffer, centred but
    // vertically adjusted according to 'y_delta'. If 'show_entities'
    // is true, the phantom locations are plotted in. The player and
    // the teleport sqaure positions are always shown.
    // NOTE With the map now drawn on 3x3 blocks, 'y_delta'
    //      has s very limited range of useable values

    // Put a Box around the map
    ssd1306_rect(32, 0 + y_delta, 64, 64, 1, false);

    // Draw the map
    uint8_t x = 34;
    uint8_t y = 2 + y_delta;

    for (uint8_t i = 0 ; i < 20 ; ++i) {
        char *line = current_map[i];
        for (uint8_t j = 0 ; j < 20 ; ++j) {
            uint8_t pixel = line[j];

            // Draw and empty (path) square
            if (pixel != MAP_TILE_WALL) {
                ssd1306_plot(x + j * 3,     y + i * 3,     1);
                ssd1306_plot(x + j * 3 + 1, y + i * 3,     1);
                ssd1306_plot(x + j * 3 + 2, y + i * 3,     1);

                ssd1306_plot(x + j * 3,     y + i * 3 + 1, 1);
                ssd1306_plot(x + j * 3 + 1, y + i * 3 + 1, 1);
                ssd1306_plot(x + j * 3 + 2, y + i * 3 + 1, 1);

                ssd1306_plot(x + j * 3,     y + i * 3 + 2, 1);
                ssd1306_plot(x + j * 3 + 1, y + i * 3 + 2, 1);
                ssd1306_plot(x + j * 3 + 2, y + i * 3 + 2, 1);
            }

            // Show the teleport square
            if (pixel == MAP_TILE_TELEPORTER) {
                ssd1306_plot(x + j * 3 + 1, y + i * 3, 0);
                ssd1306_plot(x + j * 3,     y + i * 3 + 1, 0);
                ssd1306_plot(x + j * 3 + 2, y + i * 3 + 1, 0);
                ssd1306_plot(x + j * 3 + 1, y + i * 3 + 2, 0);
            }

            // Show the player as an arrow at the current square
            if (j == player_x && i == player_y) {
                switch(player_direction) {
                    case DIRECTION_NORTH:
                        ssd1306_plot(x + j * 3 + 1, y + i * 3,     0);
                        ssd1306_plot(x + j * 3,     y + i * 3 + 1, 0);
                        ssd1306_plot(x + j * 3 + 1, y + i * 3 + 1, 0);
                        ssd1306_plot(x + j * 3 + 2, y + i * 3 + 1, 0);
                        ssd1306_plot(x + j * 3,     y + i * 3 + 2, 0);
                        ssd1306_plot(x + j * 3 + 2, y + i * 3 + 2, 0);
                        break;
                    case DIRECTION_EAST:
                        ssd1306_plot(x + j * 3,     y + i * 3,     0);
                        ssd1306_plot(x + j * 3 + 1, y + i * 3,     0);
                        ssd1306_plot(x + j * 3 + 1, y + i * 3 + 1, 0);
                        ssd1306_plot(x + j * 3 + 2, y + i * 3 + 1, 0);
                        ssd1306_plot(x + j * 3,     y + i * 3 + 2, 0);
                        ssd1306_plot(x + j * 3 + 1, y + i * 3 + 2, 0);
                        break;
                    case DIRECTION_SOUTH:
                        ssd1306_plot(x + j * 3,     y + i * 3,     0);
                        ssd1306_plot(x + j * 3 + 2, y + i * 3,     0);
                        ssd1306_plot(x + j * 3,     y + i * 3 + 1, 0);
                        ssd1306_plot(x + j * 3 + 1, y + i * 3 + 1, 0);
                        ssd1306_plot(x + j * 3 + 2, y + i * 3 + 1, 0);
                        ssd1306_plot(x + j * 3 + 1, y + i * 3 + 2, 0);
                        break;
                    default:
                        ssd1306_plot(x + j * 3    , y + i * 3 + 1, 0);
                        ssd1306_plot(x + j * 3 + 1, y + i * 3    , 0);
                        ssd1306_plot(x + j * 3 + 1, y + i * 3 + 1, 0);
                        ssd1306_plot(x + j * 3 + 1, y + i * 3 + 2, 0);
                        ssd1306_plot(x + j * 3 + 2, y + i * 3    , 0);
                        ssd1306_plot(x + j * 3 + 2, y + i * 3 + 2, 0);

                }
            }

            if (show_entities) {
                // Show any phantoms at the current square as a cross
                for (uint8_t k = 0 ; k < game.phantoms; ++k) {
                    if (j == phantoms[k].x && i == phantoms[k].y) {
                        ssd1306_plot(x + j * 3    , y + i * 3 + 1, 0);
                        ssd1306_plot(x + j * 3 + 1, y + i * 3,     0);
                        ssd1306_plot(x + j * 3 + 1, y + i * 3 + 1, 0);
                        ssd1306_plot(x + j * 3 + 1, y + i * 3 + 2, 0);
                        ssd1306_plot(x + j * 3 + 2, y + i * 3 + 1, 0);
                    }
                }
            }
        }
    }
}


uint8_t get_square_contents(uint8_t x, uint8_t y) {
    // Return the contents of the specified grid reference
    if (x > 19 || y > 19) return MAP_TILE_WALL;
    char *line = current_map[y];
    return line[x];
}


bool set_square_contents(uint8_t x, uint8_t y, uint8_t value) {
    // Set the contents of the specified grid reference,
    // and return true, otherwise false on error
    if (x > 19 || y > 19) return false;
    char *line = current_map[y];
    line[x] = value;
    return true;
}


uint8_t get_view_distance(int8_t x, int8_t y, uint8_t direction) {
    // Return the number of squares the player (at x,y) can see
    // ahead of them in the direction they are facing.
    // NOTE 'count' excludes the current square
    uint8_t count = 0;
    switch(direction) {
        case DIRECTION_NORTH:
            if (y == 0) return count;
            --y;
            do {
                if (get_square_contents(x, y) == MAP_TILE_WALL) break;
                ++count;
                --y;
            } while (y >= 0);
            break;
        case DIRECTION_EAST:
            if (x > 18) return count;
            x++;
            do {
                if (get_square_contents(x, y) == MAP_TILE_WALL) break;
                ++count;
                ++x;
            } while (x < 20);
            break;
        case DIRECTION_SOUTH:
            if (y > 18) return count;
            ++y;
            do {
                if (get_square_contents(x, y) == MAP_TILE_WALL) break;
                ++count;
                ++y;
            } while (y < 20);
            break;
        default:
            if (x == 0) return count;
            --x;
            do {
                if (get_square_contents(x, y) == MAP_TILE_WALL) break;
                ++count;
                --x;
            } while (x >= 0);
    }

    if (count > MAX_VIEW_RANGE) count = MAX_VIEW_RANGE;
    return count;
}
