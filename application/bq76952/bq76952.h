#ifndef BQ76952_H
#define BQ76952_H

#define CMD_CONTROL_STATUS (0x00)
#define CMD_SAFETY_ALERT_A (0x02)
#define CMD_SAFETY_STATUS_A (0x03)
#define CMD_SAFETY_ALERT_B (0x04)
#define CMD_SAFETY_STATUS_B (0x05)
#define CMD_SAFETY_ALERT_C (0x06)
#define CMD_SAFETY_STATUS_C (0x07)
#define CMD_PF_ALERT_A (0x0A)
#define CMD_PF_STATUS_A (0x0B)
#define CMD_PF_ALERT_B (0x0C)
#define CMD_PF_STATUS_B (0x0D)
#define CMD_PF_ALERT_C (0x0E)
#define CMD_PF_STATUS_C (0x0F)
#define CMD_PF_ALERT_D (0x10)
#define CMD_PF_STATUS_D (0x11)
#define CMD_BATTERY_STATUS (0x12)
#define CMD_VOLTAGE_CELL_1 (0x14)
#define CMD_VOLTAGE_CELL_2 (0x16)
#define CMD_VOLTAGE_CELL_3 (0x18)
#define CMD_VOLTAGE_CELL_4 (0x1A)
#define CMD_VOLTAGE_CELL_5 (0x1C)
#define CMD_VOLTAGE_CELL_6 (0x1E)
#define CMD_VOLTAGE_CELL_7 (0x20)
#define CMD_VOLTAGE_CELL_8 (0x22)
#define CMD_VOLTAGE_CELL_9 (0x24)
#define CMD_VOLTAGE_CELL_10 (0x26)
#define CMD_VOLTAGE_CELL_11 (0x28)
#define CMD_VOLTAGE_CELL_12 (0x2A)
#define CMD_VOLTAGE_CELL_13 (0x2C)
#define CMD_VOLTAGE_CELL_14 (0x2E)
#define CMD_VOLTAGE_CELL_15 (0x30)
#define CMD_VOLTAGE_CELL_16 (0x32)
#define CMD_VOLTAGE_STACK (0x34)
#define CMD_VOLTAGE_PACK (0x36)
#define CMD_VOLTAGE_LD (0x38)
#define CMD_CURRENT_CC2 (0x3A)
#define CMD_SUBCMD_LOWER (0x3E)
#define CMD_SUBCMD_UPPER (0x3F)
#define SUBCMD_DATA_START (0x40)
#define SUBCMD_DATA_CHECKSUM (0x60)
#define SUBCMD_DATA_LENGTH (0x61)
#define CMD_ALARM_STATUS (0x62)
#define CMD_ALARM_RAW_STATUS (0x64)
#define CMD_ALARM_ENABLE (0x66)
#define CMD_TEMP_INT (0x68)
#define CMD_TEMP_CFETOFF (0x6A)
#define CMD_TEMP_DFETOFF (0x6C)
#define CMD_TEMP_ALERT (0x6E)
#define CMD_TEMP_TS1 (0x70)
#define CMD_TEMP_TS2 (0x72)
#define CMD_TEMP_TS3 (0x74)
#define CMD_TEMP_HDQ (0x76)
#define CMD_TEMP_DCHG (0x78)
#define CMD_TEMP_DDSG (0x7A)
#define CMD_FET_STATUS (0x7F)

/* subcmd + checksum + length bytes */
#define SUBCMD_OVERHEAD_BYTES (4)

/* Command-only subcommands */

#define SUBCMD_EXIT_DEEPSLEEP (0x000E)
#define SUBCMD_DEEPSLEEP (0x000F)
#define SUBCMD_SHUTDOWN (0x0010)
#define SUBCMD_RESET (0x0012)
#define SUBCMD_PDSGTEST (0x001C)
#define SUBCMD_FUSE_TOGGLE (0x001D)
#define SUBCMD_PCHGTEST (0x001E)
#define SUBCMD_CHGTEST (0x001F)
#define SUBCMD_DSGTEST (0x0020)
#define SUBCMD_FET_ENABLE (0x0022)
#define SUBCMD_PF_ENABLE (0x0024)

/* PF_RESET (0x0029) is deprecated according to Reference Manual Rev. B */

