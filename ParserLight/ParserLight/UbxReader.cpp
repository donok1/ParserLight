#include "stdafx.h"
#include "UbxReader.h"
#include "OutputWriter.h"
#include <string>
#include <vector>
#include "myConstants.h"
#include <bitset>

UbxReader::UbxReader()
{
	file_size = 0;
	rate = 0;

	/* Consitency check of variable size. One never knows... */
	if (sizeof(ub_u1) != 1){ std::cerr << "WARNING: ub_u1 has wrong size!\n"; };
	if (sizeof(ub_u2) != 2){ std::cerr << "WARNING: ub_u2 has wrong size!\n"; };
	if (sizeof(ub_u4) != 4){ std::cerr << "WARNING: ub_u4 has wrong size!\n"; };
	if (sizeof(ub_i1) != 1){ std::cerr << "WARNING: ub_i1 has wrong size!\n"; };
	if (sizeof(ub_i2) != 2){ std::cerr << "WARNING: ub_i2 has wrong size!\n"; };
	if (sizeof(ub_i4) != 4){ std::cerr << "WARNING: ub_i4 has wrong size!\n"; };
	if (sizeof(ub_r4) != 4){ std::cerr << "WARNING: ub_r4 has wrong size!\n"; };
	if (sizeof(ub_r8) != 8){ std::cerr << "WARNING: ub_r8 has wrong size!\n"; };
}

UbxReader::UbxReader(double rate)
{
	file_size = 0;
	UbxReader::rate = rate;

	/* Consitency check of variable size. One never knows... */
	if (sizeof(ub_u1) != 1) { std::cerr << "WARNING: ub_u1 has wrong size!\n"; };
	if (sizeof(ub_u2) != 2) { std::cerr << "WARNING: ub_u2 has wrong size!\n"; };
	if (sizeof(ub_u4) != 4) { std::cerr << "WARNING: ub_u4 has wrong size!\n"; };
	if (sizeof(ub_i1) != 1) { std::cerr << "WARNING: ub_i1 has wrong size!\n"; };
	if (sizeof(ub_i2) != 2) { std::cerr << "WARNING: ub_i2 has wrong size!\n"; };
	if (sizeof(ub_i4) != 4) { std::cerr << "WARNING: ub_i4 has wrong size!\n"; };
	if (sizeof(ub_r4) != 4) { std::cerr << "WARNING: ub_r4 has wrong size!\n"; };
	if (sizeof(ub_r8) != 8) { std::cerr << "WARNING: ub_r8 has wrong size!\n"; };
}

UbxReader::~UbxReader()
{

}

bool UbxReader::open_file(std::string file_name){
	bool check = false;

	// Read input binary file
	int length = 0;
	int k = 0;

	std::cout << "\n*** load file\n";

	file.open(file_name, std::ios::in | std::ios::binary);
	if (file.is_open())
	{
		std::cout << "File open\n";
		file.seekg(0, std::ios::end);
		file_size = file.tellg();
		file.seekg(0, std::ios::beg);
		check = true;
	}
	else
	{
		std::cout << "Unable to open file\n";
		check = false;
	}
	return check;
}

void UbxReader::close_file(){
	file.close();
}

