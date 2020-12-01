#include "stdafx.h"
#include "UbxReader.h"
#include <string>
#include <vector>
#include "myConstants.h"
#include <bitset>

#include "stdafx.h"

bool check_option(char *value, std::string option);

int main(int argc, char *argv[])
{
	//
	int mjd_validity = 0;
	double rate = 0;
	bool rinex = false;
	bool llh = false;
	std::string file_name;
	file_name = argv[1];

	// options
	bool opt_wait = false;

	for (int i = 2; i < argc; i = i + 2)
	{
		if (strcmp(argv[i], "rinex") == 0)
		{
			rinex = check_option(argv[i + 1], "rinex");
		}
		if (strcmp(argv[i], "rate") == 0)
		{
			rate = atof(argv[i + 1]);
		}
		if (strcmp(argv[i], "llh") == 0)
		{
			llh = check_option(argv[i + 1], "llh");
		}
		if (strcmp(argv[i], "mjd") == 0)
		{
			mjd_validity = atoi(argv[i + 1]);
		}
		else if (strcmp(argv[i], "wait") == 0)
		{
			opt_wait = check_option(argv[i + 1], "wait");
		}
		else
		{
			std::cout << argv[i] << " option unknown. Option ignored.\n";
		}
	}
	
	if (rinex)
	{
		if (mjd_validity == 0)
		{
			std::cout << "mjd_validity is requested\nAbort.";

			if (opt_wait)
			{
				std::cout << "\n\nFinished. Press any button\n";
				getchar();
			}

			return 0;
		}

		std::cout << file_name;

		// load file
		UbxReader myReader(rate);
		bool my_check = myReader.open_file(file_name);
	
		int ind;
		ind = file_name.find_last_of(".");
		file_name = file_name.substr(0, ind);

		myReader.read_RAWX(mjd_validity, file_name);
	
		// close the input file
		myReader.close_file();
	}

	if (llh)
	{
		// load file
		UbxReader myReader(rate);
		bool my_check = myReader.open_file(file_name);

		int ind;
		ind = file_name.find_last_of(".");
		file_name = file_name.substr(0, ind);
		myReader.read_HPPOSLLH(file_name);
		
		// close the input file
		myReader.close_file();
	}


	if (opt_wait)
	{
		std::cout << "\n\nFinished. Press any button\n";
		getchar();
	}

	return 0;

}

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