#define SUBCMD_SEAL (0x0030)
#define SUBCMD_RESET_PASSQ (0x0082)
#define SUBCMD_PTO_RECOVER (0x008A)
#define SUBCMD_SET_CFGUPDATE (0x0090)
#define SUBCMD_EXIT_CFGUPDATE (0x0092)
#define SUBCMD_DSG_PDSG_OFF (0x0093)
#define SUBCMD_CHG_PCHG_OFF (0x0094)
#define SUBCMD_ALL_FETS_OFF (0x0095)
#define SUBCMD_ALL_FETS_ON (0x0096)
#define SUBCMD_SLEEP_ENABLE (0x0099)
#define SUBCMD_SLEEP_DISABLE (0x009A)
#define SUBCMD_OCDL_RECOVER (0x009B)
#define SUBCMD_SCDL_RECOVER (0x009C)
#define SUBCMD_LOAD_DETECT_RESTART (0x009D)
#define SUBCMD_LOAD_DETECT_ON (0x009E)
#define SUBCMD_LOAD_DETECT_OFF (0x009F)
#define SUBCMD_CFETOFF_LO (0x2800)
#define SUBCMD_DFETOFF_LO (0x2801)
#define SUBCMD_ALERT_LO (0x2802)
#define SUBCMD_HDQ_LO (0x2806)
#define SUBCMD_DCHG_LO (0x2807)
#define SUBCMD_DDSG_LO (0x2808)
#define SUBCMD_CFETOFF_HI (0x2810)
#define SUBCMD_DFETOFF_HI (0x2811)
#define SUBCMD_ALERT_HI (0x2812)
#define SUBCMD_HDQ_HI (0x2816)
#define SUBCMD_DCHG_HI (0x2817)
#define SUBCMD_DDSG_HI (0x2818)
#define SUBCMD_PF_FORCE_A (0x2857)
#define SUBCMD_PF_FORCE_B (0x29A3)
#define SUBCMD_SWAP_COMM_MODE (0x29BC)
#define SUBCMD_SWAP_TO_I2C (0x29E7)
#define SUBCMD_SWAP_TO_SPI (0x7C35)
#define SUBCMD_SWAP_TO_HDQ (0x7C40)

/* Subcommands with data */

#define SUBCMD_DEVICE_NUMBER (0x0001)
#define SUBCMD_FW_VERSION (0x0002)
#define SUBCMD_HW_VERSION (0x0003)
#define SUBCMD_IROM_SIG (0x0004)
#define SUBCMD_STATIC_CFG_SIG (0x0005)
#define SUBCMD_PREV_MACWRITE (0x0007)
#define SUBCMD_DROM_SIG (0x0009)
#define SUBCMD_SECURITY_KEYS (0x0035)
#define SUBCMD_SAVED_PF_STATUS (0x0053)
#define SUBCMD_MFG_STATUS (0x0057)
#define SUBCMD_MANU_DATA (0x0070)
#define SUBCMD_DASTATUS1 (0x0071)
#define SUBCMD_DASTATUS2 (0x0072)
#define SUBCMD_DASTATUS3 (0x0073)
#define SUBCMD_DASTATUS4 (0x0074)
#define SUBCMD_DASTATUS5 (0x0075)
#define SUBCMD_DASTATUS6 (0x0076)
#define SUBCMD_DASTATUS7 (0x0077)
#define SUBCMD_CUV_SNAPSHOT (0x0080)
#define SUBCMD_COV_SNAPSHOT (0x0081)
#define SUBCMD_CB_ACTIVE_CELLS (0x0083)
#define SUBCMD_CB_SET_LVL (0x0084)
#define SUBCMD_CBSTATUS1 (0x0085)
#define SUBCMD_CBSTATUS2 (0x0086)
#define SUBCMD_CBSTATUS3 (0x0087)
#define SUBCMD_FET_CONTROL (0x0097)
#define SUBCMD_REG12_CONTROL (0x0098)
#define SUBCMD_OTP_WR_CHECK (0x00a0)
#define SUBCMD_OTP_WRITE (0x00a1)
#define SUBCMD_READ_CAL1 (0xf081)
#define SUBCMD_CAL_CUV (0xf090)
#define SUBCMD_CAL_COV (0xf091)

/* Data Memory */

#define IS_DATA_MEM_REG_ADDR(addr) (addr >= 0x9180 && addr < 0x9400)

/* Calibration (manual section 13.2) */

