#include "stdafx.h"
#include "OutputWriter.h"
#include <string>
#include "Utils.h"
#include <time.h>
#include <string>
#include <algorithm>
#include "myConstants.h"
#include <sstream>

OutputWriter::OutputWriter()
{
}

OutputWriter::~OutputWriter()
{
}

void OutputWriter::clean_measurements()
{
	//std::cout << "\n*** clean measurements\n";
	//std::vector<int> all_mjd;
	//std::vector<ubxreader::gnss_epoch> temp;
	//double med_mjd;

	//// get the median mjd in the data
	//all_mjd.reserve(my_gnss_epoch.size());
	//for (int i = 0; i < my_gnss_epoch.size(); i++)
	//{
	//	all_mjd.push_back(my_gnss_epoch[i].mjd);
	//}

	//// compute the median
	//sort(all_mjd.begin(), all_mjd.end());
	//if (all_mjd.size() % 2 == 0){ med_mjd = (all_mjd[all_mjd.size() / 2 - 1] + all_mjd[all_mjd.size() / 2]) / 2; }
	//else { med_mjd = all_mjd[all_mjd.size() / 2]; }

	//// iterate through all epochs to get only valid mjd and obs
	//
	//temp.reserve(my_gnss_epoch.size());
	//for (int i = 0; i < my_gnss_epoch.size(); i++)
	//{
	//	if (abs(med_mjd - my_gnss_epoch[i].mjd) > myconstants::max_mjd_diff)
	//	{
	//		continue;
	//	}
	//	int count_valid = 0; // counter for the number of valid measurements within the epoch
	//	for (int j = 0; j < my_gnss_epoch[i].num_meas; j++)
	//	{
	//		if (my_gnss_epoch[i].meas[j].c1 < myconstants::max_valid_prn)
	//		{
	//			count_valid++;
	//		}
	//	}
	//	if (count_valid == 0) // if there is no valid code measurement at all, we skip the epoch
	//	{
	//		continue;
	//	}
	//	temp.push_back(my_gnss_epoch[i]);
	//}
	//
	//std::cout << temp.size() << " epochs out of " << my_gnss_epoch.size() << " are valid\n";
	//my_gnss_epoch.clear();
	//my_gnss_epoch = temp;
	//bool_clean_obs = true;
}

void OutputWriter::open_file(std::string file_name)
{
	file_name = file_name + ".out";
	file.open(file_name);
}

void OutputWriter::open_file(UbxReader::GNSS_EPOCH my_gnss_epoch, std::string file_name)
{
	double mjd;
	short int month;
	short int year;
	double hour;
	short int day;
	short int doy;
	char doy03[10];
	char year02[10];
	std::ostringstream string_stream;

	/* Create a name following the rinex naming convention */
	// get the first date
	mjd = my_gnss_epoch.mjd; // read the mjd of the first epoch
	Utils::YMDHfromMJD(mjd, &year, &month, &day, &hour);
	Utils::YearDoyFromMJD(mjd, &year, &doy);
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

void OutputWriter::close_file()
{
	file.close();
}


/* Write a Rinex Observation file, format 2.11 */
void OutputWriter::write_rinex_header(UbxReader::GNSS_EPOCH my_gnss_epoch, int receiver_number, std::string receiver_type, int antenna_number, std::string comment){
	std::cout << "\n*** write Rinex observations file header\n";
	if (receiver_type.length() > 20)
	{
		std::cout << "Error: receiver type too long (max. 20)\n";
	}
	if (antenna_number > 9 || antenna_number < 0)
	{
		std::cout << "Error: antenna number between 0 and 9 expected\n";
	}

	double mjd;
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
	// get the first date
	mjd = my_gnss_epoch.mjd; // read the mjd of the first epoch
	Utils::YMDHfromMJD(mjd, &year, &month, &day, &hour);
	Utils::YearDoyFromMJD(mjd, &year, &doy);

	// get local time
	time_t now = time(0);
	tm tm = *localtime(&now);

	// get the time of the first epoch
	sec = my_gnss_epoch.sec; // read the mjd of the first epoch
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
	if (sec = my_gnss_epoch.leap_sec_valid == 1)
	{
		sprintf(buffer, "    %2u                                                      LEAP SECONDS\n", my_gnss_epoch.leap_sec);
		file << buffer;
	}
	//sprintf(buffer, "    %2u                                                      # OF SATELLITES\n", num_sats);
	//file << buffer;
	sprintf(buffer, "                                                            END OF HEADER\n");
	file << buffer;
}

void OutputWriter::write_rinex_obs(UbxReader::GNSS_EPOCH my_gnss_epoch){
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
			if ((ceil(j / 12.0) == j / 12.0) && (j!=0)) // if more than 12 sats, use continuation line
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
			sprintf(buffer, "%14.3f\n", (double) my_gnss_epoch.meas[j].cno);
			file << buffer;
		}
	}
}

void OutputWriter::write_pos(UbxReader::POSLLH my_pos) {
	char buffer[2000]; // buffer aimed to write the file
	// epoch
	sprintf(buffer, "%10.3f; ", my_pos.tow);
	file << buffer;
					   
	// lat long height
	sprintf(buffer, "%14.9f; ", my_pos.lon);
	file << buffer;
	sprintf(buffer, "%14.9f; ", my_pos.lat);
	file << buffer;
	sprintf(buffer, "%14.4f; ", my_pos.height);
	file << buffer;
	sprintf(buffer, "%14.4f; ", my_pos.hmsl);
	file << buffer;

	// accuracies
	sprintf(buffer, "%8.4f; ", my_pos.hz_acc);
	file << buffer;
	sprintf(buffer, "%8.4f\n", my_pos.v_acc);
	file << buffer;
}
