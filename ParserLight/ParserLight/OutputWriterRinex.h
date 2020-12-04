#pragma once
#include "OutputWriter.h"
#include <vector>

class OutputWriterRinex : public OutputWriter
{
private:
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

	int first_message;

	int mjd_first_epoch;
	double sec_first_epoch;
	int leap_sec;

	int mjd_valid;

	int receiver_number;
	std::string receiver_type;
	int antenna_number;
	std::string comment;

	void write_header();
	OutputWriterRinex::GNSS_EPOCH store_RAWX(char * inbuffer);
	void write_message(OutputWriterRinex::GNSS_EPOCH my_gnss_epoch);

public:
	OutputWriterRinex(int mjd_valid);
	OutputWriterRinex(int mjd_valid, int receiver_number, std::string receiver_type, int antenna_number, std::string comment);
	~OutputWriterRinex();

	void open_file(std::string file_name);
	void set_meta_data(int receiver_number, std::string receiver_type, int antenna_number, std::string comment);
	void set_first_epoch(int mjd, double sec, int leap_sec);
	void parse_and_write(char * inbuffer);

	UbloxSpecs::UBX_HEAD get_reference_header();
};