#define CAL_VOLT_CELL1_GAIN (0x9180)
#define CAL_VOLT_CELL2_GAIN (0x9182)
#define CAL_VOLT_CELL3_GAIN (0x9184)
#define CAL_VOLT_CELL4_GAIN (0x9186)
#define CAL_VOLT_CELL5_GAIN (0x9188)
#define CAL_VOLT_CELL6_GAIN (0x918A)
#define CAL_VOLT_CELL7_GAIN (0x918C)
#define CAL_VOLT_CELL8_GAIN (0x918E)
#define CAL_VOLT_CELL9_GAIN (0x9190)
#define CAL_VOLT_CELL10_GAIN (0x9192)
#define CAL_VOLT_CELL11_GAIN (0x9194)
#define CAL_VOLT_CELL12_GAIN (0x9196)
#define CAL_VOLT_CELL13_GAIN (0x9198)
#define CAL_VOLT_CELL14_GAIN (0x919A)
#define CAL_VOLT_CELL15_GAIN (0x919C)
#define CAL_VOLT_CELL16_GAIN (0x919E)
#define CAL_VOLT_PACK_GAIN (0x91A0)
#define CAL_VOLT_TOS_GAIN (0x91A2)
#define CAL_VOLT_LD_GAIN (0x91A4)
#define CAL_VOLT_ADC_GAIN (0x91A6)
#define CAL_CURR_CC_GAIN (0x91A8)
#define CAL_CURR_CAP_GAIN (0x91AC)
#define CAL_VCELL_OFFSET (0x91B0)
#define CAL_VDIV_OFFSET (0x91B2)
#define CAL_CURR_OFFSET_CC (0x91C6)
#define CAL_CURR_OFFSET_BOARD (0x91C8)
#define CAL_TEMP_INT_OFFSET (0x91CA)
#define CAL_TEMP_CFETOFF_OFFSET (0x91CB)
#define CAL_TEMP_DFETOFF_OFFSET (0x91CC)
#define CAL_TEMP_ALERT_OFFSET (0x91CD)
#define CAL_TEMP_TS1_OFFSET (0x91CE)
#define CAL_TEMP_TS2_OFFSET (0x91CF)
#define CAL_TEMP_TS3_OFFSET (0x91D0)
#define CAL_TEMP_HDQ_OFFSET (0x91D1)
#define CAL_TEMP_DCHG_OFFSET (0x91D2)
#define CAL_TEMP_DDSG_OFFSET (0x91D3)
#define CAL_INT_TEMP_GAIN (0x91E2)
#define CAL_INT_TEMP_BASE_OFFSET (0x91E4)
#define CAL_INT_TEMP_MAX_AD (0x91E6)
#define CAL_INT_TEMP_MAX_TEMP (0x91E8)
#define CAL_18K_TEMP_COEFF_A1 (0x91EA)
#define CAL_18K_TEMP_COEFF_A2 (0x91EC)
#define CAL_18K_TEMP_COEFF_A3 (0x91EE)
#define CAL_18K_TEMP_COEFF_A4 (0x91F0)
#define CAL_18K_TEMP_COEFF_A5 (0x91F2)
#define CAL_18K_TEMP_COEFF_B1 (0x91F4)
#define CAL_18K_TEMP_COEFF_B2 (0x91F6)
#define CAL_18K_TEMP_COEFF_B3 (0x91F8)
#define CAL_18K_TEMP_COEFF_B4 (0x91FA)
#define CAL_18K_TEMP_COEFF_ADC0 (0x91FE)
#define CAL_180K_TEMP_COEFF_A1 (0x9200)
#define CAL_180K_TEMP_COEFF_A2 (0x9202)
#define CAL_180K_TEMP_COEFF_A3 (0x9204)
#define CAL_180K_TEMP_COEFF_A4 (0x9206)
#define CAL_180K_TEMP_COEFF_A5 (0x9208)
#define CAL_180K_TEMP_COEFF_B1 (0x920A)
#define CAL_180K_TEMP_COEFF_B2 (0x920C)
#define CAL_180K_TEMP_COEFF_B3 (0x920E)
#define CAL_180K_TEMP_COEFF_B4 (0x9210)
#define CAL_180K_TEMP_COEFF_ADC0 (0x9214)
#define CAL_CUST_TEMP_COEFF_A1 (0x9216)
#define CAL_CUST_TEMP_COEFF_A2 (0x9218)
#define CAL_CUST_TEMP_COEFF_A3 (0x921A)
#define CAL_CUST_TEMP_COEFF_A4 (0x921C)
#define CAL_CUST_TEMP_COEFF_A5 (0x921E)
#define CAL_CUST_TEMP_COEFF_B1 (0x9220)
#define CAL_CUST_TEMP_COEFF_B2 (0x9222)
#define CAL_CUST_TEMP_COEFF_B3 (0x9224)
#define CAL_CUST_TEMP_COEFF_B4 (0x9226)
#define CAL_CUST_TEMP_COEFF_RC0 (0x9228)
#define CAL_CUST_TEMP_COEFF_ADC0 (0x922A)
#define CAL_CURR_CC_DEADBAND (0x922D)
#define CAL_CUV_TH_OVERRIDE (0x91D4)
#define CAL_COV_TH_OVERRIDE (0x91D6)

