/**********************************************************************/
/*   ____  ____                                                       */
/*  /   /\/   /                                                       */
/* /___/  \  /                                                        */
/* \   \   \/                                                       */
/*  \   \        Copyright (c) 2003-2009 Xilinx, Inc.                */
/*  /   /          All Right Reserved.                                 */
/* /---/   /\                                                         */
/* \   \  /  \                                                      */
/*  \___\/\___\                                                    */
/***********************************************************************/

#include "xsi.h"

struct XSI_INFO xsi_info;

char *WORK_P_0327194142;
char *IEEE_P_2592010699;
char *STD_STANDARD;
char *IEEE_P_3499444699;
char *WORK_P_2703512042;
char *UNISIM_P_0947159679;
char *IEEE_P_1242562249;
char *IEEE_P_3620187407;
char *WORK_P_0821586110;
char *IEEE_P_0774719531;
char *IEEE_P_3564397177;
char *STD_TEXTIO;
char *UNISIM_P_3222816464;
char *IEEE_P_2717149903;
char *IEEE_P_1367372525;


int main(int argc, char **argv)
{
    xsi_init_design(argc, argv);
    xsi_register_info(&xsi_info);

    xsi_register_min_prec_unit(-12);
    ieee_p_2592010699_init();
    ieee_p_3499444699_init();
    ieee_p_3620187407_init();
    std_textio_init();
    ieee_p_3564397177_init();
    ieee_p_1242562249_init();
    ieee_p_2717149903_init();
    ieee_p_1367372525_init();
    ieee_p_0774719531_init();
    work_p_0327194142_init();
    work_p_2703512042_init();
    unisim_p_0947159679_init();
    unisim_p_3222816464_init();
    unisim_a_1490675510_1976025627_init();
    unisim_a_0780662263_2014779070_init();
    unisim_a_4002211264_2683070183_init();
    unisim_a_1893280375_2982649196_init();
    unisim_a_2575347804_2982649196_init();
    unisim_a_0402065255_2679555531_init();
    unisim_a_2188764497_1233898481_init();
    work_a_3537674494_3212880686_init();
    work_a_2974611363_0394840199_init();
    work_p_0821586110_init();
    work_a_3717920446_3212880686_init();
    work_a_1474961982_1142133938_init();
    work_a_3903431996_3212880686_init();
    work_a_4177355142_3212880686_init();
    work_a_0605910504_0286164271_init();
    work_a_3058721471_0753780893_init();
    work_a_1949178628_3212880686_init();


    xsi_register_tops("work_a_1949178628_3212880686");

    WORK_P_0327194142 = xsi_get_engine_memory("work_p_0327194142");
    IEEE_P_2592010699 = xsi_get_engine_memory("ieee_p_2592010699");
    xsi_register_ieee_std_logic_1164(IEEE_P_2592010699);
    STD_STANDARD = xsi_get_engine_memory("std_standard");
    IEEE_P_3499444699 = xsi_get_engine_memory("ieee_p_3499444699");
    WORK_P_2703512042 = xsi_get_engine_memory("work_p_2703512042");
    UNISIM_P_0947159679 = xsi_get_engine_memory("unisim_p_0947159679");
    IEEE_P_1242562249 = xsi_get_engine_memory("ieee_p_1242562249");
    IEEE_P_3620187407 = xsi_get_engine_memory("ieee_p_3620187407");
    WORK_P_0821586110 = xsi_get_engine_memory("work_p_0821586110");
    IEEE_P_0774719531 = xsi_get_engine_memory("ieee_p_0774719531");
    IEEE_P_3564397177 = xsi_get_engine_memory("ieee_p_3564397177");
    STD_TEXTIO = xsi_get_engine_memory("std_textio");
    UNISIM_P_3222816464 = xsi_get_engine_memory("unisim_p_3222816464");
    IEEE_P_2717149903 = xsi_get_engine_memory("ieee_p_2717149903");
    IEEE_P_1367372525 = xsi_get_engine_memory("ieee_p_1367372525");

    return xsi_run_simulation(argc, argv);

}