void UbxReader::extract_message(UbxReader::UBX_HEAD ub_head, char *message, bool *found)
{
	UbxReader::UBX_CHECKSUM ub_chksum;
	*found = false;
	char target[4] = { 0xb5, 0x62, ub_head.cl, ub_head.id };

	int aa = 0;
	// search 
	while (file.tellg() < (size_t)file_size - 6)
	{
		char file_section[6];
		file.read(file_section, 6);
		file.seekg(-6, std::ios::cur);
		if (memcmp(&file_section[0], &target, 2) == 0) // any message found
		{
			memcpy(&ub_head.cl, &file_section[2], 1);	// get the class
			memcpy(&ub_head.id, &file_section[3], 1);	// get the id
			memcpy(&ub_head.len, &file_section[4], 2);	// get the length of the message	
			if (ub_head.len > myConstants::MAX_MESSAGE_LENGTH) // too long messages are ignored
			{
				file.seekg(1, std::ios::cur); // increment cursor position
				continue;
			}

			if ((size_t)file_size - file.tellg() >= 8 + ub_head.len)
			{
				// Checksum
				char payload[myConstants::MAX_MESSAGE_LENGTH+2];				
				file.read(payload, ub_head.len+8);
				file.seekg(-ub_head.len - 8, std::ios::cur);

				ub_chksum.a = payload[6 + ub_head.len];
				ub_chksum.b = payload[6 + ub_head.len + 1];

				if (ub_checksum(&payload[6], ub_head, ub_chksum)) // checksum valid
				{
					memcpy(message, payload, ub_head.len + 8);
					if (memcmp(payload, &target, 4) == 0) // found requested message
					{
						file.seekg(6 + ub_head.len + 2, std::ios::cur);
						*found = true;
						return;
					}
					else
					{
						file.seekg(6 + ub_head.len + 2, std::ios::cur);
						// std::cout << "wrong message\n";
						continue;
					}
				}
				else // checksum invalid
				{
					file.seekg(1, std::ios::cur);
					continue;
				}
			}
			else // end of bitstring reached
			{
				std::cout << "end of bytestring\n";
				break;
			}
		}

		// no message found at all
		file.seekg(1, std::ios::cur);
	}

	std::cout << " EOF reached\n";
	file.seekg(0, std::ios::end);
	message = NULL;
	return;
}

int UbxReader::count_messages(UbxReader::UBX_HEAD ub_head)
{
	bool found;
	int counter = 0;
	while (true)
	{
		if (file.tellg() >= file_size)
		{
			break;
		}
		char message[myConstants::MAX_MESSAGE_LENGTH];
		UbxReader::extract_message(ub_head, message, &found);
		if (found)
		{
			counter = counter + 1;
		}
	}
	file.seekg(0, std::ios::beg); // reset cursor position
	return counter;
}