/* Settings (manual section 13.3) */

#define SET_FUSE_BLOW_VOLTAGE (0x9231)
#define SET_FUSE_BLOW_TIMEOUT (0x9233)
#define SET_CONF_POWER (0x9234)
#define SET_CONF_REG12 (0x9236)
#define SET_CONF_REG0 (0x9237)
#define SET_CONF_HWD_REG_OPT (0x9238)
#define SET_CONF_COMM_TYPE (0x9239)
#define SET_CONF_I2C_ADDR (0x923A)
#define SET_CONF_SPI (0x923C)
#define SET_CONF_COMM_IDLE (0x923D)
#define SET_CONF_CFETOFF (0x92FA)
#define SET_CONF_DFETOFF (0x92FB)
#define SET_CONF_ALERT (0x92FC)
#define SET_CONF_TS1 (0x92FD)
#define SET_CONF_TS2 (0x92FE)
#define SET_CONF_TS3 (0x92FF)
#define SET_CONF_HDQ (0x9300)
#define SET_CONF_DCHG (0x9301)
#define SET_CONF_DDSG (0x9302)
#define SET_CONF_DA (0x9303)
#define SET_CONF_VCELL_MODE (0x9304)
#define SET_CONF_CC3_SAMPLES (0x9307)
#define SET_PROT_CONF (0x925F)
#define SET_PROT_ENABLED_A (0x9261)
#define SET_PROT_ENABLED_B (0x9262)
#define SET_PROT_ENABLED_C (0x9263)
#define SET_PROT_CHG_FET_A (0x9265)
#define SET_PROT_CHG_FET_B (0x9266)
#define SET_PROT_CHG_FET_C (0x9267)
#define SET_PROT_DSG_FET_A (0x9269)
#define SET_PROT_DSG_FET_B (0x926A)
#define SET_PROT_DSG_FET_C (0x926B)
#define SET_PROT_BODY_DIODE_TH (0x9273)
#define SET_ALARM_DEFAULT_MASK (0x926D)
#define SET_ALARM_SF_ALERT_MASK_A (0x926F)
#define SET_ALARM_SF_ALERT_MASK_B (0x9270)
#define SET_ALARM_SF_ALERT_MASK_C (0x9271)
#define SET_ALARM_PF_ALERT_MASK_A (0x92C4)
#define SET_ALARM_PF_ALERT_MASK_B (0x92C5)
#define SET_ALARM_PF_ALERT_MASK_C (0x92C6)
#define SET_ALARM_PF_ALERT_MASK_D (0x92C7)
#define SET_PF_ENABLED_A (0x92C0)
#define SET_PF_ENABLED_B (0x92C1)
#define SET_PF_ENABLED_C (0x92C2)
#define SET_PF_ENABLED_D (0x92C3)
#define SET_FET_OPTIONS (0x9308)
#define SET_FET_CHG_PUMP (0x9309)
#define SET_FET_PCHG_START_V (0x930A)     /* mV */
#define SET_FET_PCHG_STOP_V (0x930C)      /* mV */
#define SET_FET_PDSG_TIMEOUT (0x930E)     /* 10 ms */
#define SET_FET_PDSG_STOP_DV (0x930F)     /* 10 mV */
#define SET_DSG_CURR_TH (0x9310)          /* userA */
#define SET_CHG_CURR_TH (0x9312)          /* userA */
#define SET_OPEN_WIRE_CHECK_TIME (0x9314) /* s */
#define SET_INTERCONN_RES_1 (0x9315)      /* mOhm */
#define SET_INTERCONN_RES_2 (0x9317)      /* mOhm */
#define SET_INTERCONN_RES_3 (0x9319)      /* mOhm */
#define SET_INTERCONN_RES_4 (0x931B)      /* mOhm */
#define SET_INTERCONN_RES_5 (0x931D)      /* mOhm */
#define SET_INTERCONN_RES_6 (0x931F)      /* mOhm */
#define SET_INTERCONN_RES_7 (0x9321)      /* mOhm */
#define SET_INTERCONN_RES_8 (0x9323)      /* mOhm */
#define SET_INTERCONN_RES_9 (0x9325)      /* mOhm */
#define SET_INTERCONN_RES_10 (0x9327)     /* mOhm */
#define SET_INTERCONN_RES_11 (0x9329)     /* mOhm */
#define SET_INTERCONN_RES_12 (0x932B)     /* mOhm */
#define SET_INTERCONN_RES_13 (0x932D)     /* mOhm */
#define SET_INTERCONN_RES_14 (0x932F)     /* mOhm */
#define SET_INTERCONN_RES_15 (0x9331)     /* mOhm */
#define SET_INTERCONN_RES_16 (0x9333)     /* mOhm */
#define SET_MFG_STATUS_INIT (0x9343)
#define SET_CBAL_CONF (0x9335)
#define SET_CBAL_MIN_CELL_TEMP (0x9336) /* °C */
#define SET_CBAL_MAX_CELL_TEMP (0x9337) /* °C */
#define SET_CBAL_MAX_INT_TEMP (0x9338)  /* °C */
#define SET_CBAL_INTERVAL (0x9339)      /* s */
#define SET_CBAL_MAX_CELLS (0x933A)
#define SET_CBAL_CHG_MIN_CELL_V (0x933B) /* mV */
#define SET_CBAL_CHG_MIN_DELTA (0x933D)  /* mV */
#define SET_CBAL_CHG_STOP_DELTA (0x933E) /* mV */
#define SET_CBAL_RLX_MIN_CELL_V (0x933F) /* mV */
#define SET_CBAL_RLX_MIN_DELTA (0x9341)  /* mV */
#define SET_CBAL_RLX_STOP_DELTA (0x9342) /* mV */

