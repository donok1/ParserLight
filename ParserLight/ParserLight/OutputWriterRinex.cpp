#include "stdafx.h"
#include "OutputWriterRinex.h"
#include "UbloxSpecs.h"
#include "Utils.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>

OutputWriterRinex::OutputWriterRinex(int mjd_valid)
{
	OutputWriterRinex::mjd_valid = mjd_valid;

	OutputWriterRinex::first_message = true;

	OutputWriterRinex::mjd_first_epoch = 0;
	OutputWriterRinex::sec_first_epoch = 0;
	OutputWriterRinex::leap_sec = 0;
	
	OutputWriterRinex::receiver_number = 999999;
	OutputWriterRinex::receiver_type = "generic";
	OutputWriterRinex::antenna_number = 999999;
	OutputWriterRinex::comment = "empty comment";
}

OutputWriterRinex::OutputWriterRinex(int mjd_valid, int receiver_number, std::string receiver_type, int antenna_number, std::string comment)
{
	OutputWriterRinex::mjd_valid = mjd_valid;

	OutputWriterRinex::first_message = true;
	set_meta_data(receiver_number, receiver_type, antenna_number, comment)
}

OutputWriterRinex::~OutputWriterRinex()
{
}

void OutputWriterRinex::open_file(std::string file_name)
{
	short int month;
	short int year;
	double hour;
	short int day;
	short int doy;
	char doy03[10];
	char year02[10];
	std::ostringstream string_stream;

	/* Create a name following the rinex naming convention */
	Utils::YMDHfromMJD(OutputWriterRinex::mjd_first_epoch, &year, &month, &day, &hour);
	Utils::YearDoyFromMJD(OutputWriterRinex::mjd_first_epoch, &year, &doy);
	// year with 2 digits
	sprintf(year02, "%02.0f", year - 100 * trunc(year / 100));
	// add leading zero to DOY
	sprintf(doy03, "%03u", doy);
	string_stream << doy03 << "0." << year02 << "O";
	// transform to upper case
	std::transform(file_name.begin(), file_name.end(), file_name.begin(), ::toupper);
	// write name: xxxDOY0.YYO
	file_name = file_name + string_stream.str();
	file.open(file_name);
}

void OutputWriterRinex::set_meta_data(int receiver_number, std::string receiver_type, int antenna_number, std::string comment)
{
	if (receiver_type.length() > 20)
	{
		std::cout << "Error: receiver type too long (max. 20)\n";
	}
	if (antenna_number > 9 || antenna_number < 0)
	{
		std::cout << "Error: antenna number between 0 and 9 expected\n";
	}

	OutputWriterRinex::receiver_number = receiver_number;
	OutputWriterRinex::receiver_type = receiver_type;
	OutputWriterRinex::antenna_number = antenna_number;
	OutputWriterRinex::comment = comment;
}

void OutputWriterRinex::set_first_epoch(int mjd, double sec, int leap_sec)
{
	OutputWriterRinex::mjd_first_epoch = mjd;
	OutputWriterRinex::sec_first_epoch = sec;
	OutputWriterRinex::leap_sec = leap_sec;
}

