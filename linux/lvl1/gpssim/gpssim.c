/* GPSSIM -- emulate output of a GPS unit as if it were undergoing a scripted balloon flight  
*/

/* Define ARDUINO here for Arduino or Diavolino version -- otherwise assume Windows or Linux */
/* #define ARDUINO */
/* #define ARDUINO */

/* 
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330,
  Boston, MA 02111-1307, USA.
*/

/* GPSSIM Release version 1.03 -- 03/14/2011 Gary L. Flispart (GLF) for LVL1 White Star Balloon Group
   
       GPSSIM outputs a stream of simulated GPS data to the serial port which 
       matches a simulated balloon flight, including latitude, longitude, altitude,
       and vector heading angle.  The flight positions at any given second are 
       determined by interpreting a "script" incorporated directrly into the program
       as data values, representing latitude, longitude, and altitude at specific 
       dates and times.  Values between these scripted points are interpolated.
       The data are emitted in REAL TIME, one reading group per second.
       
       The purpose of this program is to allow testing of the mission computer for
       an autonomous transatlantic "weather" balloon being developed by the White Star 
       Group, an interest group operating within the LVL1 Hackerspace, a group of
       technology enthusiasts in Louisville, KY.

       The code in this version was ported to work on an Arduino Duemilanove (or Diavolino).
       This platform assumes 32K Flash and 2K RAM.

       The Arduino version of GPSSIM has been developed and tested using the "arduino-0018"
       development environment.  No special compilation procedures are needed -- all libraries 
       used are included with the base environment.
              
       The code was originally developed (modified from GLF balscrip.c) under Windows, 
       maintained portable to Linux.  The code was simplified and "boiled down" to fit 
       into the limited memory model of an Arduino.  
	   
	   On the Arduino, the GPS strings are emitted correctly in "singsong" linear interpolation 
	   between scripted data points. The Windows/Linux version allows randomized variability in 
	   interpolated points to look more real-world realistic.  The necessary tables must be ported 
	   to FLASH ROM for this to work with limited Arduino RAM (not yet done).
       
*/                

/* gpssim for Windows, Linux, or Arduino -- Output NMEA sentences to serial port (or capture file)

                 Takes script data describing the flight of a 
                 weather ballon at various date, time, latitude, longitude, 
                 and altitude waypoints and simulates one-second position data 
                 compatible with simulation of an assumed onboard GPS device. 

				 Arduino:
				    The simulated GPS data will be output in REAL TIME by compilation default, 
					one second between data groups -- if the simulation is a 2-day run, it will 
					take 2 days to output all data to the port.  
					
				 Command line for Windows/Linux ONLY:
                    If a comport is specified, the simulated GPS data will be 
                    output in REAL TIME by default, one second between data groups 
                    -- if the simulation is a 2-day run, it will take 2 days to 
                    output all data to the port.  
					 
                    If instead no port is specified, output will go to the screen (stdout) as fast 
					as possible, without realtime delay.  			
					 
                    Invoking from command line:
                       
                       Windows:   					   
                          32gpssim [port spec] [port baud]
                       Linux terminal:   					   
                          ./lxgpssim [port spec] [port baud]
                    
                    If parameters are missing, output will go to the console
                    (can be redirected to file if desired). 
                    
                 04 Dec 2010 GLF (Gary L. Flispart)   
                    BETA version 0.9 --  GPS output to either COM port or debugging 
                                     file OK now -- tracking calculations 
                                     improved --  variability parameter 
                                     now applied to 1-second data (0 uses
                                     straight linear interpolation, 
                                     6 looks realistically like stable winds,
                                     100 (max) looks "zigzaggy" -- default = 4).
									 
                 26 Jan 2011 GLF (Gary L. Flispart)   
                    BETA version 0.95 --  (Backported from Arduino) 
					                 Modifications to output NMEA GSA sentence
									 Some modifications of hardcoded script
									 Include NMEA 2.3 compile option
									 Simulate reception dropouts about once per 6 hours

		 30 Jan 2011 GLF (Gary L. Flispart)
                    Release version 1.00 --  (Backported from Arduino) REALTIME enabled for release
                                     Bug fixed in GGA format (left off a field earlier)
                                     Finalized characteristics of fields during dropouts
                                     Moved flight script table to Flash ROM to allow full size table
                                        on Arduino while saving RAM
                                     Random variations (for realistic "bouncing") not yet implemented 
                                        on Arduino but (Windows/Linux) code in place
                                     All strings output each second with appropriate VOID coding
                                     Favorably matched simulation NMEA 2.3 against real NMEA 2.3 
                                        output of Garmin eTrex GPS
                                     Settled on simulated new satellite acquisition cycle of 17 seconds,
                                        with a forced simulated signal dropout every 3.7 hours lasting 
                                        13.85 minutes to allow control software to handle prolonged 
                                        period without a GPS fix                                        
                    
		 05 Feb 2011 GLF (Gary L. Flispart)
                    Release version 1.01 -- Code rendered OS portable and baud rate set to 57600
				     Fixed randomization code to make portable to Linux					 

		 10 Mar 2011 GLF (Gary L. Flispart)
                    Release version 1.02 -- Fixed bug in RMC and GSA records which
                                     incorrectly identified "no fix" data as A=ACTIVE instead
                                     of V=VOID as desired.  Also added arbitrary extra days to 
                                     original flight script to force a 72+ hour simulation,
                                     per request of balloon team.

		 14 Mar 2011 GLF (Gary L. Flispart)
                    Release version 1.03 -- Added  "#define PERFECT_SAT_FIXES" to
                    allow simulation with no dropouts, per request of ballooon team.
*/