/* Power (manual section 13.4) */

#define PWR_SHUTDOWN_CELL_V (0x923F)         /* mV */
#define PWR_SHUTDOWN_STACK_V (0x9241)        /* 10 mV */
#define PWR_SHUTDOWN_LOW_V_DELAY (0x9243)    /* s */
#define PWR_SHUTDOWN_TEMP (0x9244)           /* °C */
#define PWR_SHUTDOWN_TEMP_DELAY (0x9245)     /* s */
#define PWR_SHUTDOWN_FET_OFF_DELAY (0x9252)  /* 0.25 s */
#define PWR_SHUTDOWN_CMD_DELAY (0x9253)      /* 0.25 s */
#define PWR_SHUTDOWN_AUTO_TIME (0x9254)      /* min */
#define PWR_SHUTDOWN_RAM_FAIL_TIME (0x9255)  /* s */
#define PWR_SLEEP_CURRENT (0x9248)           /* mA */
#define PWR_SLEEP_VOLTAGE_TIME (0x924A)      /* s */
#define PWR_SLEEP_WAKE_COMP_CURRENT (0x924B) /* mA */
#define PWR_SLEEP_HYST_TIME (0x924D)         /* s */
#define PWR_SLEEP_CHG_V_TH (0x924E)          /* 10 mV */
#define PWR_SLEEP_PACK_TOS_DELTA (0x9250)    /* 10 mV */

/* System Data (manual section 13.5) */

#define SYS_CONFIG_RAM_SIGNATURE (0x91E0)

/* Protections (manual section 13.6) */

