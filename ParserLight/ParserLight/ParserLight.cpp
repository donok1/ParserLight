#include "stdafx.h"
#include "myConstants.h"
#include "UbxReader.h"

#include "OutputWriter.h"
#include "OutputWriterPos.h"
#include "OutputWriterRinex.h"

#include "UbloxSpecs.h"

#include <string>
#include <vector>
#include <bitset>

#include "stdafx.h"

bool check_option(char *value, std::string option);

int main(int argc, char *argv[])
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// test that the variable size implementation is correct in this build
	UbloxSpecs::test_specs();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// general options
	std::string file_name;
	bool opt_wait = false;
	double rate = 0;
	double threshold = 0.02;

	// options for RINEX
	bool rinex_out = false;
	int mjd_valid = 0;
	int receiver_number = 999999;
	std::string receiver_type = "generic";
	int antenna_number = 999999;
	std::string comment = "no comment";

	// options for position output
	bool pos_out = false;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// reading the options
	file_name = argv[1];

	for (int i = 2; i < argc; i = i + 2)
	{

		// general options
		if (strcmp(argv[i], "rate") == 0)
		{
			rate = atof(argv[i + 1]);
		}
		if (strcmp(argv[i], "threshold") == 0)
		{
			threshold = atof(argv[i + 1]);
		}
		else if (strcmp(argv[i], "wait") == 0)
		{
			opt_wait = check_option(argv[i + 1], "wait");
		}


		// rinex options
		if (strcmp(argv[i], "rinex") == 0)
		{
			rinex_out = check_option(argv[i + 1], "rinex");
		}
		if (strcmp(argv[i], "mjd") == 0)
		{
			mjd_valid = atoi(argv[i + 1]);
		}
		if (strcmp(argv[i], "receiver_number") == 0)
		{
			receiver_number = atoi(argv[i + 1]);
		}
		if (strcmp(argv[i], "receiver_type") == 0)
		{
			receiver_type = argv[i + 1];
		}
		if (strcmp(argv[i], "antenna_number") == 0)
		{
			antenna_number = atoi(argv[i + 1]);
		}
		if (strcmp(argv[i], "comment") == 0)
		{
			comment = argv[i + 1];
		}


		// position options
		if (strcmp(argv[i], "llh") == 0)
		{
			pos_out = check_option(argv[i + 1], "llh");
		}
		else
		{
			std::cout << argv[i] << " option unknown. Option ignored.\n";
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// check for option consistency
	if (rinex_out)
	{
		if (mjd_valid == 0)
		{
			std::cout << "mjd_validity is requested\nAbort.";

			if (opt_wait)
			{
				std::cout << "\n\nFinished. Press any button\n";
				getchar();
			}

			return 0;
		}
	}
	
	OutputWriter::rate = rate; // set the rate for all classes [seconds]
	OutputWriter::threshold = threshold; // set the threshold applied to the rate [seconds]
	UbxReader myReader;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// register readers
	if (rinex_out)
	{
		OutputWriterRinex* output_writer_rinex = new OutputWriterRinex(mjd_valid, receiver_number, receiver_type, antenna_number, comment);
		myReader.register_reader(output_writer_rinex);
	}
	
	if (pos_out)
	{
		OutputWriterPos* output_writer_pos = new OutputWriterPos();
		myReader.register_reader(output_writer_pos);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// processing
	myReader.open_file(file_name);
	myReader.read_any_message();
	myReader.close_file();

	if (opt_wait)
	{
		std::cout << "\n\nFinished. Press any button\n";
		getchar();
	}
	return 0;

}

//---------------------------------------------------------------------------------
bool check_option(char *value, std::string option)
{
	bool my_bool;
	switch (atoi(value))
	{
	case 0:
		my_bool = false; break;
	case 1:
		my_bool = true; break;
	default:
		std::cout << "Invalid argument for option " << option << ". Option set to false.\n";
		my_bool = false;
	}
	return my_bool;
}
//---------------------------------------------------------------------------------