/*
This program is intended to output streams of simulated GPS serial data in the NMEA format.

The following NMEA reference and description is excerpted from the following website 
(not written by the author of GPSSIM):

     http://www.gpsinformation.org/dale/nmea.htm
 
---------------------------------------------------------------------------------------------
Introduction

The National Marine Electronics Association (NMEA) has developed a specification that defines 
the interface between various pieces of marine electronic equipment. The standard permits marine 
electronics to send information to computers and to other marine equipment. A full copy of this 
standard is available for purchase at their web site. None of the information on this site comes 
from this standard and I do not have a copy. Anyone attempting to design anything to this 
standard should obtain an official copy.

GPS receiver communication is defined within this specification. Most computer programs that 
provide real time position information understand and expect data to be in NMEA format. This data 
includes the complete PVT (position, velocity, time) solution computed by the GPS receiver. The 
idea of NMEA is to send a line of data called a sentence that is totally self contained and 
independent from other sentences. There are standard sentences for each device category and there 
is also the ability to define proprietary sentences for use by the individual company. All of the 
standard sentences have a two letter prefix that defines the device that uses that sentence type. 
(For gps receivers the prefix is GP.) which is followed by a three letter sequence that defines 
the sentence contents. In addition NMEA permits hardware manufactures to define their own 
proprietary sentences for whatever purpose they see fit. All proprietary sentences begin with the 
letter P and are followed with 3 letters that identifies the manufacturer controlling that 
sentence. For example a Garmin sentence would start with PGRM and Magellan would begin with PMGN.

Each sentence begins with a '$' and ends with a carriage return/line feed sequence and can be no 
longer than 80 characters of visible text (plus the line terminators). The data is contained within 
this single line with data items separated by commas. The data itself is just ascii text and may 
extend over multiple sentences in certain specialized instances but is normally fully contained in 
one variable length sentence. The data may vary in the amount of precision contained in the message. 
For example time might be indicated to decimal parts of a second or location may be show with 3 or 
even 4 digits after the decimal point. Programs that read the data should only use the commas to 
determine the field boundaries and not depend on column positions. There is a provision for a 
checksum at the end of each sentence which may or may not be checked by the unit that reads the 
data. The checksum field consists of a '*' and two hex digits representing an 8 bit exclusive OR 
of all characters between, but not including, the '$' and '*'. A checksum is required on some 
sentences.

There have been several changes to the standard but for gps use the only ones that are likely to be 
encountered are 1.5 and 2.0 through 2.3. These just specify some different sentence configurations 
which may be peculiar to the needs of a particular device thus the gps may need to be changed to 
match the devices being interfaced to. Some gps's provide the ability configure a custom set the 
sentences while other may offer a set of fixed choices. Many gps receivers simply output a fixed 
set of sentences that cannot be changed by the user. The current version of the standard is 3.01. 
I have no specific information on this version, but I am not aware of any GPS products that require 
conformance to this version.


Hardware Connection

The hardware interface for GPS units is designed to meet the NMEA requirements. They are also 
compatible with most computer serial ports using RS232 protocols, however strictly speaking the 
NMEA standard is not RS232. They recommend conformance to EIA-422. The interface speed can be adjusted 
on some models but the NMEA standard is 4800 b/s (bit per second rate) with 8 bits of data, no parity, 
and one stop bit. All units that support NMEA should support this speed. Note that, at a b/s rate of 
4800, you can easily send enough data to more than fill a full second of time. For this reason some 
units only send updates every two seconds or may send some data every second while reserving other 
data to be sent less often. In addition some units may send data a couple of seconds old while other 
units may send data that is collected within the second it is sent. Generally time is sent in some 
field within each second so it is pretty easy to figure out what a particular gps is doing. Some 
sentences may be sent only during a particular action of the receiver such as while following a 
route while other receivers may always send the sentence and just null out the values. Other 
difference will be noted in the specific data descriptions defined later in the text.

At 4800 b/s you can only send 480 characters in one second. Since an NMEA sentence can be as long 
as 82 characters you can be limited to less than 6 different sentences. The actual limit is 
determined by the specific sentences used, but this shows that it is easy to overrun the capabilities 
if you want rapid sentence response. NMEA is designed to run as a process in the background 
spitting out sentences which are then captured as needed by the using program. Some programs 
cannot do this and these programs will sample the data stream, then use the data for screen 
display, and then sample the data again. Depending on the time needed to use the data there can 
easily be a lag of 4 seconds in the responsiveness to changed data. This may be fine in some 
applications but totally unacceptable in others. For example a car traveling at 60 mph will travel 
88 feet in one second. Several second delays could make the entire system seem unresponsive and 
could cause you to miss your turn.

The NMEA standard has been around for many years (1983) and has undergone several revisions. 
The protocol has changed and the number and types of sentences may be different depending on 
the revision. Most GPS receivers understand the standard which is called: 0183 version 2. 
This standard dictates a transfer rate of 4800 b/s. Some receivers also understand older 
standards. The oldest standard was 0180 followed by 0182 which transferred data at 1200 b/s. 
An earlier version of 0183 called version 1.5 is also understood by some receivers. Some 
Garmin units and other brands can be set to 9600 for NMEA output or even higher but this is 
only recommended if you have determined that 4800 works ok and then you can try to set it 
faster. Setting it to run as fast as you can may improve the responsiveness of the program.

In order to use the hardware interface you will need a cable. Generally the cable is unique 
to the hardware model so you will need an cable made specifically for the brand and model of the 
unit you own. Some of the latest computers no longer include a serial port but only a USB port. 
Most gps receivers will work with Serial to USB adapters and serial ports attached via the 
pcmcia (pc card) adapter. For general NMEA use with a gps receiver you will only need two wires 
in the cable, data out from the gps and ground. A third wire, Data in, will be needed if you 
expect the receiver to accept data on this cable such as to upload waypoints or send DGPS 
data to the receiver.

GPS receivers may be used to interface with other NMEA devices such as autopilots, fishfinders, 
or even another gps receivers. They can also listen to Differential Beacon Receivers that can 
send data using the RTCM SC-104 standard. This data is consistent with the hardware requirements 
for NMEA input data. There are no handshake lines defined for NMEA.


NMEA sentences

NMEA consists of sentences, the first word of which, called a data type, defines the 
interpretation of the rest of the sentence. Each Data type would have its own unique 
interpretation and is defined in the NMEA standard. The GGA sentence (shown below) 
shows an example that provides essential fix data. Other sentences may repeat some 
of the same information but will also supply new data. Whatever device or program 
that reads the data can watch for the data sentence that it is interested in and 
simply ignore other sentences that is doesn't care about. In the NMEA standard there 
are no commands to indicate that the gps should do something different. Instead each 
receiver just sends all of the data and expects much of it to be ignored. Some 
receivers have commands inside the unit that can select a subset of all the sentences 
or, in some cases, even the individual sentences to send. There is no way to indicate 
anything back to the unit as to whether the sentence is being read correctly or to 
request a re-send of some data you didn't get. Instead the receiving unit just checks 
the checksum and ignores the data if the checksum is bad figuring the data will be 
sent again sometime later.

There are many sentences in the NMEA standard for all kinds of devices that may be used 
in a Marine environment. Some of the ones that have applicability to gps receivers are 
listed below: (all message start with GP.)

    * AAM - Waypoint Arrival Alarm
    * ALM - Almanac data
    * APA - Auto Pilot A sentence
    * APB - Auto Pilot B sentence
    * BOD - Bearing Origin to Destination
    * BWC - Bearing using Great Circle route
    * DTM - Datum being used.
    * GGA - Fix information
    * GLL - Lat/Lon data
    * GRS - GPS Range Residuals
    * GSA - Overall Satellite data
    * GST - GPS Pseudorange Noise Statistics
    * GSV - Detailed Satellite data
    *
    * MSK - send control for a beacon receiver
    * MSS - Beacon receiver status information.
    * RMA - recommended Loran data
    * RMB - recommended navigation data for gps
    * RMC - recommended minimum data for gps
    * RTE - route message
    * TRF - Transit Fix Data
    * STN - Multiple Data ID
    * VBW - dual Ground / Water Spped
    * VTG - Vector track an Speed over the Ground
    * WCV - Waypoint closure velocity (Velocity Made Good)
    * WPL - Waypoint Location information
    * XTC - cross track error
    * XTE - measured cross track error
    * ZTG - Zulu (UTC) time and time to go (to destination)
    * ZDA - Date and Time 

Some gps receivers with special capabilities output these special messages.

    * HCHDG - Compass output
    * PSLIB - Remote Control for a DGPS receiver 

In addition some GPS receivers can mimic Loran-C receivers by outputing the LC prefix 
in some of their messages so that they can be used to interface to equipment that is 
expecting this prefix instead of the GP one.

The last version 2 iteration of the NMEA standard was 2.3. It added a mode indicator 
to several sentences which is used to indicate the kind of fix the receiver currently
has. This indication is part of the signal integrity information needed by the FAA. 
The value can be A=autonomous, D=differential, E=Estimated, N=not valid, S=Simulator.
Sometimes there can be a null value as well. Only the A and D values will correspond 
to an Active and reliable Sentence. This mode character has been added to the RMC, 
RMB, VTG, and GLL, sentences and optionally some others including the BWC and XTE 
sentences.

If you are interfacing a GPS unit to another device, including a computer program, 
you need to ensure that the receiving unit is given all of the sentences that it needs. 
If it needs a sentence that your GPS does not send then the interface to that unit is 
likely to fail. Here is a Link for the needs of some typical programs. The sentences 
sent by some typical receivers include:

NMEA 2.0
Name 	Garmin	Magellan	Lowrance	SiRF	Notes:
GPAPB 	N	Y	Y	N	Auto Pilot B
GPBOD 	Y	N	N	N	bearing, origin to destination - earlier G-12's do not transmit this
GPGGA 	Y	Y	Y	Y	fix data
GPGLL 	Y	Y	Y	Y	Lat/Lon data - earlier G-12's do not transmit this
GPGSA 	Y	Y	Y	Y	overall satellite reception data, missing on some Garmin models
GPGSV 	Y	Y	Y	Y	detailed satellite data, missing on some Garmin models
GPRMB 	Y	Y	Y	N	minimum recommended data when following a route
GPRMC 	Y	Y	Y	Y	minimum recommended data
GPRTE	Y	U	U 	N	route data, only when there is an active route. (this is sometimes bidirectional)
GPWPL	Y	Y	U	N 	waypoint data, only when there is an active route (this is sometimes bidirectional)

NMEA 1.5 - some units do not support version 1.5. Lowrance units provide the ability 
to customize the NMEA output by sentences so that you can develop your own custom 
sentence structure.

Name	Garmin	Magellan	Notes:
GPAPA	N	Y	Automatic Pilot A
GPBOD	Y	N	bearing origin to destination - earlier G-12's do not send this
GPBWC	Y	Y	bearing to waypoint using great circle route.
GPGLL	Y	Y	lat/lon - earlier G-12's do not send this
GPRMC	Y	N	minimum recommend data
GPRMB	Y	N	minimum recommended data when following a route
GPVTG	Y	Y	vector track and speed over ground
GPWPL	Y	N	waypoint data (only when active goto)
GPXTE	Y	Y	cross track error

The NMEA 2.3 output from the Garmin Legend, Vista, and perhaps some others include the 
BWC, VTG, and XTE sentences.

The Trimble Scoutmaster outputs: APA, APB, BWC, GGA, GLL, GSA, GSV, RMB, RMC, VTG, WCV, 
XTE, ZTG.

The Motorola Encore outputs: GGA, GLL, GSV, RMC, VTG, ZDA and a proprietary sentence PMOTG.

Units based on the SiRF chipset can output: GGA, GLL, GSA, GSV, RMC, and VTG. What is 
actually output is based on which sentences are selected by the user or application 
program. See below for more details. Some implementations have enhanced the SiRF 
capabilities with other sentences as well by changing the firmware. For example, 
the u-blox receivers add ZDA and some proprietary sentences to the above list of sentences. 
Check your documentation for more details.

Garmin receivers send the following Proprietary Sentences:

    * PGRME (estimated error) - not sent if set to 0183 1.5
    * PGRMM (map datum)
    * PGRMZ (altitude)
    * PSLIB (beacon receiver control) 

Note that Garmin converts lat/lon coordinates to the datum chosen by the user when 
sending this data. This is indicated in the proprietary sentence PGRMM. This can 
help programs that use maps with other datums but is not an NMEA standard. Be sure 
and set your datum to WGS84 on Garmin units when communicating to other NMEA devices.

Magellan also converts lat/lon coordinates to the datum chosen on the receiver but 
do not indicate this in a message. Magellan units use proprietary sentences for 
waypoint maintenance and other tasks. They use a prefix of PMGN for this data.

Most other units always output NMEA messages in the WGS84 datum. Be sure and check 
the user documentation to be sure.

It is possible to just view the information presented on the NMEA interface using 
a simple terminal program. If the terminal program can log the session then you 
can build a history of the entire session into a file. More sophisticated logging 
programs can filter the messages to only certain sentences or only collect sentences 
at prescribed intervals. Some computer programs that provide real time display and 
logging actually save the log in an ascii format that can be viewed with a text 
editor or used independently from the program that generated it.


NMEA input

Some units also support an NMEA input mode. While not too many programs support this mode 
it does provide a standardized way to update or add waypoint and route data. Note that 
there is no handshaking or commands in NMEA mode so you just send the data in the correct 
sentence and the unit will accept the data and add or overwrite the information in memory. 
If the data is not in the correct format it will simply be ignored. A carriage return/line 
feed sequence is required. If the waypoint name is the same you will overwrite existing 
data but no warning will be issued. The sentence construction is identical to what the 
unit downloads so you can, for example, capture a WPL sentence from one unit and then 
send that same sentence to another unit but be careful if the two units support waypoint 
names of different lengths since the receiving unit might truncate the name and overwrite 
a waypoint accidently. If you create a sentence from scratch you should create a correct 
checksum. Be sure you know and have set you unit to the correct datum. Many units support 
the input of WPL sentences and a few support RTE as well.

On NMEA input the receiver stores information based on interpreting the sentence itself. 
While some receivers accept standard NMEA input this can only be used to update a waypoint 
or similar task and not to send a command to the unit. Proprietary input sentences could 
be used to send commands. Since the Magellan upload and download maintenance protocol is 
based on NMEA sentences they support a modified WPL message that adds comments, altitude, 
and icon data.

Some marine units may accept input for alarms such as deep or shallow water based on the 
DPT sentence or MTW to read the water temperature. For example the Garmin Map76 supports 
DPT, MTW (temperature), and VHW (speed) input sentences. Other units may use NMEA input 
to provide initialization data via proprietary sentences, or to select which NMEA 
sentences to output.


Decode of selected position sentences


The most important NMEA sentences include the GGA which provides the current Fix data, 
the RMC which provides the minimum gps sentences information, and the GSA which provides 
the Satellite status data.

GGA - essential fix data which provide 3D location and accuracy data.

 $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47

Where:
     GGA          Global Positioning System Fix Data
     123519       Fix taken at 12:35:19 UTC
     4807.038,N   Latitude 48 deg 07.038' N
     01131.000,E  Longitude 11 deg 31.000' E
     1            Fix quality: 0 = invalid
                               1 = GPS fix (SPS)
                               2 = DGPS fix
                               3 = PPS fix
			       4 = Real Time Kinematic
			       5 = Float RTK
                               6 = estimated (dead reckoning) (2.3 feature)
			       7 = Manual input mode
			       8 = Simulation mode
     08           Number of satellites being tracked
     0.9          Horizontal dilution of position
     545.4,M      Altitude, Meters, above mean sea level
     46.9,M       Height of geoid (mean sea level) above WGS84
                      ellipsoid
     (empty field) time in seconds since last DGPS update
     (empty field) DGPS station ID number
     *47          the checksum data, always begins with *

If the height of geoid is missing then the altitude should be suspect. Some non-standard 
implementations report altitude with respect to the ellipsoid rather than geoid altitude. 
Some units do not report negative altitudes at all. This is the only sentence that 
reports altitude.


GSA - GPS DOP and active satellites. This sentence provides details
on the nature of the fix. It includes the numbers of the satellites
being used in the current solution and the DOP. DOP (dilution of 
precision) is an indication of the effect of satellite geometry on 
the accuracy of the fix. It is a unitless number where smaller is 
better. For 3D fixes using 4 satellites a 1.0 would be considered 
to be a perfect number, however for overdetermined solutions 
it is possible to see numbers below 1.0.

There are differences in the way the PRN's are presented which can 
effect the ability of some programs to display this data. For example, 
in the example shown below there are 5 satellites in the solution and 
the null fields are scattered indicating that the almanac would show 
satellites in the null positions that are not being used as part of 
this solution. Other receivers might output all of the satellites used 
at the beginning of the sentence with the null field all stacked up 
at the end. This difference accounts for some satellite display 
programs not always being able to display the satellites being tracked. 
Some units may show all satellites that have ephemeris data without 
regard to their use as part of the solution but this is non-standard.

  $GPGSA,A,3,04,05,,09,12,,,24,,,,,2.5,1.3,2.1*39

Where:
     GSA      Satellite status
     A        Auto selection of 2D or 3D fix (M = manual) 
     3        3D fix - values include: 1 = no fix
                                       2 = 2D fix
                                       3 = 3D fix
     04,05... PRNs of satellites used for fix (space for 12) 
     2.5      PDOP (dilution of precision) 
     1.3      Horizontal dilution of precision (HDOP) 
     2.1      Vertical dilution of precision (VDOP)
     *39      the checksum data, always begins with *



RMC - NMEA has its own version of essential gps pvt (position, velocity, time) 
data. It is called RMC, The Recommended Minimum, which will look similar to:

$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A

Where:
     RMC          Recommended Minimum sentence C
     123519       Fix taken at 12:35:19 UTC
     A            Status A=active or V=Void.
     4807.038,N   Latitude 48 deg 07.038' N
     01131.000,E  Longitude 11 deg 31.000' E
     022.4        Speed over the ground in knots
     084.4        Track angle in degrees True
     230394       Date - 23rd of March 1994
     003.1,W      Magnetic Variation
     *6A          The checksum data, always begins with *

Note that, as of the 2.3 release of NMEA, there is a new field in the RMC sentence 
at the end just prior to the checksum. For more information on this field see 
example below:

Example:  Garmin etrex summit (NMEA 2.3) output (note 4 decimal lat-long):

   $GPRMC,002454,A,3553.5295,N,13938.6570,E,0.0,43.1,180700,7.1,W,A*3F
   $GPGGA,002454,3553.5295,N,13938.6570,E,1,05,2.2,18.3,M,39.0,M,,*7F
   $GPGSA,A,3,01,04,07,16,20,,,,,,,,3.6,2.2,2.7*35

Compare to Garmin G12, with original NMEA spec (note 3 decimal lat-long):

   $GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A
   $GPGGA,183730,3907.356,N,12102.482,W,1,05,1.6,646.4,M,-24.1,M,,*75
   $GPGSA,A,3,02,,,07,,09,24,26,,,,,1.6,1.6,1.0*3D

Some observations as compared to the G-12:

    * Lat/Lon numbers have an extra digit.
    * This is NMEA 2.3 data as indicated by the extra A at the end of RMC, RMB and GLL.

-----------------------------------------------------------------------------------------''

NOTE:  The GPSSIM program will only output the sentences GPRMC, GPGSA, and GPGGA.

*/


/* The Arduino/Diavolino code assumes the Arduino programming environment (Processing) and standard
   libraries available as of Arduino development softrware version 0018 or later.
   
   The Windows code assumes a MinGW GCC compiler environment.  The Linux code (almost identical)
   assumes Ubuntu Linux 9.10 or later with a GCC compiler environment.  Both Windows and Linux code 
   assume certain libraries and headers are available at compilation time, which are part of
   a custom programming environment (GPE) prepared by the original author, Gary L. Flispart:  
      
         #include "gflib.h"
         #include "calensub.h"
         #include "obsolete.h"
         #include "gftermio.h"

   These libraries include general string parsing and time functions and an OS-portable serial port 
   interface.  Some of these functions have been adapted and incorporated in whole into the Arduino 
   specific code, but the external library references are retained for other OS versions.   
*/   

/* NOTE on precision in calculations -- GLF 12/04/2010:

   The Arduino Duemilanove (or Diavolino) code for GPSSIM is converted from the Windows 
   (or Linux) based code which uses GCC on an x86 platform.  That platform supports 
   double precision (64-bit) while the Arduino version of GCC has 32-bit "double" 
   equivalent to "float" type under Windows GCC.  

   This introduced a variety of side issues which took a while to diagnose and work around.
   The original constant VERYBIG was specified 1.0E99, but Arduino floats top out around 
   1.0E37.   The original linear regression function worked OK on Windows with LARGE x 
   values (seconds count in a year) because the 16+ decimal places available handled 
   values that large SQUARED without significant error.  However, the 7+ digits on
   the Arduino created infinity issues with the same x values.  This was avoided by 
   treating the x values RELATIVE to the beginning x value in a run, keeping the 
   SQUARED values MUCH smaller, allowing the original linear regression function to work.
   Also, the difference in precision in cos() and in calculations causes the vector
   angles to be different on the two systems, with the Arduino values showing variability
   within a few degrees while the Windows values are very stable.  This is NOT an error,
   just a FEATURE <grin> dependent on the available precision.
*/   

