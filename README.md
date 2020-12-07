# ParserLight
Parser for ublox UBX files

# Call
ParserLight input_file [argument value]

For instance:
ParserLight.exe ubxdatatest20112020.ubx rate 60 pos 1 wait 1 
ParserLight.exe ant2_az0.ubx rinex 1 mjd 57919 wait 1 

## Options

### General options
#### rate
Output rate in seconds. Default = 0 exports all messages.

#### threshold
Threshold for the above mentionned option. Default is 0.02 seconds.

#### wait
Tell the console to stay open at the end of the processing (1) or close automatically (0). Default = 0.

#### o
Optional output file name. Default = input file name

### Rinex optios
Rinex observation files output.

#### rinex
Enable (1) or disable (0) Rinex Observations 2.11 format files (Rinex 3.0 is not implemented yet, only Obs files are generated).
If this option is enabled, mjd is mandatory.

#### mjd
It is mandatory to provide the Modified Julian Date of the desired epochs if a rinex output is chosen.

### Rinex header options
#### run_by
#### observer
#### agency
#### marker_name
#### marker_number
#### receiver_number
#### receiver_type
#### receiver_version
#### antenna_number
#### antenna_type
#### approx_pos_x
#### approx_pos_y
#### approx_pos_z
#### delta_h
#### delta_e
#### delta_n
#### comment

### Pos options
Output of csv files with the position information (from the HPPOSLLH message).

### pos
Enable (1) or disable (0) the output of the UBX-NAV-HPPOSLLH message content in an output csv file.

# Compilation under Linux
Software not tested under Linux!

stdafx_for_linux.h instead of stdafx.h (rename it accordingly)

g++ -c stdafx.h -o stdafx.h.gch

g++ -std=c++11 ParserLight.cpp OutputWriter.cpp Utils.cpp UbloxSpecs.cpp UbxReader.cpp OutputWriterRinex.cpp OutputWriterPos.cpp -I . -o ParserLight

