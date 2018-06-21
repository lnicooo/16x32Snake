
enum MMA8452Q_Register {
	STATUS = 0x00,
	OUT_X_MSB        = 0x01,
	OUT_X_LSB        = 0x02,
	OUT_Y_MSB        = 0x03,
	OUT_Y_LSB        = 0x04,
	OUT_Z_MSB        = 0x05,
	OUT_Z_LSB        = 0x06,
	SYSMOD           = 0x0B,
	INT_SOURCE       = 0x0C,
	WHO_AM_I         = 0x0D,
	XYZ_DATA_CFG     = 0x0E,
	HP_FILTER_CUTOFF = 0x0F,
	PL_STATUS        = 0x10,
	PL_CFG           = 0x11,
	PL_COUNT         = 0x12,
	PL_BF_ZCOMP      = 0x13,
	P_L_THS_REG      = 0x14,
	FF_MT_CFG        = 0x15,
	FF_MT_SRC        = 0x16,
	FF_MT_THS        = 0x17,
	FF_MT_COUNT      = 0x18,
	TRANSIENT_CFG    = 0x1D,
	TRANSIENT_SRC    = 0x1E,
	TRANSIENT_THS    = 0x1F,
	TRANSIENT_COUNT  = 0x20,
	PULSE_CFG        = 0x21,
	PULSE_SRC        = 0x22,
	PULSE_THSX       = 0x23,
	PULSE_THSY       = 0x24,
	PULSE_THSZ       = 0x25,
	PULSE_TMLT       = 0x26,
	PULSE_LTCY       = 0x27,
	PULSE_WIND       = 0x28,
	ASLP_COUNT       = 0x29,
	CTRL_REG1        = 0x2A,
	CTRL_REG2        = 0x2B,
	CTRL_REG3        = 0x2C,
	CTRL_REG4        = 0x2D,
	CTRL_REG5        = 0x2E,
	OFF_X            = 0x2F,
	OFF_Y            = 0x30,
	OFF_Z            = 0x31,
    SLA_W            = 0x38,
    SLA_R            = 0x39
};

enum pl_pos{
	// Possible portrait/landscape settings
	PORTRAIT_U,
	PORTRAIT_D,
	LANDSCAPE_R,
	LANDSCAPE_L,
	LOCKOUT = 0x40

};

void mma_init();
uint8_t mma_get_PL();
void mma_standby();
void mma_activate();
void mma_outrate();
uint8_t mma_available();
void mma_scale();
void mma_setpl();
void mma_settap();
uint8_t mma_get_tap();
void mma_read_register(uint8_t reg);
void mma_write_register(uint8_t reg, uint8_t data);