/* If the following is defined, avoid simulation of poor satellite conditions */
#define PERFECT_SAT_FIXES


#ifdef ARDUINO
#define BUFFLIMIT 28
#else
#define BUFFLIMIT 110
#endif 

#ifdef ARDUINO

/* -------- Arduino Version ------------ */

#include "WProgram.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* The following Arduino includes and definition USEFLASH allow use of flash ROM tables */
#include <avr/io.h>
#include <avr/pgmspace.h>

#define USEFLASH 

#else

/* -------- Windows or Linux version ------------ */

#include <math.h>
#include <stdlib.h>
#include "gflib.h"
#include "calensub.h"
#include "obsolete.h"
#include <time.h>

#include "gftermio.h"

#endif

/* These compile options set characteristics of satellite reception simulation */
#define STABLE_SAT_SECONDS     17 
#define DROPOUT_SAT_SECONDS   831 
#define DROPOUT_CYCLES        783

/* int value used to seed rand() function generator */
#define RAND_SEED_VALUE 27

/* select NMEA version 2.3 -- affects RMC and GGA sentences and lat-long resolution */
#define NMEA23 

/* set baud for GPS output -- note 4800 baud is NMEA standard, but many devices allow 9600 or more */
#define BAUD_RATE 57600

/* define DEBUG_OUTPUT if you want certain debugging information
   NOTE: in Windows/Linux version this overrides REALTIME below         */
  
/* #define DEBUG_OUTPUT */

/* define REALTIME if you want output groups to be emitted once per second  */
   
#define REALTIME    


/* define USE_RANDOM_VARY if you want to support random varying winds to make 
   flightlines more realistic */
   
/* #define USE_RANDOM_VARY */

/* define SHORT_TEST if you want to omit most of the data lines to save memory for early testing
   of port to Arduino -- if you turn off SHORT_TEST, you MUST turn on 
   USEFLASH */

/* #define SHORT_TEST */



#ifdef ARDUINO

/* define for Arduino only (Windows uses other headers) */
#define TRUE 1
#define FALSE 0

/* Common to all OS versions but implemented differently */
int portspec = 1;

#else

/* Specific to Windows/Linux only */
int half_duplex = 1;     /* if half_duplex != 0, dumb terminal will echo */
int portbaud = BAUD_RATE;

/* Common to all OS versions but implemented differently */
int portspec = 0;

#endif


#ifdef ARDUINO

/* --- Arduino version -- functions providing common support but in different ways for each OS  --- */

/* Size of floats and doubles on Arduino is 32 bits */
#define VERYBIG 1E+37

/* Arduino only -- Stack space debugging support */

/* This function places the current value of the heap and stack pointers in the
 * variables. You can call it from any place in your code and save the data for
 * outputting or displaying later. This allows you to check at different parts of
 * your program flow.
 * The stack pointer starts at the top of RAM and grows downwards. The heap pointer
 * starts just above the static variables etc. and grows upwards. SP should always
 * be larger than HP or you'll be in big trouble! The smaller the gap, the more
 * careful you need to be. Julian Gall 6-Feb-2009.
 */

uint8_t * heapptr, * stackptr;

void check_mem() {
  stackptr = (uint8_t *)malloc(4);          // use stackptr temporarily
  heapptr = stackptr;                     // save value of heap pointer
  free(stackptr);      // free up the memory again (sets stackptr to 0)
  stackptr =  (uint8_t *)(SP);           // save value of stack pointer
}

/* Arduino only -- include support available in other libraries under Windows/Linux */

void serial_putc(int kar)
  {
   if ((kar < 0x00) || (kar > 0x7F))
     {
      return;
     } 
   /* output here */   
   Serial.write(kar);
 }


void serial_puts(char strg[])
  {
   Serial.print(strg);
  }


void append_strg(int kar, char strg[], int size, int *len)
  {
   int i;

   if (*len >= size)
     {
      *len = size;
      strg[*len] = 0;
      return; 
     }  
 
   /* append new character to strg */    
   strg[*len] = kar;
   *len = *len + 1; 
   strg[*len] = 0;
  }


/* --------- converted functions from calensub.h -------------- */


static int jultab[13] = {0,0,31,59,90,120,151,181,212,243,273,304,334};


/* Fixed leap year function (7/9/1996): */
int leapyr(int iyr)

{
 if ((iyr < 0) || (iyr > 9999)) return FALSE;
 if ((iyr >= 00) && (iyr <= 99)) iyr = iyr + 1900;
 if (iyr % 100 == 0)
   {
    if (iyr % 400 == 0)
       return TRUE;
    return FALSE;
   }
 else
   {
    if (iyr % 4 == 0)
       return TRUE;
   }
 return FALSE;
}

void jdvert(int jdat, int jyr, int *imo, int *ida, int *newyr)

{
 int kdat;
 int n;
 int ldat;


 *newyr = jyr;
 if ((*newyr < 0) || (*newyr > 9999)) goto L_9000;
 if ((*newyr >= 00) && (*newyr <= 99)) *newyr = *newyr + 1900;
 kdat = jdat;

L_10:
 if (kdat > 0) goto L_20;
 *newyr = *newyr - 1;
 n = 365;
 if (leapyr(*newyr)) n = 366;
 kdat = kdat + n;
 goto L_10;

L_20:
 n = 365;
 if (leapyr(*newyr)) n = 366;
 if (kdat <= n) goto L_100;
 kdat = kdat - n;
 goto L_20;

L_100:
 ldat = kdat;
 if (kdat > 31) goto L_200;
 *imo = 1;
 *ida = kdat;
 goto L_9000;

L_200:
 if (ldat > 59) goto L_290;
 *imo = 2;
 *ida = kdat - 31;
 goto L_9000;

L_290:
 if (n == 366) ldat = ldat - 1;
 if (ldat > 59) goto L_300;
 *imo = 2;
 *ida = 29;
 goto L_9000;

L_300:
 if (ldat > 90) goto L_400;
 *imo = 3;
 *ida = ldat - 59;
 goto L_9000;

L_400:
 if (ldat > 120) goto L_500;
 *imo = 4;
 *ida = ldat - 90;
 goto L_9000;

L_500:
 if (ldat > 151) goto L_600;
 *imo = 5;
 *ida = ldat - 120;
 goto L_9000;

L_600:
 if (ldat > 181) goto L_700;
 *imo = 6;
 *ida = ldat - 151;
 goto L_9000;

L_700:
 if (ldat > 212) goto L_800;
 *imo = 7;
 *ida = ldat - 181;
 goto L_9000;

L_800:
 if (ldat > 243) goto L_900;
 *imo = 8;
 *ida = ldat - 212;
 goto L_9000;

L_900:
 if (ldat > 273) goto L_1000;
 *imo = 9;
 *ida = ldat - 243;
 goto L_9000;

L_1000:
 if (ldat > 304) goto L_1100;
 *imo = 10;
 *ida = ldat - 273;
 goto L_9000;

L_1100:
 if (ldat > 334) goto L_1200;
 *imo = 11;
 *ida = ldat - 304;
 goto L_9000;

L_1200:
 *imo = 12;
 *ida = ldat - 334;

L_9000:
 ;
}


int juldat(int iyr, int imo, int ida)

{
 int n;


 if ((imo < 1) || (imo > 12)) return 0;
 if ((ida < 1) || (ida > 31)) return 0;
 if ((iyr < 0) || (iyr > 9999)) return 0;
 if ((iyr >= 00) && (iyr <= 99)) iyr = iyr + 1900;
 n = 0;
 if (leapyr(iyr))
    n = 1;
 if (imo <= 2) n = 0;
 return (jultab[imo] + n + ida);
}

/* -------------------------------------------------------- */

/* test string length function */
int16_t length(char * strg)
  {
    int i;
    
    for (i=0; ; i++)
      {
        if (!strg[i])
          {
            return i;
          }
      }
    return 0;  
  }

/* -------------------------------------------------------- */

/* workaround for lack of printf() floats in standard Arduino software -- they 
   CAN be had in printf(), but must compile with an alternate library and
   current Arduino software does not OBVIOUSLY support alternate compile flags 
   (they're working on it, maybe) */

/* Arduino's inclusion of avr-libc supports a dtostrf() function, but it leaves trailing
   blanks when a left-justified number is shorter than the field -- this fixes that
*/

char *dtostrf_chop(double val, signed char wid, unsigned char prec, char *s)
  {
   int i;

   dtostrf(val,wid,prec,s);
  
   
   /* get rid of any trailing blanks */ 
   for (i=length(s)-1; i >= 0; i--)
     {
      if (s[i] != ' ')
        {
         break; 
        } 
      s[i] = 0;  
     }   
   
   return s;      
  }
   
/* -------------------------------------------------------- */




//typedef unsigned long time_t;

static unsigned long seconds_count = 0;
static unsigned long last_time;

int seconds_elapsed(void)
  {
   unsigned long this_time;
   unsigned long diff_time;
   int i = 0;
   
   this_time = millis();
   diff_time = this_time - last_time;
   while (diff_time >= 1000)
     {
      i++; 
      diff_time -= 1000;
     } 
   /* if at least one second has elapsed, resynchronize 
      seconds detection for next call to this function */  
   if (i)
     {
      last_time = this_time - diff_time; 

      /* keep track of seconds for unix-like time count */
      seconds_count += i;        
     }

   return i;  
  }


unsigned long time(unsigned long *dummy)
  {
   seconds_elapsed(); 
   return seconds_count;
  }


  
  
  /* ----------------------------------------------------------------- */


void wait_seconds(int secs)
  {
   int i;
   
   unsigned long time_previous;
   unsigned long time_current;
   
   time(&time_previous);
   time(&time_current);
   
   for (i=0; i<secs; i++)
     {
      /* wait until next observed change of second on real time clock */
      while (time_current == time_previous)
        {
         time(&time_current);
        } 
      time_previous = time_current;  
     }  
  }

void write_com(int port, int kar)
  {
   serial_putc(kar);
  }

void com_string(int port,char strg[])
  {
   Serial.print(strg);
  }  

void com_string_crlf(int port,char strg[])
  {
   com_string(port,strg);
   write_com(port,'\r');    
   write_com(port,'\n'); 
   /* delay to ensure transmission gap at least 1 byte long at 9600 baud */
   /* ms_delay(1); */   
  }  

char x_work[BUFFLIMIT];   

#else

/* --- Windows or Linux version -- functions providing common support but in different ways for each OS  --- */

/* Size of doubles in Windows/Linux is 64 bits */
#define VERYBIG 1E+99

void wait_seconds(int secs)
  {
   int i;
   
   time_t time_previous;
   time_t time_current;
   
   time(&time_previous);
   time(&time_current);
   
   for (i=0; i<secs; i++)
     {
      /* wait until next observed change of second on real time clock */
      while (time_current == time_previous)
        {
         time(&time_current);
        } 
      time_previous = time_current;  
     }  
  }


void early_exit_closecom(void)
  {
   close_com(portspec);  
  }
                     
  
void com_string(int port,char strg[])
  {
   int i;
   
   if (port)
     {
      for (i=0; strg[i]; i++)  
        {
         write_com(port,strg[i]);    
        }
     }
   else  /* display on console if no port specified */
     {
      printf("%s",strg); 	 
	 }
  }  

void com_string_crlf(int port,char strg[])
  {
   if (port)
     {   
      com_string(port,strg);
      write_com(port,'\r');    
      write_com(port,'\n'); 
     }
   else  /* display on console if no port specified */
     {
	  printf("%s\n",strg);
     }	 
  }  


char x_work[BUFFLIMIT];   


/* ---------------------------------------------------------------------------- */
/* workaround for lack of printf() floats in standard Arduino software -- they 
   CAN be had in printf(), but must compile with an alternate library and
   current Arduino software does not OBVIOUSLY support alternate compile flags 
   (they're working on it, maybe) */

/* Arduino's inclusion of avr-libc SHOULD support a dtostrf() function which fills
   the gap, but MinGW DOES NOT have this function -- the following allows testing of 
   the concept in MinGW programs */
   


void string_double(char strg[], double val, int places, int decimal)
  {
   char work_st[200];
   char fmt_st[40];
   
   strg[0] = 0;   
   if (places > 0)
     {
      if (places < 1)
        {
         return;
        }  
        
      if (decimal < 0)
        {
         decimal = 0;   
        }  
   
      if (decimal > 9)
        {
         decimal = 9;   
        }
       
      if (decimal > (places-2))
        {
         places = decimal + 2;   
        }
     }
   else
     {
      if (places > -1)
        {
         return;
        }  
        
      if (decimal < 0)
        {
         decimal = 0;   
        }  
   
      if (decimal > 9)
        {
         decimal = 9;   
        }
       
      if (decimal > ((-places)-2))
        {
         places = -(decimal + 2);   
        }
     }     
   
   sprintf(fmt_st,"%c%d.%d%s",'%',places,decimal,"lf");   
   sprintf(strg,fmt_st,val);
  } 


