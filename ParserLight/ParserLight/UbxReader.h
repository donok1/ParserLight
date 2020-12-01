#pragma once
#include <iostream>		// for console output
#include <fstream>		// reading and writing files
#include <iomanip>		// ability to set the length of outputs when writing files
#include <cmath>		// math tools
#include <algorithm>	// e.g. max()
#include <string>
#include <vector>
#include "myConstants.h"
#include <bitset>
#include <cinttypes>

class UbxReader
{
public:
	UbxReader();
	UbxReader(double rate);
	~UbxReader();

	std::streampos file_size;
	std::ifstream file;
	double rate; // set a measurement rate in order to speed up the processing [seconds]
	double threshold = 0.02;

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

	/* CONTAINERS FOR FLAGS */
	/* FixStatusFlag*/	
	typedef struct
	{
		unsigned int fix_flag : 1;	/* 1 bit GPS Fix Flag */
		unsigned int dgps_flag : 1;	/* 1 bit DGPS Flag */
		unsigned int week_flag : 1;	/* 1 bit valid gps week number */
		unsigned int tow_flag : 1;	/* 1 bit valid gps time of week flag */
		unsigned int unused : 4;	/* pad up to 8 bits*/
	} FIX_FLAG;

	/* recStat Flag */
	typedef struct
	{
		unsigned int leap_sec : 1;	/* Leap second determined */
		unsigned int clock_res : 1;	/* Clock reset applied */
		unsigned int unused : 6;	/* pad up to 8 bits*/
	} REC_FLAG;

	/* validity flag */
	typedef struct
	{
		unsigned int valid : 1;	/* Leap second determined */
		unsigned int unused : 7; /* pad up to 8 bits*/
	} VAL_FLAG;


	/* std_flag */
	typedef struct
	{
		unsigned int std_flag : 4;
		unsigned int unused : 4; // pad up to 8 bits
	} STD_FLAG;

	/* trkStat flag */
	typedef struct
	{
		unsigned int pr_valid : 1;		/* Pseudorange valid */
		unsigned int cp_valid : 1;		/* Carrier phase valid */
		unsigned int half_cyc : 1;		/* Half cycle valid */
		unsigned int sub_half_cyc : 1;	/* Substracted half cycle from phase */
		unsigned int unused : 4;		/* pad up to 8 bits*/
	} TRK_FLAG;

	/* SV flag for NAV-SVINFO */
	typedef struct
	{
		unsigned int svUsed : 1;		/* SV is used for navigation */
		unsigned int diffCorr : 1;		/* Differential correction data is available for this SV */
		unsigned int orbitAvail : 1;	/* Orbit information is available for this SV (Ephemeris or Almanac) */
		unsigned int orbitEph : 1;		/* Orbit information is Ephemeris */
		unsigned int unhealthy : 1;		/* SV is unhealthy / shall not be used */
		unsigned int orbitAlm : 1;		/* Orbit information is Almanac Plus */
		unsigned int orbitAop : 1;		/* Orbit information is AssistNow Autonomous */
		unsigned int smoothed : 1;		/* Carrier smoothed pseudorange used */
	} SV_FLAG;

	/* Bitfield Flag for NAV-SVINFO */
	typedef struct
	{
		unsigned int antaris : 1;		/* Antatis, Antaris 4 */
		unsigned int ublox5 : 1;
		unsigned int ublox6 : 1;
		unsigned int ublox7 : 1;
		unsigned int ubloxM8 : 1;
		unsigned int unused : 3;		/* pad up to 8 bits*/
	} GEN_FLAG;

	/* Quality flag for NAV-SVINFO */
	typedef struct
	{
		unsigned int noSignal : 1;			/* no signal */
		unsigned int searchSignal : 1;		/* searching signal */
		unsigned int signalAquired : 1;		/* signal aquired */
		unsigned int signalUnusable : 1;	/* signal detected but unusable */
		unsigned int codeLocked : 1;		/* code locked and time synchronized */
		unsigned int carrierLocked1 : 1;	/* code and carrier locked and time synchronized */
		unsigned int carrierLocked2 : 1;	/* code and carrier locked and time synchronized */
		unsigned int carrierLocked3 : 1;	/* code and carrier locked and time synchronized */
	} QUAL_FLAG;

