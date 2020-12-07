#pragma once
#include <cinttypes>
#include <vector>
/* Update Revision
 * 2020.12.07   donok1      Update RAWX to UBX-18010854 - R08, 28-May-2020, complient with HPG 1.13
 */


class UbloxSpecs
{
public:
	UbloxSpecs();
	~UbloxSpecs();

	static void test_specs();

	/* The uBlox data types */
	typedef uint8_t		ub_u1;
	typedef uint16_t	ub_u2;
	typedef uint32_t	ub_u4;
	typedef int8_t		ub_i1;
	typedef int16_t		ub_i2;
	typedef int32_t		ub_i4;
	typedef float		ub_r4;
	typedef double		ub_r8;

	// for debug purpose
	typedef struct
	{
		unsigned int w1 : 1;
		unsigned int w2 : 1;
		unsigned int w3 : 1;
		unsigned int w4 : 1;
		unsigned int w5 : 1;
		unsigned int w6 : 1;
		unsigned int w7 : 1;
		unsigned int w8 : 1;
	} MY_BYTE;

	/* validity flag */
	typedef struct
	{
		unsigned int valid : 1;	/* Leap second determined */
		unsigned int unused : 7; /* pad up to 8 bits*/
	} VAL_FLAG;

	/* trkStat flag */
	typedef struct
	{
		unsigned int pr_valid : 1;		/* Pseudorange valid */
		unsigned int cp_valid : 1;		/* Carrier phase valid */
		unsigned int half_cyc : 1;		/* Half cycle valid */
		unsigned int sub_half_cyc : 1;	/* Substracted half cycle from phase */
		unsigned int unused : 4;		/* pad up to 8 bits*/
	} TRK_FLAG;

	/* recStat Flag */
	typedef struct
	{
		unsigned int leap_sec : 1;	/* Leap second determined */
		unsigned int clock_res : 1;	/* Clock reset applied */
		unsigned int unused : 6;	/* pad up to 8 bits*/
	} REC_FLAG;

	/* std_flag */
	typedef struct
	{
		unsigned int std_flag : 4;
		unsigned int unused : 4; // pad up to 8 bits
	} STD_FLAG;

	/* Container for a measurement of RXM_RAWX (0x02 0x15) messages */
	typedef struct
	{
		ub_r8 pr_mes;       /* Pseudorange measurement [m]*/
		ub_r8 cp_mes;       /* Carrier phase measurement [L1 cycles]*/
		ub_r4 do_mes;       /* Doppler measurement [Hz] */
		ub_u1 gnss_id;		/* GNSS identifier*/
		ub_u1 sv_id;        /* Space Vehicle Number */
		ub_u1 reserved2;	/* reserved2 */
		ub_u1 freq_id;		/* GLONASS frequence id: slot + 7 (0 to 13) */
		ub_u2 locktime;		/* Carrier phase locktime counter (max. 64500 ms) */
		ub_u1 cno;          /* Signal strength C/No. (dbHz) */
		STD_FLAG pr_stdev;	/* Estimated pseudorange measurement st. dev.*/
		STD_FLAG cp_stdev;	/* Estimated phase measurement st. dev.*/
		STD_FLAG do_stdev;	/* Estimated doppler measurement st. dev.*/
		TRK_FLAG trk_stat;	/* Tracking status bitfield */
		ub_u1 reserved1;	/* reserved1 */
	} UBX_RXM_RAWX_SV;

	/* Container for a RXM_RAWX (0x02 0x15) messages */
	typedef struct
	{
		ub_r8 rcv_tow;					/* Full resolution time of week in receiver local time [s] */
		ub_u2 week;						/* Measurement GPS week number (Receiver Time)*/
		ub_i1 leap_s;					/* GPS leap seconds (GPS-UTC)*/
		ub_u1 num_meas;					/* Number of measurements to follow */
		REC_FLAG rec_stat;				/* receiver tracking status */
        ub_u1 version;                  /* Message version (0x01 for this version) */
		ub_u1 reserved0[2];				/* reserved0 */
		std::vector<UBX_RXM_RAWX_SV> meas;	/* Measurements */
	} UBX_RXM_RAWX;

	/* Container for a NAV_HPPOSLLH (0x01 0x14) messages */
	typedef struct
	{
		ub_u1 version;
		ub_u1 reserved1[2];
		VAL_FLAG validity;
		ub_u4 iTOW;
		ub_i4 lon;
		ub_i4 lat;
		ub_i4 height;
		ub_i4 hMSL;
		ub_i1 lonHp;
		ub_i1 latHp;
		ub_i1 heightHp;
		ub_i1 hMSLHp;
		ub_u4 hAcc;
		ub_u4 hVacc;
	} UBX_NAV_HPPOSLLH;

	/* Header of a UBX message */
	typedef struct
	{
		ub_u1 sig[2];	/* The UBX signature (see above) */
		ub_u1 cl;		/* The class number */
		ub_u1 id;       /* The message number (id) */
		ub_u2 len;      /* The payload length */
	} UBX_HEAD;

	/* The checksum of a UBX message */
	typedef struct
	{
		ub_u1 a;          /* CK_A */
		ub_u1 b;          /* CK_B */
	} UBX_CHECKSUM;

	/* STATIC Function to calculate the checksum of a UBX message */
	static UbloxSpecs::UBX_CHECKSUM ubx_gen_checksum(char *payload, UbloxSpecs::UBX_HEAD ub_head)
	{
		int i;
		UbloxSpecs::UBX_CHECKSUM temp;
		temp.a = ub_head.cl + ub_head.id + (ub_head.len & 0xff) + (ub_head.len >> 8);
		temp.b = 4 * ub_head.cl + 3 * ub_head.id + 2 * (ub_head.len & 0xff) + (ub_head.len >> 8);
		for (i = 0; i < ub_head.len; i++) {
			temp.a += payload[i];
			temp.b += temp.a;
		}
		return(temp);
	}

	/* STATIC Function to checksum a UBX message */
	static bool ub_checksum(char *payload, UbloxSpecs::UBX_HEAD ub_head, UbloxSpecs::UBX_CHECKSUM ub_chksum)
	{
		UbloxSpecs::UBX_CHECKSUM temp;

		if (ub_head.len == 0) { return false; }

		temp = ubx_gen_checksum(payload, ub_head);
		return(temp.a == ub_chksum.a && temp.b == ub_chksum.b);
	}
};