char *dtostrf(double val, signed char width, unsigned char prec, char *s)
  {
   /* minimal to test conversion code */
   /*   if (width < 0)
     {
      width = -width;
     }
      
   string_double_x(s, val, width, prec);      
   */

   string_double(s, val, width, prec);      
   return s;      
  }


/* -------------------------------------------------------- */

/* test string length function */
int length(char * strg)
  {
    int i;
    
    for (i=0; ; i++)
      {
        if (!strg[i])
          {
            return i;
          }
      }
    return 0;  
  }

/* -------------------------------------------------------- */

/* Arduino's inclusion of avr-libc supports a dtostrf() function, but it leaves trailing
   blanks when a left-justified number is shorter than the field -- this fixes that
*/

char *dtostrf_chop(double val, signed char wid, unsigned char prec, char *s)
  {
   int i;
   
   dtostrf(val,wid,prec,s);
  
   
   /* get rid of any trailing blanks */ 
   for (i=length(s)-1; i >= 0; i--)
     {
      if (s[i] != ' ')
        {
         break; 
        } 
      s[i] = 0;  
     }   
   
   return s;      
  }

/* Windows/Linux only -- used to process command line options */
char inname[65];
char outname[65];
FILE *infile;
FILE *outfile;
int chopcol;
long endline;
long numin;

#endif


/* ---  The code after this is largely equivalent in all OS versions ---- */

/* the following functions provide safer equivalents to certain math functions 
   in the C library, but also convert them implicitly to use degrees instead 
   of radians 
*/
  
#define RAD_TO_DEG  57.29577951
#define DEG_TO_RAD  0.017453292
#define TWO_PI_VAL  6.283185307  
#define METERS_PER_DEG_LONG_EQUATOR 111325.0  
#define METERS_PER_DEG_LAT   111325.0  
#define METERS_PER_SEC_TO_KNOTS 1.94384449
#define Z_ATTENUATE 0.15
  
  
double cos_safe(double angle_deg)
  {
   double angle_radian;    
   
   angle_radian = angle_deg * DEG_TO_RAD;
   while (angle_radian < 0.000000001)
     {
      angle_radian += TWO_PI_VAL;              
     }
   while (angle_radian > TWO_PI_VAL)
     {
      angle_radian -= TWO_PI_VAL;              
     }
   if (angle_radian < 0.000000001)
     {
      return 1.0;              
     }               
   return cos(angle_radian);                 
  }
  
double acos_safe(double cos_deg)
  {
   double angle_radian;    
   
   if (cos_deg < -0.9999999999)
     {
      return 180.0;          
     }

   if (cos_deg > 0.9999999999)
     {
      return 0.0;          
     }

   angle_radian = acos(cos_deg);     
   return  angle_radian * RAD_TO_DEG;
  }
  
  
  
double sqrt_safe(double val)
  {
   if (val < 0.000000001)
     {
      return 0.0;              
     }        
   return sqrt(val);         
  }     
  
  

/* The following functions support random excursions from simple linear 
   regression to enhance the realism of the simulation.  It is assumed that 
   wind motion will vary from average by a roughly normal function in which 
   the maximum excursion will be about 35 mph (gusts) but much more frequently 
   excursions are only 0-3 mph.  These random excursions are applied to x and y, 
   and to a reduced extent, z (it is assumed that strong gusting updrafts and 
   downdrafts are more rare).  The numbers in rnd_offset_deg[] are consistent with a 
   maximum excursion of about .01 miles per second, expressed instead as +-degrees.
   The variability coefficient 0-100 is multiplied by the value selected from the 
   table to determine the random variation from average values calculated by linear 
   interpolation of waypoints.
*/   

#ifdef USE_RANDOM_VARY
double rnd_offset_deg[100] =   /* use by generating a linearly random index 0-99 */
  {
   -0.0000014, 
   -0.0000013, 
   -0.0000012, 
   -0.0000011, 
   -0.0000010, -0.0000010, 
   -0.0000009, -0.0000009,  
   -0.0000008, -0.0000008, 
   -0.0000007, -0.0000007, -0.0000007, 
   -0.0000006, -0.0000006, -0.0000006, 
   -0.0000005, -0.0000005, -0.0000005, -0.0000005, 
   -0.0000004, -0.0000004, -0.0000004, -0.0000004, -0.0000004, 
   -0.0000003, -0.0000003, -0.0000003, -0.0000003, -0.0000003, -0.0000003, 
   -0.0000002, -0.0000002, -0.0000002, -0.0000002, -0.0000002, -0.0000002, -0.0000002,  
   -0.0000001, -0.0000001, -0.0000001, -0.0000001, -0.0000001, -0.0000001, -0.0000001, -0.0000001, 
    0.0000000,  0.0000000,  0.0000000,  0.0000000,  0.0000000,  0.0000000,  0.0000000,  0.0000000, 
    0.0000001,  0.0000001,  0.0000001,  0.0000001,  0.0000001,  0.0000001,  0.0000001,  0.0000001,
    0.0000002,  0.0000002,  0.0000002,  0.0000002,  0.0000002,  0.0000002,  0.0000002,  
    0.0000003,  0.0000003,  0.0000003,  0.0000003,  0.0000003,  0.0000003, 
    0.0000004,  0.0000004,  0.0000004,  0.0000004,  0.0000004,  
    0.0000005,  0.0000005,  0.0000005,  0.0000005,  
    0.0000006,  0.0000006,  0.0000006,  
    0.0000007,  0.0000007,  0.0000007, 
    0.0000008,  0.0000008, 
    0.0000009,  0.0000009,   
    0.0000010,  0.0000010, 
    0.0000011, 
    0.0000012, 
    0.0000013,
    0.0000014
  };
#endif



/* The Linux glibc rand() function returns a 31-bit value.  Arduino and MinGW (Windows) return 
   a 16-bit value.  The following function "dumbs down" the range of rand() under Linux glibc, 
   so this function emulates variablility of MinGW and Arduino 16-bit randomization functions. 
   The random_index() function uses and returns an unsigned int which is effectively a short int. 
   A call to srand(() later in this program might otherwise be affected by this issue, but the 
   call transparently accommodates the seed value provided (16 bit if Arduino, 32-bit MinGW 
   or Linux) regardless of OS. */

#if defined(__MINGW32__) || defined(ARDUINO)        /* Windows or Arduino */

unsigned int random_index(unsigned int range)  /* returns index 0 to range-1 */
  {
   long r;
   
   r = rand();   /* convert to long to preserve bits during multiply */
   r = (r * (long)range) / RAND_MAX;
   
   return (unsigned int)r;
  }

#else                                               /* assume Linux */

unsigned int random_index(unsigned int range)  /* returns index 0 to range-1 */
  {
   unsigned long r;
   
   /* Linux glibc rand() is 31 bits wide -- reduce to 16 bits to match MinGW and Arduino */

   r = rand() >> 16;                       /* divide by 64K -- equivalent to rand() on 16-bit system */ 
   r = (r * (unsigned long)range) >> 15;   /* first multiply evenly distributed 16-bit value by range,
                                              then divide by 32768 (max range of 16-bit rand()) */
   return (unsigned int)r;
  }

#endif



void random_vary_pos(int vary_spec, double *x, double *y, double *z)
  {
   /* assume x, y, and z are preloaded with average values: 
             x in degrees longitude
             y in degrees latitude 
             z in meters altitude 
   */    
   
   double offset;
   double lat_adj;
   double unit_adj;
   
   if ((vary_spec < 0) || (vary_spec > 100))
     {
      return;   /* no variation applied */           
     }
     
   /* variation in y -- latitude -- is straightforward */
#ifdef USE_RANDOM_VARY   
   offset = (rnd_offset_deg[random_index(100)] * vary_spec);
#else
   offset = 0;
#endif      

   *y = (*y) + offset;
   

   /* variation in x -- longitude is slightly harder to allow for compression 
      of longitude degrees toward north */
   /* force away from poles (infinity problem) */      
/*   
   if ((*y) > 89.999999)
     {
      *y = 89.999999;   
     }          
   if ((*y) < -89.999999)
     {
      *y = -89.999999;
     }          
*/
/*   lat_adj = 1.000 / cos_safe(*y);    */
   lat_adj = 1.000;

#ifdef USE_RANDOM_VARY   
   offset = (rnd_offset_deg[random_index(100)] * vary_spec);
#else
   offset = 0;
#endif
   
   *x = ((*x) * lat_adj) + offset;
   
   /* variation in z -- altitude -- is straightforward but in different units and 
         assumed to be a reduced effect from that in x and y */

#ifdef USE_RANDOM_VARY   
   offset = rnd_offset_deg[random_index(100)] * vary_spec 
                                    * METERS_PER_DEG_LAT * Z_ATTENUATE;
#else
   offset = 0;
#endif
   
   *z = (*z) + offset;
  }
  


double linregress(int op, double x, double y)

  {
   static double sumx;
   static double sumy;
   static double sumxy;
   static double sumxx;
   static double sumyy;
   static int n;
   static int calc;
   static double coef1;
   static double coef2;
   static double corr;
   static double s1;
   static double s2;
   static double covar;

   static double temp;

   if (op < 0)
     {
      sumx = 0.0;
      sumy = 0.0;
      sumxy = 0.0;
      sumxx = 0.0;
      sumyy = 0.0;
      covar = 0;
      corr = 0.0;
      s1 = 0.0;
      s2 = 0.0;
      n = 0;
      calc = 0;
      return 0.0;
     }
   if (op == 0)
     {
      n++;
      sumx = sumx +  x;
      sumy = sumy +  y;
      sumxy = sumxy +  x * y;
      sumxx = sumxx +  x * x;
      sumyy = sumyy +  y * y;
      covar = 0.0;
      corr = 0.0;
      calc = 0;
      s1 = 0.0;
      s2 = 0.0;

      return (double)n;
     }

   if (!calc)  /* save time by calculating summaries only once */
     {
      temp = ((double)n * sumxx) - (sumx * sumx);

      if ((temp < -0.000000001) || (temp > 0.000000001))
        {
         coef2 = +((double)n * sumxy - sumx * sumy) / temp;
         coef1 = +(sumy - coef2 * sumx) / (double)n;
        }
      else
        {
         coef2 = VERYBIG;
         coef1 = 0.0;
        }

      temp = (sumxx / (double)n) - ((sumx / (double)n) * (sumx / (double)n));
      if (temp > 0.000000001)
        {
         s1 = sqrt(temp);
        }
      else
        {
         s1 = 0.0;
        }

      temp = (sumyy / (double)n) - ((sumy / (double)n) * (sumy / (double)n));
      if (temp > 0.000000001)
        {
         s2 = sqrt(temp);
        }
      else
        {
         s2 = 0.0;
        }

      covar = +(sumxy / (double)n) - (+(sumx * sumy) / +((double)n * (double)n));
      if ((s1 > 0.000000001) && (s2 > 0.000000001))
        {
         corr = covar / +(s1 * s2);
        }
      else
        {
         corr = 0.0;
        }

      calc = 1;
     }

   if (op == 1)
     {
      return coef1;
     }
   if (op == 2)
     {
      return coef2;
     }
   if (op == 3)
     {
      return s1;
     }
   if (op == 4)
     {
      return s2;
     }
   if (op == 5)
     {
      return corr;
     }
     
   return (double)n;
  }

  
  
/* ---  The code after this is largely equivalent in all OS versions (some differences) ---- */


int flt_randomseed = RAND_SEED_VALUE;

int flt_realtime = FALSE;

#ifdef ARDUINO
unsigned long flt_time_previous;
unsigned long flt_time_current;
#else
time_t flt_time_previous;
time_t flt_time_current;
#endif

int flt_fixtype; 
int flt_datapos = 0;

int flt_var = 0;
int flt_noise = 0;
int flt_error = 0;

int flt_firstyear = 0;
long flt_secs_firstyear = 0L;
int flt_firstpass = TRUE;

long flt_last_sec;
long flt_next_sec;

long flt_last_date;
long flt_next_date;
long flt_last_time;
long flt_next_time;

double flt_last_lat = 0.0;
double flt_last_long = 0.0;
double flt_last_alt = 0.0;
double flt_next_lat = 0.0;
double flt_next_long = 0.0;
double flt_next_alt = 0.0;

double flt_x_m = 0.0;
double flt_x_b = 0.0;

double flt_y_m = 0.0;
double flt_y_b = 0.0;

double flt_z_m = 0.0;
double flt_z_b = 0.0;

double flt_climb_rate = 0.0;
double flt_climb_user_input = 0.0;
double flt_climb_offset = 0.0;
char flt_indicate_climb[22] = "";


/* these counters and predefined macros (above) set up cycle timing for simulated 
   satellite visibility changes and simulated GPS reception dropouts */
      
int flt_stablect;
int flt_dropoutct;
int flt_cyclect;


int checksum(char strg[])
  {
   int i;
   unsigned char csum = 0;
   
   for (i=0; strg[i]; i++)
     {
      csum ^= strg[i];       
     }               
     
   return (int)csum;  
  }
  