	/* Container for RXM_SFRBX (0x02 0x13) messages */
	typedef struct
	{
		ub_u1 gnssId;	/* GNSS identifier */
		ub_u1 svId;		/* satellite identifier */
		ub_u1 res;		/* reserved */
		ub_u1 freqId;	/* only for GLONASS, slot +7, 0..13 */
		ub_u1 numWords;	/* number of data words in the message, from 0 to 16 */
		ub_u1 res2;		/* reserved2 */
		ub_u1 version;	/* Message version, 1 = this message */
		ub_u1 res3;		/* reserved3 */
		ub_u4 dwrd[16];	/* data words */
	} UBX_RXM_SFRBX;


	/* Container for NAV_SOL (0x01 0x06) messages */
	typedef struct
	{
		ub_u4 itow;         /* GPS milliseconds time of week [ms] */
		ub_i4 frac;         /* nanoseconds reminder of rounded ms above, range -500000 .. +500000 [ns] */
		ub_i2 week;         /* GPS week */
		ub_u1 gpsfix;       /* GPS fix type, range 0..3 (see below) */
		FIX_FLAG flags;     /* flags, see below */
		ub_i4 ecef_x;       /* ECEF x coordinate [cm] */
		ub_i4 ecef_y;       /* ECEF y coordinate [cm] */
		ub_i4 ecef_z;       /* ECEF z coordinate [cm] */
		ub_u4 p_acc;        /* 3d position accuracy estimate [cm] */
		ub_i4 ecef_vx;      /* ECEF x velocity [cm/s] */
		ub_i4 ecef_vy;      /* ECEF y velocity [cm/s] */
		ub_i4 ecef_vz;      /* ECEF z velocity [cm/s] */
		ub_u4 s_acc;        /* speed accuracy estimate [cm/s] */
		ub_u2 pdop;         /* position DOP [1e-2] */
		ub_u1 reserved1;	/* reserved */
		ub_u1 numsv;        /* number of SVs used in nav solution */
		ub_u4 reserved2;    /* reserved */
	} UBX_NAV_SOL;

	/* Container for a satellite of NAV_SVINFO (0x01 0x30) messages */
	typedef struct
	{
		ub_u1 chn;			/* Channel number, 255 for SVs not assigned to a channel */
		ub_u1 svid;			/* Satellite ID */
		SV_FLAG flags;		/* Signal flag */
		QUAL_FLAG quality;	/* Quality flag */
		ub_u1 cno;			/* Carrier to noise ratio [dB-Hz] */
		ub_i1 elev;			/* Elevation angle in integer degreee [deg] */
		ub_i2 azim;			/* Azimuth angle in integer degreee [deg] */
		ub_i4 prRes;		/* Pseudo range residual in centimeters [cm] */
	} UBX_NAV_SVINFO_SV;

	/* Container for a NAV_SVINFO (0x01 0x30) messages */
	typedef struct
	{
		ub_u4							iTOW;			/* GPS time of week of the navigation epoch [ms] */
		ub_u1							numCh;			/* Number of channels */
		GEN_FLAG						globalFlags;	/* Receiver generation flag */
		ub_u1							reserved1[2];	/* Reserved */
	} UBX_NAV_SVINFO;

	/* Container for a satellite of RXM_RAW (0x02 0x10) messages */
	typedef struct 
	{
		ub_r8 cp_mes;       /* Carrier phase measurement [L1 cycles]*/
		ub_r8 pr_mes;       /* Pseudorange measurement [m]*/
		ub_r4 do_mes;       /* Doppler measurement [Hz] */
		ub_u1 sv_id;        /* Space Vehicle Number */
		ub_i1 mes_qi;       /* Nav Measurements Quality Indicator */
		ub_i1 cno;          /* Signal strength C/No. (dbHz) */
		ub_u1 lli;          /* Loss of lock indicator (RINEX definition) */
	} UBX_RXM_RAW_SV;