/* Write a Rinex Observation file, format 2.11 */
void OutputWriterRinex::write_header() {
	short int month;
	short int year;
	short int minutes;
	double hour;
	double sec;
	short int day;
	short int doy;
	char buffer[2000]; // buffer aimed to write the file
	double rinex_version = 2.11;
	std::ostringstream string_stream;

	/* Create a name following the rinex naming convention */
	Utils::YMDHfromMJD(OutputWriterRinex::mjd_first_epoch, &year, &month, &day, &hour);
	Utils::YearDoyFromMJD(OutputWriterRinex::mjd_first_epoch, &year, &doy);

	// get local time
	time_t now = time(0);
	tm tm = *localtime(&now);

	// get the time of the first epoch
	sec = OutputWriterRinex::sec_first_epoch; // read the mjd of the first epoch
	hour = (int)(sec / 3600);
	minutes = (int)((sec - hour * 3600) / 60);
	sec = sec - hour * 3600 - minutes * 60;

	std::string sat_name = "L30";

	/* write header lines  */
	sprintf(buffer, "     %4.2f           OBSERVATION DATA    M                   RINEX VERSION / TYPE\n", rinex_version);
	file << buffer;
	file << "UBX PARSER                              " << std::put_time(&tm, "%d-%m-%y %H:%M") << "      PGM / RUN BY / DATE\n";
	sprintf(buffer, "                                        ETH Zurich          OBSERVER / AGENCY\n");
	file << buffer;
	sprintf(buffer, "%3s                                                         MARKER NAME\n", sat_name.c_str());
	file << buffer;
	sprintf(buffer, "%3s                                                         MARKER NUMBER\n", sat_name.c_str());
	file << buffer;
	sprintf(buffer, "%6u              %-20s                    REC # / TYPE / VERS\n", receiver_number, receiver_type.c_str());
	file << buffer;
	sprintf(buffer, "                    CUBEANT%1u                                ANT # / TYPE\n", antenna_number);
	file << buffer;
	sprintf(buffer, "        0.0000        0.0000        0.0000                  APPROX POSITION XYZ\n");
	file << buffer;
	sprintf(buffer, "        0.0000        0.0000        0.0000                  ANTENNA: DELTA H/E/N\n");
	file << buffer;
	sprintf(buffer, "     1     1                                                WAVELENGTH FACT L1/2\n");
	file << buffer;
	sprintf(buffer, "     4    C1    L1    D1    S1                              # / TYPES OF OBSERV\n");
	file << buffer;
	sprintf(buffer, "  %4u    %2u    %2u    %2u    %2u   %10.7f     GPS         TIME OF FIRST OBS\n", (int)year, (int)month, (int)day, (int)hour, (int)minutes, sec);
	file << buffer;
	//sprintf(buffer, "  %4u    %2u    %2u    %2u    %2u   %10.7f     GPS         TIME OF LAST OBS\n", (int)year_end, (int)month_end, (int)day_end, (int)hour_end, (int)minutes_end, sec_end);
	//file << buffer;
	if (OutputWriterRinex::leap_sec > 0)
	{
		sprintf(buffer, "    %2u                                                      LEAP SECONDS\n", OutputWriterRinex::leap_sec);
		file << buffer;
	}
	//sprintf(buffer, "    %2u                                                      # OF SATELLITES\n", num_sats);
	//file << buffer;
	sprintf(buffer, "                                                            END OF HEADER\n");
	file << buffer;
}