/* convert coordinate to style used by GPS:  DEGMM.MMM   from fractional degree:  DEG.DDDDDD */
double gps_coord(double deg_coord)
  {
   int gps;
   int deg;
   double fracdeg; 
   double sgn = 1.0;
   
   /* need to work with absolute value */
   if (deg_coord < 0.000)
     {
      sgn = -1.0;
      deg_coord = -deg_coord;           
     }
   deg = (int)deg_coord;

   fracdeg = deg;
   fracdeg = deg_coord - (double)(deg);
   fracdeg = ((double)deg * 100.0) + (fracdeg * 60.0);
   
   /* restore sign */
   fracdeg *= sgn;

   return fracdeg;
  }

/* convert coordinate to fractional degree:  DEG.DDDDDD  from style used by GPS:  DEGMM.MMM */
double deg_coord(double gps_coord) 
  {
   int deg;
   int sub;
   double fracmin, fracdeg; 
   double sgn = 1.0;
   
   /* need to work with absolute value */
   if (gps_coord < 0.000)
     {
      sgn = -1.0;
      gps_coord = -gps_coord;           
     }
   deg = gps_coord;
   deg = deg / 100;
   sub = deg * 100;
   fracdeg = deg;
   fracmin = gps_coord - (double)(sub);
   fracdeg += (fracmin / 60.0);
   
   /* restore sign */
   fracdeg *= sgn;

   return fracdeg;
  }


void open_script(void)
  {
   flt_datapos = 0;

   /* DEFAULT randomized wind variation = 4 for stable realistic winds */
   flt_var = 0;
   flt_noise = 0;
   flt_error = 0;
   
   srand(flt_randomseed);
   
   flt_firstpass = TRUE;
   flt_firstyear = 0;
   flt_secs_firstyear = 0L;
   
   flt_fixtype = 1;
   
   flt_last_sec = 0;
   flt_next_sec = 0;

   flt_last_date = 0;
   flt_next_date = 0;

   flt_last_time = 0;
   flt_next_time = 0;

   flt_last_lat = 0.0;
   flt_last_long = 0.0;
   flt_last_alt = 0.0;

   flt_next_lat = 0.0;
   flt_next_long = 0.0;
   flt_next_alt = 0.0;

   flt_x_m = 0.0;
   flt_x_b = 0.0;
   flt_y_m = 0.0;
   flt_y_b = 0.0;
   flt_z_m = 0.0;
   flt_z_b = 0.0;

   flt_climb_rate = 0.0;
   flt_climb_user_input = 0.0;
   flt_climb_offset = 0.0;
   strcpy(flt_indicate_climb,"          |          ");

   flt_stablect = 0;        
   flt_dropoutct = 0;
   flt_cyclect = 1;      /* when this returns to 0, a dropout will begin */
  }


  
  
/*  
   FLIGHT SIM data condensed from script_snox4.txt:

      Simulates the actual balloon flight of Spirit of Knoxville IV.
*/

/* data is split into two sections -- long int date and time, and double x,y,z data */


/* ARDUINO ISSUE if space is at a premium, try moving these tables to flash */
/* GLF -- note the use of the PROGMEM attribute in the following data tables */ 

#ifdef USEFLASH
 unsigned long date_time[] PROGMEM =   /* NOTE:  MUST avoid leading zeros -- they make OCTAL constants */
#else 
 unsigned long date_time[] =   /* NOTE:  MUST avoid leading zeros -- they make OCTAL constants */
#endif 
  {  
   100308,  14000, 
   100308,  14100, 
   100308,  15100, 
   100308,  20100, 
   100308,  21100, 
   100308,  22100, 
   100308,  23100, 


   100308,  24100, 
   100308,  25100, 
   100308,  30100, 
   100308,  31100, 
   100308,  32100, 
   100308,  33100, 
   100308,  34100, 
   100308,  35100, 
   100308,  50100, 
   100308,  60100, 


   100308,  62100, 
   100308,  64100, 
   100308,  65100, 
   100308,  70100, 

   100308,  82100, 
   100308,  84100,   /* simulate precipitous drop and rise again */
   100308,  85600, 

   100308, 115100, 
   100308, 121100, 
   100308, 125100, 


#ifndef SHORT_TEST   
   100308, 130100, 
   100308, 150100, 
   100308, 151100, 
   100308, 152100, 
   100308, 154100, 
   100308, 155100, 
   100308, 160100, 
   100308, 162100, 
   100308, 164100, 
   100308, 173100, 


   100308, 184100, 
   100308, 185100, 
   100308, 190100, 
   100308, 191100, 
   100308, 192100, 
   100308, 195100, 
   100308, 200100, 
   100308, 202100, 
   100308, 203100, 
   100308, 204100, 
   100308, 205100, 
   100308, 210100, 
   100308, 211100, 
   100308, 212100, 
   100308, 213100, 
   100308, 214100, 
   100308, 215100, 
   100308, 220100, 
   100308, 221100, 

/* this is two days of REALLY stupid straight-line travel */

   120308, 222100, 
   120308, 223100, 
   120308, 230100, 
   120308, 233100, 
   120308, 234100, 
   130308,  13100, 
   130308,  30100, 
   130308,  75100, 
   130308,  81100, 
   130308,  82100, 
   130308,  85100, 
   130308,  92100, 
   130308, 100100, 
   130308, 105100, 
   130308, 104100, 
   130308, 105100, 
   130308, 111100, 
   130308, 114100, 
   130308, 115100, 
   130308, 123100, 
   130308, 125100, 
   130308, 143100, 
   130308, 144100, 
   130308, 145100, 
   130308, 150100, 
   130308, 152100, 
   130308, 154100, 
   130308, 155100, 
   130308, 160100, 
   130308, 161100, 
   130308, 162100, 
   130308, 163100, 
   130308, 164100, 
   130308, 165100, 
   130308, 170100, 
   130308, 171100, 
   130308, 172100, 
   130308, 173100, 
   130308, 174100, 
   130308, 175100, 
   130308, 180100, 
   130308, 181100, 
   130308, 182100, 
   130308, 183100,

#endif   
   130308, 185100,        /* including this endpoint makes simulation last 1 1/2 days */
    
   0,0                    /* 0 date signals end of list */
  };


/* ARDUINO ISSUE if space is at a premium, try moving these tables to flash */

#ifdef USEFLASH
 double lat_long_alt[] PROGMEM =  
#else 
 double lat_long_alt[]  = 
#endif 
  {
   3557.749, -8352.413,   256.0,
   3557.748, -8352.413,   257.0,
   3557.037, -8351.164,  2228.0,
   3555.776, -8344.359,  4775.0,
   3554.307, -8329.445,  7715.0,
   3552.607, -8309.613, 10975.0,
   3553.731, -8251.674, 12037.0,


   3554.307, -8233.967, 11986.0,
   3555.005, -8216.677, 12106.0,
   3556.374, -8159.333, 12063.0,
   3557.622, -8141.193, 12018.0,
   3559.319, -8122.504, 12065.0,
   3600.990, -8103.685, 12024.0,
   3603.005, -8044.601, 11948.0,
   3605.354, -8025.009, 11831.0,
   3607.684, -8005.171, 12008.0,
   3618.395, -7803.240, 11870.0,


   3618.285, -7803.240, 11870.0,
   3617.313, -7637.609, 11596.0,
   3617.164, -7615.666, 11625.0,
   3617.207, -7531.655, 11623.0,

   3618.516, -7509.980, 11677.0,
   3619.423, -7448.194,  9700.0,    /* simuate a precipitous drop and rise again */
   3629.617, -6535.473, 11797.0,

   3629.535, -6514.150, 11852.0,
   3629.535, -6514.150, 11852.0,
   3629.542, -6328.238, 11968.0,


#ifndef SHORT_TEST
   3636.888, -6027.288, 11692.0,
   3640.550, -5902.053, 11593.0,
   3643.977, -5820.178, 11674.0,
   3647.010, -5738.898, 11763.0,
   3648.425, -5718.412, 11671.0,
   3650.148, -5657.758, 11707.0,
   3652.206, -5637.437, 11651.0,
   3659.547, -5637.437, 11588.0,
   3707.229, -5438.544, 11694.0,
   3709.963, -5419.361, 11683.0,


   3724.284, -5245.042, 11686.0,
   3740.409, -5113.898, 11648.0,
   3743.683, -5055.783, 11623.0,
   3746.429, -5037.692, 11588.0,
   3755.364, -5001.687, 11578.0,
   3758.175, -4926.909, 11547.0,
   3801.268, -4909.834, 11568.0,
   3807.986, -4835.266, 11514.0,
   3811.771, -4818.194, 11519.0,
   3815.822, -4800.680, 11538.0,
   3820.182, -4742.896, 11532.0,
   3824.635, -4724.853, 11480.0,
   3829.153, -4707.358, 11445.0,
   3833.352, -4650.688, 11286.0,
   3837.612, -4634.543, 11026.0,
   3841.725, -4615.723, 10878.0,
   3846.768, -4558.419, 10647.0,
   3851.191, -4540.809, 10478.0,
   3855.561, -4522.616, 10250.0,

/* this is two days of REALLY stupid straight-line travel */

   3901.518, -2505.665,  9899.0,
   3908.383, -2449.485,  9735.0,
   3922.068, -2418.539,  9519.0,
   3947.588, -2318.151,  8923.0,
   3953.711, -2303.819,  8818.0,
   3953.711, -2303.819,  8818.0,
   4243.240, -1846.382,  6209.0,
   4642.322, -1056.476,  5015.0,
   4648.292, -1043.007,  4861.0,
   4658.610, -1016.453,  4317.0,
   4702.195, -1004.683,  4088.0,
   4710.650, -0924.292,  2899.0,
   4715.990, -0849.683,  2424.0,
   4718.671, -0823.219,  2873.0,
   4718.671, -0823.219,  2873.0,
   4718.671, -0823.219,  2873.0,
   4721.556, -0807.596,  3451.0,
   4721.556, -0807.596,  3451.0,
   4742.030, -0714.939,  5184.0,
   4802.635, -0620.746,  5481.0,
   4808.579, -0606.182,  5607.0,
   4825.853, -0521.920,  5477.0,
   4857.995, -0349.035,  3754.0,
   4900.803, -0339.791,  4084.0,
   4904.021, -0328.273,  4183.0,
   4911.686, -0301.157,  4617.0,
   4915.137, -0246.620,  4859.0,
   4924.107, -0214.710,  5239.0,
   4929.784, -0158.483,  5518.0,
   4935.310, -0142.067,  5699.0,
   4940.378, -0124.429,  5845.0,
   4945.135, -0106.151,  5936.0,
   4949.440, -0047.019,  6133.0,
   4953.508, -0026.744,  6226.0,
   4957.067, -0006.011,  6351.0,
   5000.586,  0005.161,  6334.0,
   5004.423,  0025.422,  6218.0,
   5008.820,  0046.502,  5830.0,
   5012.482,  0108.364,  5300.0,
   5015.439,  0131.249,  4646.0,
   5017.350,  0146.895,  3960.0,
   5017.875,  0156.171,  3183.0,
   5018.451,  0205.833,  2554.0,
   5018.443,  0218.144,  2026.0,

#endif
   5019.496,  0240.950,  1495.0,     /* including this endpoint makes simulation last 1 1/2 days */

   -1.0, -1.0, -1.0                  /* dummies for end of list */  
  };



void close_script(void)
  {
  }


long secs_to_date(long secs)
  {
   long ljd;
   int jd; 
   int yr; 
   int mo,day,newyr;
   char work[BUFFLIMIT];
     
   /* the following conversion is only valid within a 2-yr span */  
   yr = flt_firstyear;
   if (secs > flt_secs_firstyear)
     {
      secs -= flt_secs_firstyear;  
      yr++;    
     }         

   ljd = (secs / 86400L) + 1L;
   jd = (int)ljd;
   jdvert(jd,yr,&mo,&day,&newyr);
   if ((newyr >= 1950) && (newyr <= 1999))
     {
      newyr -= 1900;        
     }
   if ((newyr >= 2000) && (newyr <= 2049))
     {
      newyr -= 2000;        
     }
   if ((newyr < 0) || (newyr > 99))
     {
      return 0L;        
     }  
     
   sprintf(work,"%02d%02d%02d",day,mo,newyr);  

   return atol(work);
  }

long secs_to_time(long secs)
  {
   long ljd;
   int jd; 
   int yr; 
   int mo,day,newyr;
   char work[BUFFLIMIT];
   long dropout;
   int hr,min,sc;  
   long minsec;
     
   /* the following conversion is only valid within a 2-yr span */  
   yr = flt_firstyear;
   if (secs > flt_secs_firstyear)
     {
      secs -= flt_secs_firstyear;  
      yr++;    
     }         
   ljd = (secs / 86400L) + 1L;
   jd = (int)ljd;
   jdvert(jd,yr,&mo,&day,&newyr);
   if ((newyr >= 1950) && (newyr <= 1999))
     {
      newyr -= 1900;        
     }
   if ((newyr >= 2000) && (newyr <= 2049))
     {
      newyr -= 2000;        
     }
   if ((newyr < 0) || (newyr > 99))
     {
      return 0L;        
     }  
     

   /* now that a valid date is found, drop out all the seconds in the year 
      up to that julian date */  
   dropout = (ljd - 1L) * 86400L;
      
   secs -= dropout;
   
   /* now secs = seconds in this time of day */   
   hr = (int)(secs / 3600L);
   minsec = (secs - (long)hr * 3600L);
   min = (int)(minsec / 60L);
   sc =  (int)(minsec - (long)min * 60L);

   sprintf(work,"%02d%02d%02d",hr,min,sc);  
   return atol(work);
  }

