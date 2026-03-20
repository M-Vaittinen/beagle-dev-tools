/* pindata.c - Static pin tables for BeagleBone Black P8 and P9 connectors
 *
 * Sources:
 *   BBB SRM Tables 7.1 (P8), 7.2 (P9), 6.7/6.8 (PRU access)
 *   TI AM335x TRM SPRUH73Q Table 9-10 (Control Module register offsets)
 *
 * Mode column assignment (MODE0..MODE7):
 *   MODE0 = primary pad function (most reliable)
 *   MODE1-MODE6 = alternate functions per BBB SRM
 *   MODE7 = GPIO (always; mux register value = 7)
 *   NULL  = mode not available / not defined for this pin
 *
 * Note: Mode assignments for some alternate functions are sourced from PDF
 * extraction and may not cover all available modes. Always verify against
 * the official BBB SRM / TRM for critical designs.
 */

#include <stddef.h>
#include <string.h>
#include <strings.h>  /* strcasecmp */
#include "pindata.h"

/* ------------------------------------------------------------------ */
/* Connector P8                                                        */
/* P8_01, P8_02 are GND; P8_03..P8_46 are the configurable pins.     */
/* ------------------------------------------------------------------ */

const pin_info_t p8_pins[] = {
    /* P8_03 - R9 - gpmc_ad6 - offset 0x818 - GPIO1[6]=38 */
    { "P8_03", "R9", "conf_gpmc_ad6", 0x818, 1, 6,
      { "gpmc_ad6", "gpmc_a8", NULL, NULL,
        "pr1_pru1_pru_r30_6", "pr1_pru1_pru_r31_6", NULL, "gpio1[6]" },
      NULL },

    /* P8_04 - T9 - gpmc_ad7 - offset 0x81C - GPIO1[7]=39 */
    { "P8_04", "T9", "conf_gpmc_ad7", 0x81C, 1, 7,
      { "gpmc_ad7", "gpmc_a10", NULL, NULL,
        "pr1_pru1_pru_r30_7", "pr1_pru1_pru_r31_7", NULL, "gpio1[7]" },
      NULL },

    /* P8_05 - R8 - gpmc_ad2 - offset 0x808 - GPIO1[2]=34 */
    { "P8_05", "R8", "conf_gpmc_ad2", 0x808, 1, 2,
      { "gpmc_ad2", "gpmc_a9", NULL, NULL,
        "pr1_pru1_pru_r30_4", "pr1_pru1_pru_r31_4", NULL, "gpio1[2]" },
      NULL },

    /* P8_06 - T8 - gpmc_ad3 - offset 0x80C - GPIO1[3]=35 */
    { "P8_06", "T8", "conf_gpmc_ad3", 0x80C, 1, 3,
      { "gpmc_ad3", "gpmc_a11", NULL, NULL,
        "pr1_pru1_pru_r30_5", "pr1_pru1_pru_r31_5", NULL, "gpio1[3]" },
      NULL },

    /* P8_07 - R7 - gpmc_advn_ale - offset 0x890 - GPIO2[2]=66 */
    { "P8_07", "R7", "conf_gpmc_advn_ale", 0x890, 2, 2,
      { "gpmc_advn_ale", "gpmc_a18", NULL, NULL,
        "pr1_pru1_pru_r30_2", "pr1_pru1_pru_r31_2", NULL, "gpio2[2]" },
      NULL },

    /* P8_08 - T7 - gpmc_oen_ren - offset 0x894 - GPIO2[3]=67 */
    { "P8_08", "T7", "conf_gpmc_oen_ren", 0x894, 2, 3,
      { "gpmc_oen_ren", "gpmc_a19", NULL, NULL,
        "pr1_pru1_pru_r30_3", "pr1_pru1_pru_r31_3", NULL, "gpio2[3]" },
      NULL },

    /* P8_09 - T6 - gpmc_be0n_cle - offset 0x89C - GPIO2[5]=69 */
    { "P8_09", "T6", "conf_gpmc_be0n_cle", 0x89C, 2, 5,
      { "gpmc_be0n_cle", "gpmc_a17", NULL, NULL,
        "pr1_pru1_pru_r30_0", "pr1_pru1_pru_r31_0", NULL, "gpio2[5]" },
      NULL },

    /* P8_10 - U6 - gpmc_wen - offset 0x898 - GPIO2[4]=68 */
    { "P8_10", "U6", "conf_gpmc_wen", 0x898, 2, 4,
      { "gpmc_wen", "gpmc_a15", NULL, NULL,
        "pr1_pru1_pru_r30_1", "pr1_pru1_pru_r31_1", NULL, "gpio2[4]" },
      NULL },

    /* P8_11 - R12 - gpmc_ad13 - offset 0x834 - GPIO1[13]=45 */
    { "P8_11", "R12", "conf_gpmc_ad13", 0x834, 1, 13,
      { "gpmc_ad13", "gpmc_a16", NULL, NULL,
        "pr1_pru0_pru_r30_15", "pr1_pru0_pru_r31_15", NULL, "gpio1[13]" },
      NULL },

    /* P8_12 - T12 - gpmc_ad12 - offset 0x830 - GPIO1[12]=44 */
    { "P8_12", "T12", "conf_gpmc_ad12", 0x830, 1, 12,
      { "gpmc_ad12", "gpmc_a14", NULL, NULL,
        "pr1_pru1_pru_r30_13", "pr1_pru1_pru_r31_13", NULL, "gpio1[12]" },
      NULL },

    /* P8_13 - T10 - gpmc_ad9 - offset 0x824 - GPIO0[23]=23 */
    { "P8_13", "T10", "conf_gpmc_ad9", 0x824, 0, 23,
      { "gpmc_ad9", "gpmc_a12", NULL, "ehrpwm2B",
        "ehrpwm2_tripzone_in", NULL, "mcasp0_fsr", "gpio0[23]" },
      NULL },

    /* P8_14 - T11 - gpmc_ad10 - offset 0x828 - GPIO0[26]=26 */
    { "P8_14", "T11", "conf_gpmc_ad10", 0x828, 0, 26,
      { "gpmc_ad10", "gpmc_a13", NULL, "ehrpwm2_tripzone_in",
        NULL, NULL, NULL, "gpio0[26]" },
      NULL },

    /* P8_15 - U13 - gpmc_ad15 - offset 0x83C - GPIO1[15]=47 */
    { "P8_15", "U13", "conf_gpmc_ad15", 0x83C, 1, 15,
      { "gpmc_ad15", "gpmc_a6", NULL, "eQEP2_strobe",
        "pr1_pru1_pru_r30_12", "pr1_pru1_pru_r31_12", NULL, "gpio1[15]" },
      NULL },

    /* P8_16 - V13 - gpmc_ad14 - offset 0x838 - GPIO1[14]=46 */
    { "P8_16", "V13", "conf_gpmc_ad14", 0x838, 1, 14,
      { "gpmc_ad14", "gpmc_a7", NULL, "eQEP2_index",
        "pr1_pru1_pru_r30_8", "pr1_pru1_pru_r31_8", NULL, "gpio1[14]" },
      NULL },

    /* P8_17 - U12 - gpmc_ad11 - offset 0x82C - GPIO0[27]=27 */
    { "P8_17", "U12", "conf_gpmc_ad11", 0x82C, 0, 27,
      { "gpmc_ad11", "gpmc_a4", NULL, NULL,
        NULL, NULL, NULL, "gpio0[27]" },
      NULL },

    /* P8_18 - V12 - gpmc_clk_mux0 - offset 0x88C - GPIO2[1]=65 */
    { "P8_18", "V12", "conf_gpmc_clk", 0x88C, 2, 1,
      { "gpmc_clk_mux0", "gpmc_a5", "gpmc_wait1", NULL,
        NULL, NULL, NULL, "gpio2[1]" },
      NULL },

    /* P8_19 - U10 - gpmc_ad8 - offset 0x820 - GPIO0[22]=22 */
    { "P8_19", "U10", "conf_gpmc_ad8", 0x820, 0, 22,
      { "gpmc_ad8", "gpmc_a2", "mmc1_dat0", "ehrpwm2A",
        NULL, NULL, "mcasp0_axr1", "gpio0[22]" },
      NULL },

    /* P8_20 - V9 - gpmc_csn2 - offset 0x884 - GPIO1[31]=63 */
    { "P8_20", "V9", "conf_gpmc_csn2", 0x884, 1, 31,
      { "gpmc_csn2", "gpmc_a3", "mmc1_cmd", NULL,
        "pr1_pru1_pru_r30_10", "pr1_pru1_pru_r31_10", NULL, "gpio1[31]" },
      "Also accessible as pr1_pru0_pru_r30_14" },

    /* P8_21 - U9 - gpmc_csn1 - offset 0x880 - GPIO1[30]=62 */
    { "P8_21", "U9", "conf_gpmc_csn1", 0x880, 1, 30,
      { "gpmc_csn1", "gpmc_a0", "mmc1_clk", NULL,
        "pr1_pru1_pru_r30_9", "pr1_pru1_pru_r31_9", NULL, "gpio1[30]" },
      NULL },

    /* P8_22 - V8 - gpmc_ad5 - offset 0x814 - GPIO1[5]=37 */
    { "P8_22", "V8", "conf_gpmc_ad5", 0x814, 1, 5,
      { "gpmc_ad5", "gpmc_a2", "mmc1_dat3", NULL,
        NULL, NULL, NULL, "gpio1[5]" },
      NULL },

    /* P8_23 - U8 - gpmc_ad4 - offset 0x810 - GPIO1[4]=36 */
    { "P8_23", "U8", "conf_gpmc_ad4", 0x810, 1, 4,
      { "gpmc_ad4", "gpmc_a3", "mmc1_dat2", NULL,
        NULL, NULL, NULL, "gpio1[4]" },
      NULL },

    /* P8_24 - V7 - gpmc_ad1 - offset 0x804 - GPIO1[1]=33 */
    { "P8_24", "V7", "conf_gpmc_ad1", 0x804, 1, 1,
      { "gpmc_ad1", "gpmc_a0", "mmc1_dat7", NULL,
        NULL, NULL, NULL, "gpio1[1]" },
      NULL },

    /* P8_25 - U7 - gpmc_ad0 - offset 0x800 - GPIO1[0]=32 */
    { "P8_25", "U7", "conf_gpmc_ad0", 0x800, 1, 0,
      { "gpmc_ad0", "gpmc_a1", "mmc1_dat6", NULL,
        NULL, NULL, NULL, "gpio1[0]" },
      NULL },

    /* P8_26 - V6 - gpmc_csn0 - offset 0x87C - GPIO1[29]=61 */
    { "P8_26", "V6", "conf_gpmc_csn0", 0x87C, 1, 29,
      { "gpmc_csn0", NULL, NULL, NULL,
        NULL, NULL, NULL, "gpio1[29]" },
      NULL },

    /* P8_27 - U5 - lcd_vsync - offset 0x8E0 - GPIO2[22]=86 */
    { "P8_27", "U5", "conf_lcd_vsync", 0x8E0, 2, 22,
      { "lcd_vsync", "gpmc_a8", "lcd_data18", NULL,
        "pr1_pru1_pru_r30_6", "pr1_pru1_pru_r31_6", NULL, "gpio2[22]" },
      NULL },

    /* P8_28 - V5 - lcd_pclk - offset 0x8E8 - GPIO2[24]=88 */
    { "P8_28", "V5", "conf_lcd_pclk", 0x8E8, 2, 24,
      { "lcd_pclk", "gpmc_a10", "lcd_data19", NULL,
        "pr1_pru1_pru_r30_7", "pr1_pru1_pru_r31_7", NULL, "gpio2[24]" },
      NULL },

    /* P8_29 - R5 - lcd_hsync - offset 0x8E4 - GPIO2[23]=87 */
    { "P8_29", "R5", "conf_lcd_hsync", 0x8E4, 2, 23,
      { "lcd_hsync", "gpmc_a9", "lcd_data22", NULL,
        "pr1_pru1_pru_r30_4", "pr1_pru1_pru_r31_4", NULL, "gpio2[23]" },
      NULL },

    /* P8_30 - R6 - lcd_ac_bias_en - offset 0x8EC - GPIO2[25]=89 */
    { "P8_30", "R6", "conf_lcd_ac_bias_en", 0x8EC, 2, 25,
      { "lcd_ac_bias_en", "gpmc_a11", "lcd_data21", NULL,
        NULL, NULL, NULL, "gpio2[25]" },
      NULL },

    /* P8_31 - V4 - lcd_data14 - offset 0x8D8 - GPIO0[10]=10 */
    { "P8_31", "V4", "conf_lcd_data14", 0x8D8, 0, 10,
      { "lcd_data14", "gpmc_a16", "lcd_data16", "mmc2_dat1",
        "uart5_ctsn", NULL, NULL, "gpio0[10]" },
      NULL },

    /* P8_32 - T5 - lcd_data15 - offset 0x8DC - GPIO0[11]=11 */
    { "P8_32", "T5", "conf_lcd_data15", 0x8DC, 0, 11,
      { "lcd_data15", "gpmc_a17", "lcd_data17", "mmc2_dat0",
        "uart5_rtsn", NULL, NULL, "gpio0[11]" },
      NULL },

    /* P8_33 - V3 - lcd_data13 - offset 0x8D4 - GPIO0[9]=9 */
    { "P8_33", "V3", "conf_lcd_data13", 0x8D4, 0, 9,
      { "lcd_data13", "gpmc_a15", "lcd_data20", "mmc2_dat5",
        "uart4_rtsn", NULL, NULL, "gpio0[9]" },
      NULL },

    /* P8_34 - U4 - lcd_data11 - offset 0x8CC - GPIO2[17]=81 */
    { "P8_34", "U4", "conf_lcd_data11", 0x8CC, 2, 17,
      { "lcd_data11", "gpmc_a13", "lcd_data22", "mmc2_dat6",
        "uart3_rtsn", NULL, NULL, "gpio2[17]" },
      NULL },

    /* P8_35 - V2 - lcd_data12 - offset 0x8D0 - GPIO0[8]=8 */
    { "P8_35", "V2", "conf_lcd_data12", 0x8D0, 0, 8,
      { "lcd_data12", "gpmc_a14", "lcd_data21", "mmc2_dat3",
        "uart4_ctsn", NULL, NULL, "gpio0[8]" },
      NULL },

    /* P8_36 - U3 - lcd_data10 - offset 0x8C8 - GPIO2[16]=80 */
    { "P8_36", "U3", "conf_lcd_data10", 0x8C8, 2, 16,
      { "lcd_data10", "gpmc_a12", "lcd_data20", "mmc2_dat2",
        "uart3_ctsn", NULL, NULL, "gpio2[16]" },
      NULL },

    /* P8_37 - U1 - lcd_data8 - offset 0x8C0 - GPIO2[14]=78 */
    { "P8_37", "U1", "conf_lcd_data8", 0x8C0, 2, 14,
      { "lcd_data8", "gpmc_a6", "lcd_data16", "mmc2_dat7",
        "uart5_txd", NULL, NULL, "gpio2[14]" },
      NULL },

    /* P8_38 - U2 - lcd_data9 - offset 0x8C4 - GPIO2[15]=79 */
    { "P8_38", "U2", "conf_lcd_data9", 0x8C4, 2, 15,
      { "lcd_data9", "gpmc_a7", "lcd_data17", "mmc2_clk",
        "uart5_rxd", NULL, NULL, "gpio2[15]" },
      NULL },

    /* P8_39 - T3 - lcd_data6 - offset 0x8B8 - GPIO2[12]=76 */
    { "P8_39", "T3", "conf_lcd_data6", 0x8B8, 2, 12,
      { "lcd_data6", "gpmc_a4", "lcd_data23", "mmc2_dat4",
        "pr1_pru1_pru_r30_5", "pr1_pru1_pru_r31_5", NULL, "gpio2[12]" },
      NULL },

    /* P8_40 - T4 - lcd_data7 - offset 0x8BC - GPIO2[13]=77 */
    { "P8_40", "T4", "conf_lcd_data7", 0x8BC, 2, 13,
      { "lcd_data7", "gpmc_a5", "gpmc_be1n", NULL,
        "pr1_pru1_pru_r30_2", "pr1_pru1_pru_r31_2", NULL, "gpio2[13]" },
      NULL },

    /* P8_41 - T1 - lcd_data4 - offset 0x8B0 - GPIO2[10]=74 */
    { "P8_41", "T1", "conf_lcd_data4", 0x8B0, 2, 10,
      { "lcd_data4", "gpmc_a2", "mmc1_dat5", "mmc2_dat3",
        "pr1_pru1_pru_r30_3", "pr1_pru1_pru_r31_3", NULL, "gpio2[10]" },
      NULL },

    /* P8_42 - T2 - lcd_data5 - offset 0x8B4 - GPIO2[11]=75 */
    { "P8_42", "T2", "conf_lcd_data5", 0x8B4, 2, 11,
      { "lcd_data5", "gpmc_a3", "mmc1_dat4", "mmc2_dat2",
        "pr1_pru1_pru_r30_0", "pr1_pru1_pru_r31_0", NULL, "gpio2[11]" },
      NULL },

    /* P8_43 - R3 - lcd_data2 - offset 0x8A8 - GPIO2[8]=72 */
    { "P8_43", "R3", "conf_lcd_data2", 0x8A8, 2, 8,
      { "lcd_data2", "gpmc_a0", "mmc1_dat1", "mmc2_dat1",
        "pr1_pru1_pru_r30_1", "pr1_pru1_pru_r31_1", NULL, "gpio2[8]" },
      NULL },

    /* P8_44 - R4 - lcd_data3 - offset 0x8AC - GPIO2[9]=73 */
    { "P8_44", "R4", "conf_lcd_data3", 0x8AC, 2, 9,
      { "lcd_data3", "gpmc_a1", "mmc1_dat0", "mmc2_dat0",
        NULL, NULL, NULL, "gpio2[9]" },
      NULL },

    /* P8_45 - R1 - lcd_data0 - offset 0x8A0 - GPIO2[6]=70 */
    { "P8_45", "R1", "conf_lcd_data0", 0x8A0, 2, 6,
      { "lcd_data0", "gpmc_a18", "mmc1_dat1", "mmc2_dat5",
        "ehrpwm2A", NULL, NULL, "gpio2[6]" },
      NULL },

    /* P8_46 - R2 - lcd_data1 - offset 0x8A4 - GPIO2[7]=71 */
    { "P8_46", "R2", "conf_lcd_data1", 0x8A4, 2, 7,
      { "lcd_data1", "gpmc_a19", "mmc1_dat3", "mmc2_dat7",
        NULL, NULL, NULL, "gpio2[7]" },
      NULL },
};