UbxReader::GNSS_EPOCH UbxReader::store_RAWX(char *inbuffer)
{
	int my_mjd;
	double my_sec;
	UbxReader::UBX_HEAD ub_head;
	ub_head.cl = 0x02; // RAWX class
	ub_head.id = 0x15; // RAWX id

	UBX_RXM_RAWX my_rawx;
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
		get_mjd(1000.0*my_rawx.rcv_tow, my_rawx.week, &my_mjd, &my_sec); // time conversion
		temp.mjd = my_mjd;
		temp.sec = my_sec;
		temp.num_meas = my_rawx.num_meas;
		temp.leap_sec = my_rawx.leap_s;
		temp.leap_sec_valid = my_rawx.rec_stat.leap_sec;
		temp.clock_reset = my_rawx.rec_stat.clock_res;

		for (int i = 0; i < my_rawx.num_meas; i++)
		{
			GNSS_MEAS temp_meas;
			UBX_RXM_RAWX_SV my_sv;

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

void UbxReader::read_RAWX(int mjd_check, std::string out_file)
{
	UbxReader::UBX_HEAD ub_head;
	int counter_invalid = 0;
	bool found = false;
	bool first = true;
	ub_head.cl = 0x02; // RAWX class
	ub_head.id = 0x15; // RAWX id
	UbxReader::GNSS_EPOCH my_epoch;
	OutputWriter myWriter;

	std::cout << "\n*** read RAWX\n";

	std::cout << file_size << " is the file size\n";

	while (true)
	{

		if (file.tellg() >= file_size)
		{
			break;
		}
		char message[myConstants::MAX_MESSAGE_LENGTH];
		UbxReader::extract_message(ub_head, message, &found);

		if (found)
		{
			my_epoch = UbxReader::store_RAWX(message);
			// check that mjd is within validity range
			if (abs(mjd_check - my_epoch.mjd) > 100)
			{
				counter_invalid++;
				continue;
			}

			// skip if rate option was chosen
			if (rate > 0)
			{
				int divisor = (int)round(my_epoch.sec / rate);
				double reminder = my_epoch.sec - divisor * rate;
				if (fabs(reminder) > threshold)
				{
					continue;
				}
			}

			if (first)
			{
				myWriter.open_file(my_epoch, out_file);
				first = false;
				myWriter.write_rinex_header(my_epoch, 0, "ublox", 0, "Hallo Dani");
			}
			myWriter.write_rinex_obs(my_epoch);
		}
	}
	myWriter.close_file();

	std::cout << counter_invalid << " Epochs removed (invalid MJD)\n";

	return;
}

UbxReader::POSLLH UbxReader::store_HPPOSLLH(char *inbuffer)
{
	UbxReader::UBX_HEAD ub_head;
	ub_head.cl = 0x01; // RAWX class
	ub_head.id = 0x14; // RAWX id

	UBX_NAV_HPPOSLLH my_hpposllh;
	POSLLH temp;

	unsigned char head[4] = { 0xb5, 0x62, ub_head.cl, ub_head.id }; // header

	char inhead[4] = { inbuffer[0], inbuffer[1], inbuffer[2], inbuffer[3] };

	if (memcmp(&head, &inhead, 4) == 0)
	{
		ub_head.cl = inbuffer[2];
		ub_head.id = inbuffer[3];
		memcpy(&ub_head.len, &inbuffer[4], 2);	//length

		// READ
		memcpy(&my_hpposllh.version, &inbuffer[6], 1);
		memcpy(&my_hpposllh.reserved1, &inbuffer[7], 2);
		memcpy(&my_hpposllh.validity, &inbuffer[9], 1);
		memcpy(&my_hpposllh.iTOW, &inbuffer[10], 4);
		memcpy(&my_hpposllh.lon, &inbuffer[14], 4);
		memcpy(&my_hpposllh.lat, &inbuffer[18], 4);
		memcpy(&my_hpposllh.height, &inbuffer[22], 4);
		memcpy(&my_hpposllh.hMSL, &inbuffer[26], 4);
		memcpy(&my_hpposllh.lonHp, &inbuffer[30], 1);
		memcpy(&my_hpposllh.latHp, &inbuffer[31], 1);
		memcpy(&my_hpposllh.heightHp, &inbuffer[32], 1);
		memcpy(&my_hpposllh.hMSLHp, &inbuffer[33], 1);
		memcpy(&my_hpposllh.hAcc, &inbuffer[34], 4);
		memcpy(&my_hpposllh.hVacc, &inbuffer[38], 4);
		
		temp.tow = my_hpposllh.iTOW/1000.0;
		temp.lon = 1.0e-7*my_hpposllh.lon + 1.0e-9*my_hpposllh.lonHp;
		temp.lat = 1.0e-7*my_hpposllh.lat + 1.0e-9*my_hpposllh.latHp;
		temp.height = 1.0e-3*(my_hpposllh.height + 0.1*my_hpposllh.heightHp);
		temp.hmsl = 1.0e-3*(my_hpposllh.hMSL + 0.1*my_hpposllh.hMSLHp);
		temp.hz_acc = 1.0e-4*my_hpposllh.hAcc;
		temp.v_acc = 1.0e-4*my_hpposllh.hVacc;
	}

	return temp;
}

void UbxReader::read_HPPOSLLH(std::string out_file)
{
	UbxReader::UBX_HEAD ub_head;
	bool found = false;
	bool first = true;
	ub_head.cl = 0x01; // RAWX class
	ub_head.id = 0x14; // RAWX id
	UbxReader::POSLLH my_pos;
	OutputWriter myWriter;
	double progress;
	double progress_old = -1;

	std::cout << "\n*** read HP POS LLH\n";

	std::cout << file_size << " is the file size\n";

	while (true)
	{

		if (file.tellg() >= file_size)
		{
			break;
		}
		char message[myConstants::MAX_MESSAGE_LENGTH];
		UbxReader::extract_message(ub_head, message, &found);

		progress = round(1e2*((double)file.tellg() / (double)file_size));

		if (found)
		{
			// parse the message
			my_pos = UbxReader::store_HPPOSLLH(message);
			
			// skip if rate option was chosen
			if (rate > 0)
			{
				int divisor = (int)round(my_pos.tow/rate);
				double reminder = my_pos.tow - divisor * rate;
				
				// std::cout << std::setprecision(6) << divisor << " " << reminder << "\n";
				
				if (fabs(reminder) > threshold)
				{
					continue;
				}
			}

			if (first)
			{
				myWriter.open_file(out_file);
				first = false;
			}
			myWriter.write_pos(my_pos);
		}

		if (progress != progress_old)
		{
			std::cout << "Progress: " << progress << "%\n";
			progress_old = progress;
		}

	}
	std::cout << "\n";
	myWriter.close_file();

	return;
}