OutputWriterRinex::GNSS_EPOCH OutputWriterRinex::store_RAWX(char *inbuffer)
{
	int my_mjd;
	double my_sec;
	UbloxSpecs::UBX_HEAD ub_head;
	ub_head.cl = 0x02; // RAWX class
	ub_head.id = 0x15; // RAWX id

	UbloxSpecs::UBX_RXM_RAWX my_rawx;
	GNSS_EPOCH temp;

	unsigned char head[4] = { 0xb5, 0x62, ub_head.cl, ub_head.id }; // header

	char inhead[4] = { inbuffer[0], inbuffer[1], inbuffer[2], inbuffer[3] };

	if (memcmp(&head, &inhead, 4) == 0)
	{
		ub_head.cl = inbuffer[2];
		ub_head.id = inbuffer[3];
		memcpy(&ub_head.len, &inbuffer[4], 2);	//length

		// READ
		memcpy(&my_rawx.rcv_tow, &inbuffer[6], 8); /* [s] */
		memcpy(&my_rawx.week, &inbuffer[14], 2);
		memcpy(&my_rawx.leap_s, &inbuffer[16], 1);
		memcpy(&my_rawx.num_meas, &inbuffer[17], 1);
		memcpy(&my_rawx.rec_stat, &inbuffer[18], 1);

		// MAP to general bin
		Utils::get_mjd(1000.0*my_rawx.rcv_tow, my_rawx.week, &my_mjd, &my_sec); // time conversion
		temp.mjd = my_mjd;
		temp.sec = my_sec;
		temp.num_meas = my_rawx.num_meas;
		temp.leap_sec = my_rawx.leap_s;
		temp.leap_sec_valid = my_rawx.rec_stat.leap_sec;
		temp.clock_reset = my_rawx.rec_stat.clock_res;

		for (int i = 0; i < my_rawx.num_meas; i++)
		{
			GNSS_MEAS temp_meas;
			UbloxSpecs::UBX_RXM_RAWX_SV my_sv;

			// READ
			memcpy(&my_sv.pr_mes, &inbuffer[i * 32 + 22], 8);
			memcpy(&my_sv.cp_mes, &inbuffer[i * 32 + 30], 8);
			memcpy(&my_sv.do_mes, &inbuffer[i * 32 + 38], 4);
			memcpy(&my_sv.gnss_id, &inbuffer[i * 32 + 42], 1);
			memcpy(&my_sv.sv_id, &inbuffer[i * 32 + 43], 1);
			memcpy(&my_sv.freq_id, &inbuffer[i * 32 + 45], 1);
			memcpy(&my_sv.locktime, &inbuffer[i * 32 + 46], 2);
			memcpy(&my_sv.cno, &inbuffer[i * 32 + 48], 1);
			memcpy(&my_sv.pr_stdev, &inbuffer[i * 32 + 49], 1);
			memcpy(&my_sv.cp_stdev, &inbuffer[i * 32 + 50], 1);
			memcpy(&my_sv.do_stdev, &inbuffer[i * 32 + 51], 1);
			memcpy(&my_sv.trk_stat, &inbuffer[i * 32 + 52], 1);

			// MAP to general bin
			temp_meas.C1 = my_sv.pr_mes;
			temp_meas.L1 = my_sv.cp_mes;
			temp_meas.D1 = my_sv.do_mes;
			temp_meas.sv_id = my_sv.sv_id;
			temp_meas.gnss_id = my_sv.gnss_id;
			temp_meas.freq_id = my_sv.freq_id;
			temp_meas.locktime = my_sv.locktime;
			temp_meas.mes_qi = -1;
			temp_meas.cno = my_sv.cno;
			temp_meas.lol = -1;
			temp_meas.C1_stdev = 0.01*pow(2, my_sv.pr_stdev.std_flag);
			temp_meas.L1_stdev = 0.004*my_sv.cp_stdev.std_flag;
			temp_meas.D1_stdev = 0.002*pow(2, my_sv.do_stdev.std_flag);
			temp_meas.C1_valid = my_sv.trk_stat.pr_valid;
			temp_meas.L1_valid = my_sv.trk_stat.cp_valid;
			temp_meas.half_cycle_valid = my_sv.trk_stat.half_cyc;
			temp_meas.hald_cycle_substracted = my_sv.trk_stat.sub_half_cyc;

			temp.meas.push_back(temp_meas);
		}
	}
	return temp;
}

