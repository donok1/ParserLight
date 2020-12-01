# ParserLight
Parser for ublox UBX files

# Call
ParserLight ubxdatatest20112020.ubx rate 60 llh 1 wait 1 
ParserLight ant2_az0.ubx rinex 1 mjd 57919 wait 1 

## Options
### rinex
Enable (1) or disable (0) Rinex Observations files
If this option is enabled, mjd is mandatory

### mjd
It is mandatory to provide the Modified Julian Date of the desired epochs if a rinex output is chosen

### rate
Output message rate in seconds. The default value is 0, which will simply output all messages

### llh
Enable (1) or disable (0) the output of the UBX-NAV-HPPOSLLH as csv file

### wait
Tell the console to stay open at the end of the processing (1) or close automatically (0)



