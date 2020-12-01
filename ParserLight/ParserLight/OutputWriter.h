#pragma once
#include "UbxReader.h"
#include <string>
#include "myConstants.h"
#include <sstream>
//#include "MyOstream.cpp"

class OutputWriter
{
public:
	/* Constructor */
	OutputWriter();
	~OutputWriter();

	std::ofstream file;

	static double nom_epoch(double epoch)
	{
		return round(2 * myConstants::MAX_SAMPLING_RATE*epoch) / (2 * myConstants::MAX_SAMPLING_RATE);
	}



	/* Methods */
	void OutputWriter::write_rinex_header(UbxReader::GNSS_EPOCH my_gnss_epoch, int receiver_number, std::string receiver_type, int antenna_number, std::string comment); // Rinex header
	void OutputWriter::write_rinex_obs(UbxReader::GNSS_EPOCH my_gnss_epoch); // Rinex writer 
	void write_pos(UbxReader::POSLLH my_pos);
	void OutputWriter::clean_measurements(); // removes bad measurements from my_gnss_epoch. Should be excecuted before synchronise
	void open_file(std::string file_name);
	void OutputWriter::open_file(UbxReader::GNSS_EPOCH my_gnss_epoch, std::string file_name);
	void OutputWriter::close_file();
};

// void OutputWriter::write_obs_file(std::string file_name, std::string comment); // Pseudo Bernese Format 