void OutputWriterRinex::write_message(OutputWriterRinex::GNSS_EPOCH my_gnss_epoch) {
	double mjd;
	double sec;
	short int minutes;
	short int month;
	short int year;
	double hour;
	short int day;
	short int year2;
	char buffer[2000]; // buffer aimed to write the file
	int rinex_snr;
	std::string rinex_lli;


	if (my_gnss_epoch.num_meas > 0)
	{
		// get the first date
		mjd = my_gnss_epoch.mjd; // read the mjd of the first epoch
		Utils::YMDHfromMJD(mjd, &year, &month, &day, &hour);

		// get the time of the epoch
		sec = my_gnss_epoch.sec; // read the mjd of the first epoch

		// DEBUG UHRZEUGS
		// sec = round(sec);

		hour = (int)(sec / 3600);
		minutes = (int)((sec - hour * 3600) / 60);
		sec = sec - hour * 3600 - minutes * 60;

		year2 = (short)(year - 100 * trunc(year / 100));

		sprintf(buffer, " %02u %2u %2u %2u %2u %10.7f %2u%3u", (int)year2, (int)month, (int)day, (int)hour, (int)minutes, sec, 0, my_gnss_epoch.num_meas);
		file << buffer;

		std::string gnss_id = " ";
		for (int j = 0; j < my_gnss_epoch.num_meas; j++)
		{
			switch (my_gnss_epoch.meas[j].gnss_id)
			{
			case 0:
				gnss_id = "G"; break; // GPS
			case 1:
				gnss_id = "S"; break; // SBAS
			case 2:
				gnss_id = "E"; break; // Galileo
			case 3:
				gnss_id = "X"; break; // BeiDou
			case 4:
				gnss_id = "X"; break; // IMES
			case 5:
				gnss_id = "X"; break; // QZSS
			case 6:
				gnss_id = "R"; break; // GLONASS
			default:
				gnss_id = "G";
			}
			if ((ceil(j / 12.0) == j / 12.0) && (j != 0)) // if more than 12 sats, use continuation line
			{
				sprintf(buffer, "\n                                ");
				file << buffer;
			}
			sprintf(buffer, "%1s%2u", gnss_id.c_str(), my_gnss_epoch.meas[j].sv_id);
			file << buffer;
		}
		sprintf(buffer, "\n");
		file << buffer;
		/* writing the actual observations */
		for (int j = 0; j < my_gnss_epoch.num_meas; j++)
		{
			/* C/A */
			rinex_snr = std::min(std::max((int)ceil(my_gnss_epoch.meas[j].cno / 6.0), 1), 9);
			sprintf(buffer, "%14.3f  ", my_gnss_epoch.meas[j].C1);
			file << buffer;
			/* L1 */
			rinex_lli = " ";
			if (my_gnss_epoch.meas[j].lol == -1)
			{
				// rinex loss of lock indicator is put to 2 in case of half cycle ambiguities
				if (my_gnss_epoch.meas[j].half_cycle_valid == 0)
				{
					rinex_lli = "2";
				}

			}
			else
			{
				if (my_gnss_epoch.meas[j].lol > 0)
				{
					std::stringstream convert;
					convert << my_gnss_epoch.meas[j].lol;
					rinex_lli = convert.str();
				}
			}
			sprintf(buffer, "%14.3f%1s%1u ", my_gnss_epoch.meas[j].L1, rinex_lli.c_str(), rinex_snr);
			file << buffer;
			/* D1 */
			sprintf(buffer, "%14.3f  ", my_gnss_epoch.meas[j].D1);
			file << buffer;
			/* SNR */
			sprintf(buffer, "%14.3f\n", (double)my_gnss_epoch.meas[j].cno);
			file << buffer;
		}
	}
}

void OutputWriterRinex::parse_and_write(char * inbuffer)
{
	OutputWriterRinex::GNSS_EPOCH my_data;
	my_data = store_RAWX(inbuffer);
	
	// reject all messages with an invalid mjd
	if (abs(mjd_valid - my_data.mjd) > 10)
	{
		return;
	}

	// skip message if rate option was chosen
	if (OutputWriter::rate > 0)
	{
		int divisor = (int)round(my_data.sec / rate);
		double reminder = my_data.sec - divisor * rate;
		if (fabs(reminder) > OutputWriter::threshold)
		{
			return;
		}
	}

	if (OutputWriterRinex::first_message)
	{
		// set the first epoch
		if (my_data.leap_sec_valid)
		{
			set_first_epoch(my_data.mjd, my_data.sec, my_data.leap_sec);
		}
		else
		{
			set_first_epoch(my_data.mjd, my_data.sec, 0);
		}
		set_first_epoch(my_data.mjd, my_data.sec, my_data.leap_sec);

		// write the header
		write_header();
		OutputWriterRinex::first_message = false;
	}

	// write the message
	write_message(my_data);
}

UbloxSpecs::UBX_HEAD OutputWriterRinex::get_reference_header()
{
	UbloxSpecs::UBX_HEAD my_head;
	my_head.cl = 0x02;
	my_head.id = 0x15;
	return my_head;
}