long time_secs(long time)
  {
   char work[BUFFLIMIT];
   char work2[3];
   int thr, tmin, tsec;
   long secs_for_time = 0L;
   

   if ((time < 0L) || (time > 235959))
     {
      return 0L;       
     }
       
   /* use strings to break apart pieces of time */                   
   sprintf(work,"%06ld",time);
   
   strncpy(work2,work,2);
   work2[2] = 0;
   thr = atoi(work2);
   
   strncpy(work2,work+2,2);
   work2[2] = 0;
   tmin = atoi(work2);
   
   strncpy(work2,work+4,2);
   work2[2] = 0;
   tsec = atoi(work2);
   
   if ((thr < 0) || (thr > 23))
     {
      return 0L;
     }
   
   if ((tmin < 0) || (tmin > 59))
     {
      return 0L;
     }
   
   if ((tsec < 0) || (tsec > 59))
     {
      return 0L;
     }
   
   /* at this point time is considered valid -- convert to seconds offset */
   secs_for_time = (long)tsec;
   secs_for_time += ((long)tmin) * 60L;
   secs_for_time += ((long)thr) * 3600L;
   
   return secs_for_time;                   
  }


long date_secs(long date)
  {
   char work[BUFFLIMIT];
   char work2[3];
   int tday, tmo, tyr;
   int jd;
   
   long secs_for_date = 0L;
                      
   if ((date < 010100L) || (date > 311299))
     {
      return 0L;       
     }

   /* use strings to break apart pieces of date */                   
   sprintf(work,"%06ld",date);
   
   strncpy(work2,work,2);
   work2[2] = 0;
   tday = atoi(work2);
   
   strncpy(work2,work+2,2);
   work2[2] = 0;
   tmo = atoi(work2);
   
   strncpy(work2,work+4,2);
   work2[2] = 0;
   tyr = atoi(work2);
   
   /* convert 2-digit to 4-digit date temporarily */
   if ((tyr >= 0) && (tyr <= 49))
     {
      tyr += 2000;      
     }
   else
     {
      tyr += 1900;      
     }  
   
   if (flt_firstyear == 0)
     {
      flt_firstyear = tyr;  /* note this now MUST be between 1950 and 2049 */            
      if (leapyr(tyr))
        {
         flt_secs_firstyear = 366L * 24L * 60L * 60L;
        }
      else
        {
         flt_secs_firstyear = 365L * 24L * 60L * 60L;
        }  
     }
   
   jd = juldat(tyr,tmo,tday);
   
   if (jd == 0)
     {
      return 0L;    
     }

   /* at this point date is considered valid -- convert to seconds offset */
   
   /* number of seconds since Jan 0 this year */
   secs_for_date = (long)(jd - 1)* 86400L;

   /* allow for days since first year encountered if not same --  the following 
      method is only valid for a 2-yr span (OK for a balloon flight) */
   if (tyr > flt_firstyear)
     {
      secs_for_date += flt_secs_firstyear;
     }   

/*   secs_for_date += ldifda(flt_firstyear,1,1,tyr,1,1); */

   return secs_for_date;                   
  }
  
  


int track_calc(double delta_x_deg, double delta_y_deg, double delta_t_secs, 
               double lat_deg, 
               double *knots, double *trackangle)

  {
   double delta_x_meters;              
   double delta_y_meters;              
   double sin_track;
   double cos_track;
   
   double mag;

   /* Note x is in longitude degrees and y is in latitude degrees */
                 
   /* meters per second (toward east) at given latitude */
   delta_x_meters = delta_x_deg * cos_safe(lat_deg) * METERS_PER_DEG_LONG_EQUATOR; 
   
   /* meters per second (toward north) */             
   delta_y_meters = delta_y_deg * METERS_PER_DEG_LAT; 
                 
   /* distance formula to calculate vecor magnitude and direction */              
   mag = sqrt_safe(delta_x_meters * delta_x_meters  +  
                          delta_y_meters * delta_y_meters);
                          
   if (mag > 0.000000001)
     {
      /* Note: since 0 degrees points right and 90 deg up for math funcs, but
               0 deg is North and 90 deg east for tracking, switch sin and cos
               orientation from math-normal in calculations.
      */              
      cos_track = delta_y_meters / mag;                
      sin_track = delta_x_meters / mag;                
     }    
   else
     {
      /* no magnitude, therefore no direction either -- signal this with a FALSE return
         value --  Program can decide whether to carry forward a prior direction.
      */    
      *knots = 0.0;
      *trackangle = 0.0;
      return FALSE;
     }              


   /* track angle is normalized to true north,   <= trackangle <= 360, east is 90 */
   if (delta_x_meters > 0.000000001)
     {
      /* we're working the right (east) side of the circle */                
      *trackangle = acos_safe(cos_track);           
     }                
   else
     {
      /* we're working the left (west) side of the circle */                
      *trackangle = 360.0 - (acos_safe(cos_track));           
     }                
   
   /* since magnitude is in total meters and delta_t_sec is in seconds, 
      calculate meters per second and convert to knots */
   *knots =  (mag * METERS_PER_SEC_TO_KNOTS) / delta_t_secs;
   return TRUE;          
  }  





void interp_setup(void)

  {
   double x = 0.0;
   double y = 0.0;  
   
   /* reset linear regression algorithm */
   linregress(-1,x,y);
   
   /* put in endpoints for x with respect to t */
   linregress(0,(double)(flt_last_sec-flt_last_sec),flt_last_long);
   linregress(0,(double)(flt_next_sec-flt_last_sec),flt_next_long);
   flt_x_m = linregress(2,x,y);
   flt_x_b = linregress(1,x,y);
 
   /* reset linear regression algorithm */
   linregress(-1,x,y);
   
   /* put in endpoints for y with respect to t */
   linregress(0,(double)(flt_last_sec-flt_last_sec),flt_last_lat);
   linregress(0,(double)(flt_next_sec-flt_last_sec),flt_next_lat);
   flt_y_m = linregress(2,x,y);
   flt_y_b = linregress(1,x,y);
 
   /* reset linear regression algorithm */
   linregress(-1,x,y);
   
   /* put in endpoints for z with respect to t */
   linregress(0,(double)(flt_last_sec-flt_last_sec),flt_last_alt);
   linregress(0,(double)(flt_next_sec-flt_last_sec),flt_next_alt);
   flt_z_m = linregress(2,x,y);
   flt_z_b = linregress(1,x,y);
  }


char st_normlat[BUFFLIMIT], st_normlong[BUFFLIMIT], st_hdilpos[BUFFLIMIT], st_vdilpos[BUFFLIMIT], st_pdilpos[BUFFLIMIT],
     st_z[BUFFLIMIT], st_geoid_height[BUFFLIMIT],
     st_knots[BUFFLIMIT], st_track_angle[BUFFLIMIT], st_norm_magvar[BUFFLIMIT],
     st_work[BUFFLIMIT];
   
   
/* global variables to track satellites by ID */
int  totalsats = 0;
char satarray[12][3];    /* sat IDs "" if none, "01" - "12" -- must be cleared and 
                            built by functions below */  
                            
void clear_satellites(void)
  {
   int i;
  
   for (i=0; i<12; i++)
     {
      satarray[i][0] = 0;
     } 
   totalsats = 0;  
   flt_fixtype = 1;
  }
 
 
int clear_dilutions(double *hdpos, double *vdpos, double *pdpos)
  {
   *hdpos = 9.9; 
   *vdpos = 9.9; 
   *pdpos = 9.9; 
  }


int sim_satellites(int forcenum, double *hdpos, double *vdpos, double *pdpos)
  {
   /* sort-of-randomly select a list of satellites visible */  
   int randval;
   int numsats;
   
   /* pick a number of satellites 3-6 -- about one per fifty set to 2 for simulated no fix */ 
   /* also create a matching set of plausible satellite IDs */
   
   /* NOTE:  assumes totalsats was set and retained from prior call -- both totalsats an numsats
             MUST be 0 to 6 at all times */
   
   if ((forcenum > 0) && (forcenum <= 6))
     {
      numsats = forcenum; 
     }  

   randval = random_index(50);   /* random 0 to 49 */ 
   numsats = 4;                 /* stays 2 if randval == 0 */
   flt_fixtype = 3;

   if (randval == 0)
     {
      #ifndef PERFECT_SAT_FIXES
         numsats = 2; 
         flt_fixtype = 1;
      #endif      /* otherwise remains 4 */
     }
   if ((randval > 0) && (randval <= 6))
     {
      numsats = 3; 
      flt_fixtype = 2;
     }
   if ((randval > 6) && (randval <= 16))
     {
      numsats = 4; 
     } 
   if ((randval > 16) && (randval <= 39))
     {
      numsats = 5; 
     }
   if ((randval > 39) && (randval <= 49))
     {
      numsats = 6; 
     }
   
   
   /* sanity check -- should never happen, but if it does, this prevents a blowup... */
   if ((totalsats > 6) || (numsats > 6))
     {
      clear_satellites();
      numsats = 0;
      flt_fixtype = 1; 
     }
   
   
   /* if new number of satellites is less than prior, pick sats for deletion one at at time 
      until count matches new number */
   while (numsats < totalsats)
     {
      randval = random_index(12);  /* random 0 to 11 */ 
      if (satarray[randval][0] != 0)   /* if random spot is NOT blank (in use)... */
        {
         satarray[randval][0] = 0;  /* clear it */
         totalsats--;
        } 
     }   

   /* if new number of satellites is greater than prior value, create a new ID in list */   
   while (numsats > totalsats)
     {
      randval = random_index(12);  /* random 0 to 11 */
      if (satarray[randval][0] == 0)   /* if random spot is blank (not in use)... */
        {
         sprintf(satarray[randval],"%02d",randval);  /* sat ID is its own position number */
         totalsats++;
        } 
     }   



   /* dilution of position stats -- smaller is better, 1.0 is perfect */
   switch (numsats)
     {
      case 3:
        {
         *hdpos = 2.1; 
         *vdpos = 9.9; 
         *pdpos = 9.9; 
         break; 
        } 
      case 4:
        {
         *hdpos = 1.3; 
         *vdpos = 1.6; 
         *pdpos = 1.6; 
         break; 
        } 
      case 5:
        {
         *hdpos = 1.1; 
         *vdpos = 1.2; 
         *pdpos = 1.2; 
         break; 
        } 
      case 6:
        {
         *hdpos = 1.0; 
         *vdpos = 1.0; 
         *pdpos = 1.0; 
         break; 
        } 
      default:
        {
         *hdpos = 9.9; 
         *vdpos = 9.9; 
         *pdpos = 9.9; 
         break; 
        } 
     } 

   return totalsats;
  }



/* This function is called once per script line -- note that each script line
   may represent many seconds (even perhaps hours) of simulated balloon flight
   and so each call to this function will cause output of many lines
   od\f data.  To continue looping, returns nonzero -- a 0 specifies loop is 
   finished. 
*/