#define PROT_CUV_THRESHOLD (0x9275)  /* 50.6 mV */
#define PROT_CUV_DELAY (0x9276)      /* 3.3 ms */
#define PROT_CUV_RECOV_HYST (0x927B) /* 50.6 mV */
#define PROT_COV_THRESHOLD (0x9278)  /* 50.6 mV */
#define PROT_COV_DELAY (0x9279)      /* 3.3 ms */
#define PROT_COV_RECOV_HYST (0x927C) /* 50.6 mV */
#define PROT_COVL_LATCH_LIMIT (0x927D)
#define PROT_COVL_COUNTER_DEC_DELAY (0x927E) /* s */
#define PROT_COVL_RECOV_TIME (0x927F)        /* s */
#define PROT_OCC_THRESHOLD (0x9280)          /* 2 mV */
#define PROT_OCC_DELAY (0x9281)              /* 3.3 ms */
#define PROT_OCC_RECOVERY (0x9288)           /* mA */
#define PROT_OCC_PACK_TOS_DELTA (0x92B0)     /* 10 mV */
#define PROT_OCD1_THRESHOLD (0x9282)         /* 2 mV */
#define PROT_OCD1_DELAY (0x9283)             /* 3.3 ms */
#define PROT_OCD2_THRESHOLD (0x9284)         /* 2 mV */
#define PROT_OCD2_DELAY (0x9285)             /* 3.3 ms */
#define PROT_SCD_THRESHOLD (0x9286)          /* see SCD_threshold_setting */
#define PROT_SCD_DELAY (0x9287)              /* 15 us */
#define PROT_SCD_RECOV_TIME (0x9294)         /* s */
#define PROT_OCD3_THRESHOLD (0x928A)         /* userA */
#define PROT_OCD3_DELAY (0x928C)             /* s */
#define PROT_OCD_RECOV_THRESHOLD (0x928D)    /* mA */
#define PROT_OCDL_LATCH_LIMIT (0x928F)
#define PROT_OCDL_COUNTER_DEC_DELAY (0x9290) /* s */
#define PROT_OCDL_RECOV_TIME (0x9291)        /* s */
#define PROT_OCDL_RECOV_THRESHOLD (0x9292)   /* mA */
#define PROT_SCDL_LATCH_LIMIT (0x9295)
#define PROT_SCDL_COUNTER_DEC_DELAY (0x9296) /* s */
#define PROT_SCDL_RECOV_TIME (0x9297)        /* s */
#define PROT_SCDL_RECOV_THRESHOLD (0x9298)   /* mA */
#define PROT_OTC_THRESHOLD (0x929A)          /* °C */
#define PROT_OTC_DELAY (0x929B)              /* s */
#define PROT_OTC_RECOVERY (0x929C)           /* °C */
#define PROT_OTD_THRESHOLD (0x929D)          /* °C */
#define PROT_OTD_DELAY (0x929E)              /* s */
#define PROT_OTD_RECOVERY (0x929F)           /* °C */
#define PROT_OTF_THRESHOLD (0x92A0)          /* °C */
#define PROT_OTF_DELAY (0x92A1)              /* s */
#define PROT_OTF_RECOVERY (0x92A2)           /* °C */
#define PROT_OTINT_THRESHOLD (0x92A3)        /* °C */
#define PROT_OTINT_DELAY (0x92A4)            /* s */
#define PROT_OTINT_RECOVERY (0x92A5)         /* °C */
#define PROT_UTC_THRESHOLD (0x92A6)          /* °C */
#define PROT_UTC_DELAY (0x92A7)              /* s */
#define PROT_UTC_RECOVERY (0x92A8)           /* °C */
#define PROT_UTD_THRESHOLD (0x92A9)          /* °C */
#define PROT_UTD_DELAY (0x92AA)              /* s */
#define PROT_UTD_RECOVERY (0x92AB)           /* °C */
#define PROT_UTINT_THRESHOLD (0x92AC)        /* °C */
#define PROT_UTINT_DELAY (0x92AD)            /* °C */
#define PROT_UTINT_RECOVERY (0x92AE)         /* °C */
#define PROT_RECOVERY_TIME (0x92AF)          /* s */
#define PROT_HWD_DELAY (0x92B2)              /* s */
#define PROT_LOAD_DET_ACTIVE_TIME (0x92B4)   /* s */
#define PROT_LOAD_DET_RETRY_DELAY (0x92B5)   /* s */
#define PROT_LOAD_DET_TIMEOUT (0x92B6)       /* hrs */
#define PROT_PTO_CHG_THRESHOLD (0x92BA)      /* mA */
#define PROT_PTO_DELAY (0x92BC)              /* s */
#define PROT_PTO_RESET (0x92BE)              /* Ah */

void get_16_cell_voltage(void);
void get_all_temperature(void);

#endif