	/* Container for a satellite of RCL_RAW (0x11 0x10) messages */
	typedef struct
	{
		ub_r8 cp_mes;       /* Carrier phase measurement [L1 cycles]*/
		ub_r8 pr_mes;       /* Pseudorange measurement [m]*/
		ub_r4 do_mes;       /* Doppler measurement [Hz] */
		ub_u1 sv_id;         /* Space Vehicle Number */
		ub_i1 mes_qi;       /* Nav Measurements Quality Indicator */
		ub_i1 cno;          /* Signal strength C/No. (dbHz) */
		ub_u1 lli;          /* Loss of lock indicator (RINEX definition) */
		ub_u1 gnss_id;		/* GNSS identifier*/
		ub_u1 res1;			/* reserved1 */
		ub_u2 res2;			/* reserved2 */
	} UBX_RCL_RAW_SV;

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
		ub_u1 reserved3;	/* reserved3 */
	} UBX_RXM_RAWX_SV;

	/* Container for a RXM_RAW (0x02 0x10) messages */
	typedef struct
	{
		ub_i4 itow;						/* GPS millisecond time of week [ms] */
		ub_i2 week;						/* Measurement GPS week number (Receiver Time)*/
		ub_u1 num_sv;					/* # of satellites following. */
		ub_u1 reserved1;				/* reserved */
		std::vector<UBX_RXM_RAW_SV>	meas;	/* Measurements */
	} UBX_RXM_RAW;

	/* Container for RCL_RAW (0x11 0x10) messages */
	typedef struct
	{
		ub_i4 rcv_tow;					/* Measurement time of week in receiver local time [ms] */
		ub_i2 week;						/* Measurement GPS week number (Receiver Time)*/
		ub_u1 num_sv;					/* # of satellites following. */
		ub_u1 reserved1;				/* reserved1 */
		ub_r8 tow;						/* Full resolution time of week in receiver local time [s] */
		ub_u4 reserved2;				/* reserved2 */
		ub_u4 reserved3;				/* reserved3 */
		std::vector<UBX_RCL_RAW_SV> meas;	/* Measurements */
	} UBX_RCL_RAW;

	/* Container for a RXM_RAWX (0x02 0x15) messages */
	typedef struct
	{
		ub_r8 rcv_tow;					/* Full resolution time of week in receiver local time [s] */
		ub_u2 week;						/* Measurement GPS week number (Receiver Time)*/
		ub_i1 leap_s;					/* GPS leap seconds (GPS-UTC)*/
		ub_u1 num_meas;					/* Number of measurements to follow */
		REC_FLAG rec_stat;				/* receiver tracking status */
		ub_u1 reserved1[3];				/* reserved1 */
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
	static UbxReader::UBX_CHECKSUM ubx_gen_checksum(char *payload, UbxReader::UBX_HEAD ub_head)
	{
		int i;
		UbxReader::UBX_CHECKSUM temp;
		temp.a = ub_head.cl + ub_head.id + (ub_head.len & 0xff) + (ub_head.len >> 8);
		temp.b = 4 * ub_head.cl + 3 * ub_head.id + 2 * (ub_head.len & 0xff) + (ub_head.len >> 8);
		for (i = 0; i < ub_head.len; i++) {
			temp.a += payload[i];
			temp.b += temp.a;
		}
		return(temp);
	}

	/* STATIC Function to checksum a UBX message */
	static bool ub_checksum(char *payload, UbxReader::UBX_HEAD ub_head, UbxReader::UBX_CHECKSUM ub_chksum)
	{
		UbxReader::UBX_CHECKSUM temp;

		if (ub_head.len == 0){return false;}

		temp = ubx_gen_checksum(payload, ub_head);
		return(temp.a == ub_chksum.a && temp.b == ub_chksum.b);
	}

	/* STATIC Function to get MJD and seconds within day from miliseconds in week (tow) and week */
	void get_mjd(double tow_ms, int week, int *mjd, double *sec)
	{
		int dow; // day of week
		double tow_sec; // time of week in seconds
		dow = (int) trunc(tow_ms / (1000.0*86400.0));
		*mjd = 44244 + 7 * week + dow;
		tow_sec = (double)tow_ms / 1000.0;
		*sec = tow_sec - 86400 * dow;
		return;
	}

	/* Common bin for GNSS measurements*/
	typedef struct
	{
		double		C1;			/* Code measurement L1 [m] */
		double		L1;			/* Phase measurement L1 [cycles] */
		double		D1;			/* Doppler measurement L1 [Hz] */
		int			sv_id;		/* Space vehicle identification number */
		int			gnss_id;	/* Satellite system identification number (0=GPS,1=SBAS,2=Galileo,3=BeiDou,4=IMES,5=GZSS,6=GLONASS)*/
		int			freq_id;	/* Frequence ID for GLONASS (slot + 7) */
		long int	locktime;	/* Carrier phase locktime counter (max. 64500 ms) */
		int			mes_qi;		/* Quality indicator (>=4 : PR+DO OK, >=5 : PR+DO+CP OK, <6 : likely loss of carrier lock)*/
		int			cno;		/* Carrier to noise [dB-Hz] */
		int			lol;		/* Loss of lock indicator (RINEX convention) */
		double		C1_stdev;	/* Standard deviation of C1 [m] */
		double		L1_stdev;	/* Standard deviation of L1 [cycles] */
		double		D1_stdev;	/* Standard deviation of D1 [Hz] */
		int			C1_valid;
		int			L1_valid;
		int			half_cycle_valid;
		int			hald_cycle_substracted;
	} GNSS_MEAS;

	/* Common bin for gnss epoch*/
	struct GNSS_EPOCH
	{
		long int				mjd;				/* modified julian date of day */
		double					sec;				/* seconds within day, with fractional part */
		int						num_meas;			/* number of measurements within epoch */
		int						leap_sec;			/* leap seconds */
		int						leap_sec_valid;		/* leap second flag */
		int						clock_reset;		/* clock reset flag */
		std::vector<GNSS_MEAS>	meas;				/* actual measurements */
		bool operator < (const GNSS_EPOCH &a) const
		{
			return 86400 * mjd + sec < 86400 * a.mjd + a.sec;
		}
	};
	std::vector<GNSS_EPOCH> my_gnss_epoch;
	
	/* Bin for HPPOSLLH*/
	struct POSLLH
	{
		double tow;
		double lon;
		double lat;
		double height;
		double hmsl;
		double hz_acc;
		double v_acc;
	};

	/* Common bin for additional information*/
	/* Container for NAV_SOL (0x01 0x06) messages */
	struct NAV_EPOCH
	{
		long int	mjd;			/* modified julian date [days] */
		double		sec;			/* seconds within day [sec] */
		int			gpsfix;			/* GPS fix type, range 0..3 (see below) */
		int			fix_ok;			
		int			dgps_used;
		int			valid_week;
		int			valid_tow;
		double		ecef_x;			/* ECEF x coordinate [m] */
		double		ecef_y;			/* ECEF y coordinate [m] */
		double		ecef_z;			/* ECEF z coordinate [m] */
		double		p_acc;			/* 3d position accuracy estimate [m] */
		double		ecef_vx;		/* ECEF x velocity [m/s] */
		double		ecef_vy;		/* ECEF y velocity [m/s] */
		double		ecef_vz;		/* ECEF z velocity [m/s] */
		double		s_acc;			/* speed accuracy estimate [m/s] */
		double		pdop;			/* position DOP [1e-2] */
		int			numsv;			/* number of SVs used in nav solution */
		bool operator < (const NAV_EPOCH &a) const
		{
			return 86400*mjd+sec<86400*a.mjd+a.sec;
		}
	} ;
	std::vector<NAV_EPOCH> my_nav_epoch;

	/* Bin for SV_INFO SV */
	typedef struct
	{
		int channel;		/* channel number */
		int sv_id;			/* SV id */
		int svUsed;			/* SV is used for navigation */
		int diffCorr1;		/* Differential correction data is available for this SV */
		int orbitAvail;		/* Orbit information is available for this SV (Ephemeris or Almanac) */
		int orbitEph;		/* Orbit information is Ephemeris */
		int unhealthy;		/* SV is unhealthy / shall not be used */
		int orbitAlm;		/* Orbit information is Almanac Plus */
		int orbitAop;		/* Orbit information is AssistNow Autonomous */
		int smoothed;		/* Carrier smoothed pseudorange used */
		int noSignal;		/* no signal */
		int searchSignal;	/* searching signal */
		int signalAquired;	/* signal aquired */
		int signalUnusable;	/* signal detected but unusable */
		int codeLocked;		/* code locked and time synchronized */
		int carrierLocked;	/* code and carrier locked and time synchronized */
		int cno;			/* CN0 [dB-Hz] */
		int elevation;		/* [deg] */
		int azimuth;		/* [deg] */
		double res;			/* pseudo range residuals [m] */
	} SVINFO_BIN;

	/* Bin for SV_INFO*/
	typedef struct
	{
		double		sow; /* seconds within week */
		int			num_ch;
		int			generation;
		std::vector<SVINFO_BIN> sv;
	} SVINFO_EPOCH;
	std::vector<SVINFO_EPOCH> my_svinfo_epoch;

	/* Bin for Subframe Data */
	typedef struct
	{
		int	gnss_id;	/* Satellite system identification number (0=GPS,1=SBAS,2=Galileo,3=BeiDou,4=IMES,5=GZSS,6=GLONASS)*/
		int	sv_id;		/* Space vehicle identification number */
		int	freq_id;	/* Frequence ID for GLONASS (slot + 7) */
		int	numWord;	/* Number of words */
		ub_u4 dwrd[16];	/* Words */
	} SUBFRAME_BIN;
	std::vector<SUBFRAME_BIN> my_subframes;

	/* Bin for Ephemeris Data */
	struct EPHEMERIS_BIN
	{
		int gnss_id;
		int sv_id;
		int freq_id;
		//
		int week;
		int IODC;
		int IODE;
		int toc; // [s]
		int toe; // [s]
		//
		int code_L2;
		int L2_flag;
		//
		int SV_acc;
		int SV_health;
		//
		double tgd; // [s]
		double af2; // [s s-2]
		double af1; // [s s-1]
		double af0; // [s]
		//
		double sqrtA; // [m1/2]
		double e; // [-]
		double i0; // [semi-circles]
		double M0; // [semi-circles]
		double OMEGA0; // [semi-circles]
		double omega; // [semi-circles]
		//
		double dN; // [semi-circles s-1]
		double OMEGADOT; // [semi-circles s-1]
		double IDOT;  // [semi-circles s-1]
		//
		double Cus; // [rad]
		double Cuc; // [rad]
		double Crs; // [m]
		double Crc; // [m]
		double Cis; // [rad]
		double Cic; // [rad]
		// < operator to be able to sort ...
		bool operator < (const EPHEMERIS_BIN &a) const
		{
			return myConstants::MAX_PRN*(604800 * week + toe) + sv_id < myConstants::MAX_PRN*(604800 * a.week + a.toe) + a.sv_id;
		}
		// equality opertator to compare
		bool operator == (const EPHEMERIS_BIN &a) const
		{
			return (gnss_id == a.gnss_id&&sv_id == a.sv_id&&freq_id == a.freq_id&&week == a.week&&IODC == a.IODC&&IODE == a.IODE&&toc == a.toc
				&&toe == a.toe&&code_L2 == a.code_L2&&L2_flag == a.L2_flag&&SV_acc == a.SV_acc&&SV_health == a.SV_health&&tgd == a.tgd
				&&af2 == a.af2&&af1 == a.af1&&af0 == a.af0&&sqrtA == a.sqrtA&&e == a.e&&i0 == a.i0&&M0 == a.M0&&OMEGA0 == a.OMEGA0
				&&omega == a.omega&&dN == a.dN&&OMEGADOT == a.OMEGADOT&&IDOT == a.IDOT&&Cus == a.Cus&&Cuc == a.Cuc&&Crs == a.Crs
				&&Crc == a.Crc&&Cis == a.Cis&&Cic == a.Cic);
		}

	};
	std::vector<EPHEMERIS_BIN> my_ephemeris;

	struct gps_nav_meta
	{
		int len;
		int len2;
		int pos;
		int pos2;
		bool sgn;
		int scale;
		int save_index;
		std::string name;
	};
	std::vector< std::vector<gps_nav_meta> > my_specs;

	/* Non static functions */
	bool UbxReader::open_file(std::string file_name);
	void UbxReader::close_file();
	void UbxReader::extract_message(UbxReader::UBX_HEAD ub_head, char *message, bool *found);	
	int UbxReader::count_messages(UbxReader::UBX_HEAD ub_head);
	// function that retrieves a specific message
	double UbxReader::retrieve_GPS_NAV(std::bitset<300> input_bitset, int a, int b);
	// functions that parse the GPS NAV bitstring
	signed long long int UbxReader::parse_GPS_NAV(std::bitset<300>, std::vector<int> i1, std::vector<int> i2, bool sgn);
	signed long long int UbxReader::parse_GPS_NAV(std::bitset<300> input_bitset, int i1, int len, int i2, int len2, bool signed_var);
	// void function that loads information as the begin and the length of each message, the scale factor and so on into a global var my_specs
	void UbxReader::load_nav_specs();
	// RAWX
	UbxReader::GNSS_EPOCH UbxReader::store_RAWX(char *payload);
	void UbxReader::read_RAWX(int mjd_check, std::string out_file);
	UbxReader::POSLLH store_HPPOSLLH(char * inbuffer);
	void read_HPPOSLLH(std::string out_file);
	// NAV_SOL
	void UbxReader::store_NAV_SOL(char *payload);
	void UbxReader::read_NAV_SOL();
	// RXM_SFRBX
	void UbxReader::store_RXM_SFRBX(char *payload);
	void UbxReader::read_RXM_SFRBX();
	void UbxReader::parse_subframes();
	// SVINFO
	void UbxReader::store_NAV_SVINFO(char *payload);
	void UbxReader::read_NAV_SVINFO();
	// get version
	double UbxReader::get_version();
};