int process_script(void)
  {
   long lsec;
   double x,y,z;
   double dsec;
   
   int keycode = 0;

   int dt_pos;
   int lla_pos;
      
   
   double d_lat,d_long,d_alt;
   long d_date, d_time, d_temp;


   char out_strg[120];
   char work_strg[120];
   
   double normlat,normlong;
   char northsouth,eastwest;

   int nsats;
   double hdilpos,pdilpos,vdilpos,geoid_height;

   char status_active;
   double knots;
   double track_angle;
   double magvar_deg;   
   double norm_magvar;
   char magvar_eastwest;

   double t_secs;
   double prior_x_deg;
   double prior_y_deg;
   double prior_t_secs;
   
   int firstloop;


   double linear_x, linear_y, linear_z;



   /* check for keywords first */  
   
   /* The default is that the random generator gives the same sequence for 
      each run.  A user may optionally invoke "Random" in the script 
      to change this behavior.  
      Random 0 or without a parameter causes a time-based random seed different 
      for each run.  Random with any nonzero parameter will set a repeatable 
      random seed useful for replication of simulations (default case uses 1).
   */   

   /* assume is a normal script line with flight sim waypoint data */  

    
   /* advance simulator to next flight segment -- each line represents a waypoint
      with date/time and position x, y, and z -- the simulator will interpolate 
      between waypoints for each second of simulated flight -- convert lat, long 
      data (y, x) to decimal degrees as it is read in */
   flt_last_date = flt_next_date;
   flt_last_time = flt_next_time;
   flt_last_lat = flt_next_lat;
   flt_last_long = flt_next_long;
   flt_last_alt = flt_next_alt;
   
   /* get data for simulator -- equivalent to extracting data from 
      original balscript line */
      
   dt_pos = flt_datapos + flt_datapos;
   lla_pos = flt_datapos + flt_datapos + flt_datapos;


#ifdef USEFLASH
   d_temp = (long)pgm_read_dword(date_time+dt_pos);   
#else   
   d_temp = date_time[dt_pos];   
#endif
      
   if (d_temp == 0)
     {
      return 0;   
     }


#ifdef DEBUG_OUTPUT  


#ifdef USEFLASH
   d_lat  = (float)pgm_read_float(lat_long_alt + lla_pos);   
   d_long = (float)pgm_read_float(lat_long_alt + lla_pos + 1);   
   d_alt  = (float)pgm_read_float(lat_long_alt + lla_pos + 2);   
#else   
   d_lat  = lat_long_alt[lla_pos];   
   d_long = lat_long_alt[lla_pos + 1];   
   d_alt  = lat_long_alt[lla_pos + 2];   
#endif   

   dtostrf_chop(d_lat,-8,3,st_normlat);
   dtostrf_chop(d_long,-8,3,st_normlong);
   dtostrf_chop(d_alt,-8,3,st_work);
   
   sprintf(out_strg,"lat=%s long=%s alt=%s",
                st_normlat,st_normlong,st_work);
   com_string_crlf(portspec,out_strg);

#endif


#ifdef USEFLASH
   d_date = (long)pgm_read_dword(date_time+dt_pos);   
   d_time = (long)pgm_read_dword(date_time+dt_pos+1);   
#else
   d_date = date_time[dt_pos];   
   d_time = date_time[dt_pos+1];   
#endif

   flt_next_date = d_date;
   flt_next_time = d_time;

#ifdef USEFLASH
   d_lat  = (float)pgm_read_float(lat_long_alt + lla_pos);   
   d_long = (float)pgm_read_float(lat_long_alt + lla_pos + 1);   
   d_alt  = (float)pgm_read_float(lat_long_alt + lla_pos + 2);   
#else
   d_lat  = lat_long_alt[lla_pos];   
   d_long = lat_long_alt[lla_pos + 1];   
   d_alt  = lat_long_alt[lla_pos + 2];   
#endif

   flt_next_lat  = deg_coord(d_lat);
   flt_next_long = deg_coord(d_long);

   flt_next_alt  = d_alt;
   
   flt_datapos++;
   

   /* convert combination dates/times numbers of seconds elapsed 
      since the beginning of the first year encountered */   
   flt_last_sec = date_secs(flt_last_date) + time_secs(flt_last_time);   
   flt_next_sec = date_secs(flt_next_date) + time_secs(flt_next_time);   
   
   /* if this is first pass through this function, there is not enough 
      data to interpolate yet, so bypass processing */
   if (flt_firstpass)
     {
      flt_firstpass = FALSE;               
      return 1;
     }   
     
   flt_firstpass = FALSE;               
      


   /* determine interpolation parameters for flight segment -- calculates a set 
      of slopes and intercepts (e.g. m and b in: y = mx + b) for each component 
      dimension x, y, and z which will be used later to interpolate between waypoints */
   interp_setup();
    
   /* simulate all outputs for entire flight segment -- generate 1 second readings 
      for all interpolated positions between specified "last" and "next" locations */
   prior_x_deg = 0.0;
   prior_y_deg = 0.0;
   prior_t_secs = 0.0;


   /* preseed the real-time simulator with clock time */
   time(&flt_time_previous);

   /* note that in the following loop, output for the FIRST second is skipped 
      so that it won't be duplicated later -- for simulation purposes, can afford to 
      miss the very first second (only) if all others are used -- however even the 
      first one is processed to gather tracking data */
      
   firstloop = TRUE;  

   /* will simulate sats coming and going */
   clear_satellites();
   nsats = sim_satellites(4,&hdilpos, &vdilpos, &pdilpos);     /* initialize to 3 satellites */
   
   for (lsec=flt_last_sec; lsec<=flt_next_sec; lsec++)
     {
      /* use slopes and intercepts to interpolate x, y, z data between waypoints */                       
      dsec = lsec-flt_last_sec;                       
      x = flt_x_m * dsec + flt_x_b;                      
      y = flt_y_m * dsec + flt_y_b;                      
      z = flt_z_m * dsec + flt_z_b;     
      t_secs = dsec;           
      
      /* Apply random variation to x, y, z -- DO NOT apply to either first or last 
         iterations, taken from original waypoints assumed to be correct. 
      */
   
      /* hold original interpolated values for debug comparison */
      linear_x = x;
      linear_y = y;
      linear_z = z;
      
      if ((lsec != flt_last_sec) && (lsec != flt_next_sec))
        {
         random_vary_pos(flt_var,&x,&y,&z);
        }

                             
/*    convert lat, long data back to gps format from decimal degrees */                             

      /* set up strings for output as simulation of GPS */ 
      normlat = gps_coord(y);
      normlong = gps_coord(x);
      northsouth = 'N';
      eastwest = 'E';

      status_active = 'A';    /* can be 'A' or 'V' */
 

      magvar_deg = -1.4;   
      magvar_eastwest = 'E';

      if (normlong < 0.000)
        {
         normlong = -normlong;
         eastwest = 'W';
        }
      if (normlat < 0.000)
        {
         normlat = -normlat;
         northsouth = 'S';
        }

      norm_magvar = magvar_deg;
      if (norm_magvar < 0.000)
        {
         norm_magvar = -norm_magvar;
         magvar_eastwest = 'W';
        }

      geoid_height = 47.1;   /* arbitrary -- don't try to simulate this */

      if (!firstloop)
        {
         /* calculate heading and speed from delta x, y, and t */
         if (track_calc(x - prior_x_deg, y - prior_y_deg, 
                        t_secs - prior_t_secs, y,
                        &knots, &track_angle))
           {
            /* actually calculated a heading */                                                                             
           }  
         else
           {
            /* no heading -- decide whether to carry forward prior heading with 0 speed */  
           }                                                                              

         /* NOTE:  for now, will ignore what happens if clock gets "behind" due to computer
                   being too busy to keep up -- pretend it is on schedule.
                   Later, should force a data droput for each "catch-up" second detected 
         */          

         /* since the slope of the z regression is in meters and t is in seconds      
            it happens that flt_z_m is the average climb rate in meters per sec */

         /* also apply any user-entered offset to climb rate for dynamic testing of 
            onboard computer's response (ballast  drop, etc.) -- note that the user 
            input is a second-by-second variation which superimposes on the scripted
            fight path, therefore flt_climb_offset is cumulative for all seconds since 
            a change is applied to climb rate -- this cumulative offset is added to scripted
            altitude to get the altitude after user changes have been applied */
    
	
	     /* Apply 1-second clock for realtime output (unless no port specified in Windows/Linux) */
         #ifdef REALTIME

            #ifdef ARDUINO		 
            /* wait until next observed change of second on real time clock */
               while (!seconds_elapsed())
                 {
                 } 
            #else
               if (portspec)
                 {			   
                  /* wait until next observed change of second on real time clock */
                  wait_seconds(1);
				 } 
			#endif

         #endif  

        /* NOTE: Workaround is needed for lack of printf() floats in standard Arduino software 
		   -- they CAN be had via printf(), but must compile with an alternate library and
           current Arduino software does not OBVIOUSLY support alternate compile flags (they're 
		   working on it, maybe) -- therefore standard Arduino code can't use %f or %lf formats */
        
        /* Arduino's inclusion of avr-libc SHOULD support a dtostrf() function which fills
           the gap, but MinGW (Windows) DOES NOT have this function.  An emulated function is 
		   supplied above to allow code for all OSes to use the same method as Arduino. */ 
           

/* --------- OUTPUT OF NMEA SENTENCES TO SERIAL PORT ---------------------------------- */

         /* GPSSIM simulates acquisition of satellites such that at least some GPS readings
            are simulated as "missed", with inadequate satellite reception.  This is partly
            random, partly predetermined.  Acquisition is indicated by a "3" (for 3D) in the 
            GSA sentence "3D fix" field -- a 1 indicates no fix.
            
            Randomly selected readings will be skipped to indicate signal dropouts.  Simulated
            dropouts will routinely be short, a inute or two, but the simulator will ensure 
            that at least one dropout in 6 simulated hours exceeds 10 minutes of "no fix" 
            to allow testing the controller software in handling signal dropouts.
            
            When a droput occurs, only a GSA sentence will be output.
         */   


         /* keep satellite list stable for about a minute or two, then randomly change list */

      #ifndef PERFECT_SAT_FIXES             
         if (flt_cyclect == 0)  /* if it's time for a long dropout... */
           {
            /* SPECIAL -- force extended dropout period every 6 hours or so */
            clear_satellites();
            nsats = 0;
            clear_dilutions(&hdilpos, &vdilpos, &pdilpos);  

            flt_dropoutct++;
            if (flt_dropoutct >= DROPOUT_SAT_SECONDS)
              {
               flt_dropoutct = 0;
               flt_cyclect++;
               nsats = sim_satellites(3, &hdilpos, &vdilpos, &pdilpos);
              }
           }
         else
           {
      #else
         if (1)
           {
      #endif  
            flt_stablect++;
            if (flt_stablect >= STABLE_SAT_SECONDS)
              {
               flt_stablect = 0; 
    
#ifndef PERFECT_SAT_FIXES
               flt_cyclect++;
               if (flt_cyclect >= DROPOUT_CYCLES)
                 {
                  flt_cyclect = 0; 
                 }
#endif    
               /* randomly simulate a list of satellites visible */  
               nsats = sim_satellites(0, &hdilpos, &vdilpos, &pdilpos);
              }
           }  

         /* at this point satellites are set up -- the following executes once per second... */  

         /* --------------------- GPRMC sentence -------------------- */
#ifdef NMEA23
         dtostrf_chop(normlat,-6,4,st_normlat);
         dtostrf_chop(normlong,-6,4,st_normlong);
#else
         dtostrf_chop(normlat,-5,3,st_normlat);
         dtostrf_chop(normlong,-5,3,st_normlong);
#endif

         dtostrf_chop(knots,-5,1,st_knots);
         dtostrf_chop(track_angle,-5,1,st_track_angle);
         dtostrf_chop(norm_magvar,-3,1,st_norm_magvar);

         status_active = 'A';
       
         
         if (flt_fixtype != 1)   
           {
            #ifdef NMEA23
            sprintf(work_strg,"GPRMC,%06ld,%c,%s,%c,%s,%c,"
                                    "%s,%s,%06ld,%s,%c,A",
                                    secs_to_time(lsec),status_active,st_normlat,northsouth,st_normlong,eastwest,
                                    st_knots,st_track_angle,secs_to_date(lsec),st_norm_magvar,magvar_eastwest);
            #else
            sprintf(work_strg,"GPRMC,%06ld,%c,%s,%c,%s,%c,"
                                    "%s,%s,%06ld,%s,%c",
                                    secs_to_time(lsec),status_active,st_normlat,northsouth,st_normlong,eastwest,
                                    st_knots,st_track_angle,secs_to_date(lsec),st_norm_magvar,magvar_eastwest);
            #endif
           }  
         else   /* invalid data -- no fix */
           {
            status_active = 'V'; 
            if (nsats == 0)
              {
               #ifdef NMEA23
               sprintf(work_strg,"GPRMC,,%c,,,,,,,%06ld,,,N",status_active,secs_to_date(lsec));
               #else
               sprintf(work_strg,"GPRMC,,%c,,,,,,,%06ld,,",status_active,secs_to_date(lsec));
               #endif
              }
            else
              {
               #ifdef NMEA23
               sprintf(work_strg,"GPRMC,%06ld,%c,%s,%c,%s,%c,"
                                    "%s,%s,%06ld,%s,%c,%c",
                                    secs_to_time(lsec),status_active,st_normlat,northsouth,st_normlong,eastwest,
                                    st_knots,st_track_angle,secs_to_date(lsec),st_norm_magvar,magvar_eastwest,status_active);
               #else
               sprintf(work_strg,"GPRMC,%06ld,%c,%s,%c,%s,%c,"
                                    "%s,%s,%06ld,%s,%c",
                                    secs_to_time(lsec),status_active,st_normlat,northsouth,st_normlong,eastwest,
                                    st_knots,st_track_angle,secs_to_date(lsec),st_norm_magvar,magvar_eastwest);
               #endif
              }  
           }  
  
         sprintf(out_strg,"$%s*%02X",work_strg,checksum(work_strg));  /* still needs CRLF */
         com_string_crlf(portspec,out_strg);

         /* --------------------- GPGGA sentence -------------------- */
      
#ifdef NMEA23
         dtostrf_chop(normlat,-6,4,st_normlat);
         dtostrf_chop(normlong,-6,4,st_normlong);
#else
         dtostrf_chop(normlat,-5,3,st_normlat);
         dtostrf_chop(normlong,-5,3,st_normlong);
#endif

         dtostrf_chop(hdilpos,-3,1,st_hdilpos);
         dtostrf_chop(z,-3,1,st_z);
         dtostrf_chop(geoid_height,-3,1,st_geoid_height);
  
  
         if (flt_fixtype != 1)   
           {
            sprintf(work_strg,"GPGGA,%06ld,%s,%c,%s,%c,"
                                    "%d,%02d,%s,%s,M,%s,M,,",
                                    secs_to_time(lsec),st_normlat,northsouth,st_normlong,eastwest,
                                    1,nsats,st_hdilpos,st_z,st_geoid_height);
           }  
         else   /* invalid data -- no fix */
           {
            if (nsats = 0)
              {
               sprintf(work_strg,"GPGGA,,,,,,"
                                    "%d,%02d,,,M,,M,,",0,nsats);
              }
            else
              {
               #ifdef NEMA23
                sprintf(work_strg,"GPGGA,%06ld,%s,%c,%s,%c,"
                                    "%d,%02d,%s,%s,M,%s,M,,",
                                    secs_to_time(lsec),st_normlat,northsouth,st_normlong,eastwest,
                                    6,nsats,st_hdilpos,st_z,st_geoid_height);
               #else
               sprintf(work_strg,"GPGGA,%06ld,%s,%c,%s,%c,"
                                    "%d,%02d,%s,%s,M,%s,M,,",
                                    secs_to_time(lsec),st_normlat,northsouth,st_normlong,eastwest,
                                    0,nsats,st_hdilpos,st_z,st_geoid_height);
               #endif                     
              }  
           }  
     
         sprintf(out_strg,"$%s*%02X",work_strg,checksum(work_strg));  /* still needs CRLF */
         com_string_crlf(portspec,out_strg);
   
         /* --------------------- GPGSA sentence -------------------- */

         dtostrf_chop(hdilpos,-3,1,st_hdilpos);
         dtostrf_chop(pdilpos,-3,1,st_pdilpos);
         dtostrf_chop(vdilpos,-3,1,st_vdilpos);

         sprintf(work_strg,"GPGSA,%c,%d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,"
                                        "%s,%s,%s",
                                 status_active,       
                                 flt_fixtype,
                                 satarray[0],
                                 satarray[1],
                                 satarray[2],
                                 satarray[3],
                                 satarray[4],
                                 satarray[5],
                                 satarray[6],
                                 satarray[7],
                                 satarray[8],
                                 satarray[9],
                                 satarray[10],
                                 satarray[11],
                                 st_pdilpos,st_hdilpos,st_vdilpos);
                                 
   
         sprintf(out_strg,"$%s*%02X",work_strg,checksum(work_strg));  /* still needs CRLF */
         com_string_crlf(portspec,out_strg);
        }
        
      firstloop = FALSE;  
            
      /* NOW FOR THE POINT of isolating the first loop -- hold the first data for deltas */
      prior_x_deg = x;
      prior_y_deg = y;
      prior_t_secs =  t_secs;
     }

   return 1;
  }

  
  

  
#ifdef ARDUINO

/* ------- Arduino only -- alternative to main() -------------------------------- */
/** MAIN program Setup
 */
void setup()                    // run once, when the sketch starts
{
 /* initialize for seconds_elapsed() */
 last_time = millis();

 Serial.begin(BAUD_RATE);
 
 #ifdef DEBUG_OUTPUT
    serial_puts("GPSSIM 1.03 -- GLF 03/14/2011 for LVL1 -- GPS NMEA Output Emulator\r\n");
 #endif 
  
 /* main section of original Windows GPSSIM can largely go here */

 open_script();

 while (process_script())
   {
    #ifdef DEBUG_OUTPUT      
       check_mem(); 
       sprintf(x_work,"HP %04X  SP %04X\r\n",heapptr,stackptr);
       serial_puts(x_work);
    #endif   
   }
      
 close_script();

 for (;;)
   {
    #ifdef DEBUG_OUTPUT    
       if (seconds_elapsed())
         {
          serial_puts("Done.\r\n"); 
         } 
    #endif  
   }
}


/* -----------------------------------------------------------------  */
/** Arduino standard MAIN program Loop
 */
void loop()                     // run over and over again
{
 /* never actually gets here -- all code run in setup() above */
}

 
#else

/*  Windows/Linux only --- main() is needed -- Arduino does it diferently (no command line or environment) */  

main(int argc, char *argv[])

{
 char strg[1000];
 char work[100];
 long recct;
 int recshow;
 int tval;

 double val;

 printf("\nGPSSIM 1.03 -- GLF 03/14/2011 for LVL1 -- GPS NMEA Output Emulator\n"
          "--------------------------------------------------------------------------\n");  
 
 if (argc > 1)
   {
    strncpy(work,argv[1],64);
    work[64] = 0;
   }

 portbaud = BAUD_RATE;
 portspec = stri(work,0,0);
 if ((portspec < 1) || (portspec > 8))
   {
    portspec = 0;           
   }

 if (argc > 2)
   {
    strncpy(work,argv[2],64);
    work[64] = 0;

    tval = stri(work,0,0);
    if (tval < 0)
      {
       tval = -tval;
      }
    
    if ((tval == 2400) || (tval == 4800) || (tval == 9600) || 
                                  (tval == 19200) || (tval == 38400))
      {
       portbaud = tval;                           
      }
   }
   
 flt_realtime = FALSE;
  

#ifdef REALTIME
 if (portspec)
   {
    flt_realtime = TRUE;
   }
#endif
 
 if (portspec)
   {
    if (flt_realtime)
      {
       printf("\nSerial output is in REAL TIME (1 GPS data group per second)\n");
      }
    else
      {
       printf("\nSerial output is not delayed (maximum speed for baud rate)\n");
      }  
   }   
 else
   {
    printf("There is no serial output port specified.  Will output to screen instead.\n");
   } 


 printf("Port: %d   Baud: %d\n\n",portspec,portbaud);

 if (portspec)
   {        
    if (open_com(portspec,portbaud,8,0,1))
      {
       atexit(early_exit_closecom);
      }
    else
      {
       printf("\nCOM PORT %d NOT OPENED!\n",portspec);
	   exit(1);
      } 
   }   
  
 open_script();

 recct = 0;


#ifdef DEBUG_OUTPUT
/* ---------------- */
 printf("Experiment with float output:\n");
 
 val = -497.23;
/* string_double(work,val,7,2); */
 dtostrf(val,7,2,work);
 
 printf("val = %14.5lf    strg = [%s]  7.2\n", val,work);    

 val = -0.0;
/* string_double(work,val,7,2); */
 dtostrf(val,7,2,work);
 
 printf("val = %14.5lf    strg = [%s]  7.2\n", val,work);    

 val = 0.005;
/* string_double(work,val,7,2); */
 dtostrf(val,7,2,work);
 
 printf("val = %14.5lf    strg = [%s]  7.2\n", val,work);    

 val = 0.0;
/* string_double(work,val,7,2); */
 dtostrf(val,7,2,work);
 
 printf("val = %14.5lf    strg = [%s]  7.2\n", val,work);    

 val = 0.0;
/* string_double(work,val,7,2); */
 dtostrf(val,3,0,work);
 
 printf("val = %14.5lf    strg = [%s]  3.0\n", val,work);    

 val = 0.0;
/* string_double(work,val,7,2); */
 dtostrf(val,3,1,work);
 
 printf("val = %14.5lf    strg = [%s]  3.1\n", val,work);    

 val = 0.0;
/* string_double(work,val,7,2); */
 dtostrf(val,3,2,work);
 
 printf("val = %14.5lf    strg = [%s]  3.2\n", val,work);    

 val = -497.23;
/* string_double(work,val,7,2); */
 dtostrf(val,7,2,work);
 
 printf("val = %14.5lf    strg = [%s]  7.2\n", val,work);    

 val = -497.23;
/* string_double(work,val,7,2); */
 dtostrf(val,7,2,work);
 
 printf("val = %14.5lf    strg = [%s] 7.2\n", val,work);    

 val = -7.2368;
/* string_double(work,val,7,2); */
 dtostrf(val,7,2,work);
 printf("val = %14.5lf    strg = [%s]  7.2\n", val,work);    

 val = 7.2329;
/* string_double(work,val,7,6); */
 dtostrf(val,7,6,work);
 printf("val = %14.5lf    strg = [%s]  7.6\n", val,work);    

 val = 999999.9999;
/*  string_double(work,val,7,2); */
 dtostrf(val,7,2,work);
 printf("val = %14.5lf    strg = [%s]  7.2\n", val,work);    

 val = -497.23;
/* string_double(work,val,7,2); */
 dtostrf(val,-7,2,work);
 
 printf("val = %14.5lf    strg = [%s]  -7.2\n", val,work);    

 val = -0.0;
/* string_double(work,val,7,2); */
 dtostrf(val,-7,2,work);
 
 printf("val = %14.5lf    strg = [%s]  -7.2\n", val,work);    

 val = 0.005;
/* string_double(work,val,7,2); */
 dtostrf(val,-7,2,work);
 
 printf("val = %14.5lf    strg = [%s]  -7.2\n", val,work);    

 val = 0.0;
/* string_double(work,val,7,2); */
 dtostrf(val,-7,2,work);
 
 printf("val = %14.5lf    strg = [%s]  -7.2\n", val,work);    

 val = 0.0;
/* string_double(work,val,7,2); */
 dtostrf(val,-3,0,work);
 
 printf("val = %14.5lf    strg = [%s]  -3.0\n", val,work);    

 val = 0.0;
/* string_double(work,val,7,2); */
 dtostrf(val,-3,1,work);
 
 printf("val = %14.5lf    strg = [%s]  -3.1\n", val,work);    

 val = 0.0;
/* string_double(work,val,7,2); */
 dtostrf(val,-3,2,work);
 
 printf("val = %14.5lf    strg = [%s]  -3.2\n", val,work);    

 val = -497.23;
/* string_double(work,val,7,2); */
 dtostrf(val,-7,2,work);
 
 printf("val = %14.5lf    strg = [%s]  -7.2\n", val,work);    

 val = -497.23;
/* string_double(work,val,7,2); */
 dtostrf(val,-7,2,work);
 
 printf("val = %14.5lf    strg = [%s]  -7.2\n", val,work);    

 val = -7.2368;
/* string_double(work,val,7,2); */
 dtostrf(val,-7,2,work);
 printf("val = %14.5lf    strg = [%s]  -7.2\n", val,work);    

 val = 7.2329;
/* string_double(work,val,7,6); */
 dtostrf(val,-7,6,work);
 printf("val = %14.5lf    strg = [%s]  -7.6\n", val,work);    

 val = 999999.9999;
/*  string_double(work,val,7,2); */
 dtostrf(val,-7,2,work);
 printf("val = %14.5lf    strg = [%s]  -7.2\n", val,work);    

 val = -497.23;
/* string_double(work,val,7,2); */
 dtostrf_chop(val,-7,2,work);
 
 printf("val = %14.5lf    strg = [%s]  -7.2 chop\n", val,work);    

 val = -0.0;
/* string_double(work,val,7,2); */
 dtostrf_chop(val,-7,2,work);
 
 printf("val = %14.5lf    strg = [%s]  -7.2 chop\n", val,work);    

 val = 0.005;
/* string_double(work,val,7,2); */
 dtostrf_chop(val,-7,2,work);
 
 printf("val = %14.5lf    strg = [%s]  -7.2 chop\n", val,work);    

 val = 0.0;
/* string_double(work,val,7,2); */
 dtostrf_chop(val,-7,2,work);
 
 printf("val = %14.5lf    strg = [%s]  -7.2 chop\n", val,work);    

 val = 0.0;
/* string_double(work,val,7,2); */
 dtostrf_chop(val,-3,0,work);
 
 printf("val = %14.5lf    strg = [%s]  -3.0 chop\n", val,work);    

 val = 0.0;
/* string_double(work,val,7,2); */
 dtostrf_chop(val,-3,1,work);
 
 printf("val = %14.5lf    strg = [%s]  -3.1 chop\n", val,work);    

 val = 0.0;
/* string_double(work,val,7,2); */
 dtostrf_chop(val,-3,2,work);
 
 printf("val = %14.5lf    strg = [%s]  -3.2 chop\n", val,work);    

 val = -497.23;
/* string_double(work,val,7,2); */
 dtostrf_chop(val,-7,2,work);
 
 printf("val = %14.5lf    strg = [%s]  -7.2 chop\n", val,work);    

 val = -497.23;
/* string_double(work,val,7,2); */
 dtostrf_chop(val,-7,2,work);
 
 printf("val = %14.5lf    strg = [%s]  -7.2 chop\n", val,work);    

 val = -7.2368;
/* string_double(work,val,7,2); */
 dtostrf_chop(val,-7,2,work);
 printf("val = %14.5lf    strg = [%s]  -7.2 chop\n", val,work);    

 val = 7.2329;
/* string_double(work,val,7,6); */
 dtostrf_chop(val,-7,6,work);
 printf("val = %14.5lf    strg = [%s]  -7.6 chop\n", val,work);    

 val = 999999.9999;
/*  string_double(work,val,7,2); */
 dtostrf_chop(val,-7,2,work);
 printf("val = %14.5lf    strg = [%s]  -7.2 chop\n", val,work);    

/*  exit(1); */
/* ---------------- */
#endif 
 

 if (flt_realtime)
   {
    printf("Processing waypoint script lines in REAL TIME (takes a LONG time)...\n\n");
   }
 else
   { 
    printf("Processing waypoint script lines (accelerated output)...\n\n");
   }

 while (process_script())
   {
    recct++;
   }
      
 close_script();
        
 printf("\n%8ld records processed\n",recct);
 
 if (portspec)
   {
    /* delay 3 seconds to allow port to finish output buffer */
    wait_seconds(3);
   } 
  
 close_com(portspec);            
}

#endif