const int p8_pin_count = (int)(sizeof(p8_pins) / sizeof(p8_pins[0]));

/* ------------------------------------------------------------------ */
/* Connector P9                                                        */
/* P9_01/02=GND, P9_03/04=3.3V, P9_05/06=VDD_5V, P9_07/08=SYS_5V   */
/* P9_09=PWR_BUT, P9_10=SYS_RESETn, P9_32=VADC, P9_33-40=ADC-only  */
/* P9_34=AGND, P9_43-46=GND                                          */
/* Configurable (pinmux) pins listed below.                           */
/* ------------------------------------------------------------------ */

const pin_info_t p9_pins[] = {
    /* P9_11 - T17 - gpmc_wait0 - offset 0x870 - GPIO0[30]=30 */
    { "P9_11", "T17", "conf_gpmc_wait0", 0x870, 0, 30,
      { "gpmc_wait0", "mii2_crs", "gpmc_csn4", "rmii2_crs_dv",
        "mmc1_sdcd", "pr1_uart0_rxd", NULL, "gpio0[30]" },
      NULL },

    /* P9_12 - U18 - gpmc_be1n - offset 0x878 - GPIO1[28]=60 */
    { "P9_12", "U18", "conf_gpmc_ben1", 0x878, 1, 28,
      { "gpmc_be1n", "mii2_col", "gpmc_csn6", "mmc2_dat3",
        "gpmc_dir", "pr1_uart0_txd", NULL, "gpio1[28]" },
      NULL },

    /* P9_13 - U17 - gpmc_wpn - offset 0x874 - GPIO0[31]=31 */
    { "P9_13", "U17", "conf_gpmc_wpn", 0x874, 0, 31,
      { "gpmc_wpn", "mii_rxerr", "gpmc_csn5", "rmii2_rxerr",
        "mmc2_sdcd", "pr1_uart0_rxd", NULL, "gpio0[31]" },
      NULL },

    /* P9_14 - U14 - gpmc_a2 - offset 0x848 - GPIO1[18]=50 */
    { "P9_14", "U14", "conf_gpmc_a2", 0x848, 1, 18,
      { "gpmc_a2", "rgmii2_td3", NULL, "mmc2_dat1",
        "mmc1_sdcd", "pr1_uart0_rts_n", "ehrpwm1A_mux1", "gpio1[18]" },
      NULL },

    /* P9_15 - R13 - gpmc_a0 - offset 0x840 - GPIO1[16]=48 */
    { "P9_15", "R13", "conf_gpmc_a0", 0x840, 1, 16,
      { "gpmc_a0", "gmii2_txen", "rgmii2_txen", "mii2_txen",
        "mmc0_sdcd_mux1", NULL, NULL, "gpio1[16]" },
      NULL },

    /* P9_16 - T14 - gpmc_a3 - offset 0x84C - GPIO1[19]=51 */
    { "P9_16", "T14", "conf_gpmc_a3", 0x84C, 1, 19,
      { "gpmc_a3", "mii2_txd2", "rgmii2_td2", "mmc2_dat2",
        "mmc2_sdcd", "pr1_uart0_cts_n", "ehrpwm1B_mux1", "gpio1[19]" },
      NULL },

    /* P9_17 - A16 - spi0_cs0 - offset 0x95C - GPIO0[5]=5 */
    { "P9_17", "A16", "conf_spi0_cs0", 0x95C, 0, 5,
      { "spi0_cs0", "mmc2_sdwp", "I2C1_SCL", "ehrpwm0_synci",
        NULL, "pr1_uart0_txd", NULL, "gpio0[5]" },
      NULL },

    /* P9_18 - B16 - spi0_d1 - offset 0x958 - GPIO0[4]=4 */
    { "P9_18", "B16", "conf_spi0_d1", 0x958, 0, 4,
      { "spi0_d1", "mmc1_sdwp", "I2C1_SDA", "ehrpwm0_tripzone_in",
        NULL, "pr1_uart0_rxd", NULL, "gpio0[4]" },
      NULL },

    /* P9_19 - D17 - uart1_rtsn - offset 0x97C - GPIO0[13]=13 */
    { "P9_19", "D17", "conf_uart1_rtsn", 0x97C, 0, 13,
      { "uart1_rtsn", "timer5", "dcan0_rx", "I2C2_SCL",
        NULL, "pr1_uart0_rts_n", NULL, "gpio0[13]" },
      NULL },

    /* P9_20 - D18 - uart1_ctsn - offset 0x978 - GPIO0[12]=12 */
    { "P9_20", "D18", "conf_uart1_ctsn", 0x978, 0, 12,
      { "uart1_ctsn", "timer6", "dcan0_tx", "I2C2_SDA",
        NULL, "pr1_uart0_cts_n", NULL, "gpio0[12]" },
      NULL },

    /* P9_21 - B17 - spi0_d0 - offset 0x954 - GPIO0[3]=3 */
    { "P9_21", "B17", "conf_spi0_d0", 0x954, 0, 3,
      { "spi0_d0", "uart2_txd", "I2C2_SCL", "ehrpwm0B",
        NULL, "pr1_uart0_rts_n", NULL, "gpio0[3]" },
      NULL },

    /* P9_22 - A17 - spi0_sclk - offset 0x950 - GPIO0[2]=2 */
    { "P9_22", "A17", "conf_spi0_sclk", 0x950, 0, 2,
      { "spi0_sclk", "uart2_rxd", "I2C2_SDA", "ehrpwm0A",
        NULL, "pr1_uart0_cts_n", NULL, "gpio0[2]" },
      NULL },

    /* P9_23 - V14 - gpmc_a1 - offset 0x844 - GPIO1[17]=49 */
    { "P9_23", "V14", "conf_gpmc_a1", 0x844, 1, 17,
      { "gpmc_a1", "gmii2_rxdv", "rgmii2_rxdv", "mmc2_dat0",
        "gpmc_a17", NULL, NULL, "gpio1[17]" },
      NULL },

    /* P9_24 - D15 - uart1_txd - offset 0x984 - GPIO0[15]=15 */
    { "P9_24", "D15", "conf_uart1_txd", 0x984, 0, 15,
      { "uart1_txd", "mmc2_sdwp", "dcan1_rx", "I2C1_SCL",
        NULL, "pr1_uart0_cts_n", NULL, "gpio0[15]" },
      NULL },

    /* P9_25 - A14 - mcasp0_ahclkx - offset 0x9AC - GPIO3[21]=117 */
    { "P9_25", "A14", "conf_mcasp0_ahclkx", 0x9AC, 3, 21,
      { "mcasp0_ahclkx", "eQEP0_strobe", "mcasp0_axr3", "mcasp1_axr1",
        NULL, "pr1_pru0_pru_r30_7", "pr1_pru0_pru_r31_7", "gpio3[21]" },
      "Also 24.576MHz HDMI audio clock input; disable oscillator before GPIO use" },

    /* P9_26 - D16 - uart1_rxd - offset 0x980 - GPIO0[14]=14 */
    { "P9_26", "D16", "conf_uart1_rxd", 0x980, 0, 14,
      { "uart1_rxd", "mmc1_sdwp", "dcan1_tx", "I2C1_SDA",
        NULL, "pr1_uart0_txd", NULL, "gpio0[14]" },
      NULL },

    /* P9_27 - C13 - mcasp0_fsr - offset 0x9A4 - GPIO3[19]=115 */
    { "P9_27", "C13", "conf_mcasp0_fsr", 0x9A4, 3, 19,
      { "mcasp0_fsr", "eQEP0B_in", "mcasp0_axr3", "mcasp1_fsx",
        NULL, "pr1_pru0_pru_r30_5", "pr1_pru0_pru_r31_5", "gpio3[19]" },
      NULL },

    /* P9_28 - C12 - mcasp0_ahclkr - offset 0x99C - GPIO3[17]=113 */
    { "P9_28", "C12", "conf_mcasp0_ahclkr", 0x99C, 3, 17,
      { "mcasp0_ahclkr", "ehrpwm0_synci", "mcasp0_axr2", "spi1_cs0",
        NULL, "pr1_pru0_pru_r30_3", "pr1_pru0_pru_r31_3", "gpio3[17]" },
      NULL },

    /* P9_29 - B13 - mcasp0_fsx - offset 0x994 - GPIO3[15]=111 */
    { "P9_29", "B13", "conf_mcasp0_fsx", 0x994, 3, 15,
      { "mcasp0_fsx", "ehrpwm0B", NULL, "spi1_d0",
        NULL, "pr1_pru0_pru_r30_1", "pr1_pru0_pru_r31_1", "gpio3[15]" },
      NULL },

    /* P9_30 - D12 - mcasp0_axr0 - offset 0x998 - GPIO3[16]=112 */
    { "P9_30", "D12", "conf_mcasp0_axr0", 0x998, 3, 16,
      { "mcasp0_axr0", "ehrpwm0_tripzone_in", NULL, "spi1_d1",
        NULL, "pr1_pru0_pru_r30_2", "pr1_pru0_pru_r31_2", "gpio3[16]" },
      NULL },

    /* P9_31 - A13 - mcasp0_aclkx - offset 0x990 - GPIO3[14]=110 */
    { "P9_31", "A13", "conf_mcasp0_aclkx", 0x990, 3, 14,
      { "mcasp0_aclkx", "ehrpwm0A", NULL, "spi1_sclk",
        NULL, "pr1_pru0_pru_r30_0", "pr1_pru0_pru_r31_0", "gpio3[14]" },
      NULL },

    /* P9_41 - D14 - xdma_event_intr1 - offset 0x9B4 - GPIO0[20]=20 */
    { "P9_41", "D14", "conf_xdma_event_intr1", 0x9B4, 0, 20,
      { "xdma_event_intr1", "eQEP0_index", "tclkin", "clkout2",
        "timer7_mux1", "pr1_pru0_pru_r30_6", "pr1_pru0_pru_r31_6", "gpio0[20]" },
      "Primary GPIO0[20]; GPIO3[20] also accessible via R221 (remove to disconnect)" },

    /* P9_42 - D13 - ecap0_in_pwm0_out - offset 0x964 - GPIO0[7]=7 */
    { "P9_42", "D13", "conf_ecap0_in_pwm0_out", 0x964, 0, 7,
      { "ecap0_in_pwm0_out", "uart3_txd", NULL, "pr1_ecap0_ecap_capin_apwm_o",
        "spi1_sclk", NULL, "pr1_pru0_pru_r30_4", "gpio0[7]" },
      "Primary GPIO0[7]; GPIO3[18] also accessible via R202 (remove to disconnect)" },
};

const int p9_pin_count = (int)(sizeof(p9_pins) / sizeof(p9_pins[0]));

/* ------------------------------------------------------------------ */
/* Lookup helpers                                                      */
/* ------------------------------------------------------------------ */

const pin_info_t *pin_find(const char *name)
{
    int i;
    if (!name)
        return NULL;

    for (i = 0; i < p8_pin_count; i++) {
        if (strcasecmp(p8_pins[i].name, name) == 0)
            return &p8_pins[i];
    }
    for (i = 0; i < p9_pin_count; i++) {
        if (strcasecmp(p9_pins[i].name, name) == 0)
            return &p9_pins[i];
    }
    return NULL;
}

uint32_t gpio_base(int bank)
{
    switch (bank) {
    case 0: return GPIO0_BASE;
    case 1: return GPIO1_BASE;
    case 2: return GPIO2_BASE;
    case 3: return GPIO3_BASE;
    default: return 0;
    }
}
