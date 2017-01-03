EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:wemos_mini
LIBS:PartLib
LIBS:MeetingRoomDisplayConnector-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L WeMos_mini U1
U 1 1 5846DA55
P 5350 4000
F 0 "U1" H 5350 4500 60  0000 C CNN
F 1 "WeMos_mini" H 5350 3500 60  0000 C CNN
F 2 "wemos_d1_mini:D1_mini_board" H 5900 3300 60  0001 C CNN
F 3 "" H 5900 3300 60  0000 C CNN
	1    5350 4000
	1    0    0    -1  
$EndComp
$Comp
L ILI9341_24_Touch U2
U 1 1 5846DFFC
P 7300 4050
F 0 "U2" H 7850 2850 60  0000 C CNN
F 1 "ILI9341_24_Touch" H 7450 4250 60  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x14" H 7300 4250 60  0001 C CNN
F 3 "" H 7300 4250 60  0000 C CNN
	1    7300 4050
	1    0    0    -1  
$EndComp
Wire Wire Line
	5850 3650 6250 3650
Wire Wire Line
	4850 3750 4200 3750
$Comp
L GND #PWR01
U 1 1 5846E091
P 4200 3750
F 0 "#PWR01" H 4200 3500 50  0001 C CNN
F 1 "GND" H 4200 3600 50  0000 C CNN
F 2 "" H 4200 3750 50  0000 C CNN
F 3 "" H 4200 3750 50  0000 C CNN
	1    4200 3750
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 5846E0A9
P 6350 4850
F 0 "#PWR02" H 6350 4600 50  0001 C CNN
F 1 "GND" H 6350 4700 50  0000 C CNN
F 2 "" H 6350 4850 50  0000 C CNN
F 3 "" H 6350 4850 50  0000 C CNN
	1    6350 4850
	1    0    0    -1  
$EndComp
Wire Wire Line
	6350 4850 6500 4850
Wire Wire Line
	6500 4850 6500 4900
Wire Wire Line
	6500 4900 6600 4900
Text Label 6250 3650 2    60   ~ 0
VCC
Text Label 6250 5300 2    60   ~ 0
VCC
Wire Wire Line
	6600 5050 6600 5300
Wire Wire Line
	6600 5300 6250 5300
Wire Wire Line
	4850 4150 4600 4150
Wire Wire Line
	4600 4150 4600 4750
Wire Wire Line
	4600 4750 6600 4750
Wire Wire Line
	5850 4350 5950 4350
Wire Wire Line
	5950 4350 5950 4600
Wire Wire Line
	5950 4600 6600 4600
Wire Wire Line
	4850 4050 4500 4050
Wire Wire Line
	4500 4050 4500 4850
Wire Wire Line
	4500 4850 6250 4850
Wire Wire Line
	6250 4850 6250 4450
Wire Wire Line
	6250 4450 6600 4450
Wire Wire Line
	5850 3850 6350 3850
Wire Wire Line
	6250 3850 6250 4300
Wire Wire Line
	6250 4300 6600 4300
Wire Wire Line
	6600 3400 6350 3400
Wire Wire Line
	6350 3400 6350 3850
Connection ~ 6250 3850
Wire Wire Line
	5850 4050 6400 4050
Wire Wire Line
	6350 4050 6350 4150
Wire Wire Line
	6350 4150 6600 4150
Wire Wire Line
	6600 3700 6400 3700
Wire Wire Line
	6400 3700 6400 4050
Connection ~ 6350 4050
Wire Wire Line
	5850 3950 6000 3950
Wire Wire Line
	6000 3950 6000 3250
Wire Wire Line
	6000 3250 6600 3250
Wire Wire Line
	6600 3850 6450 3850
Wire Wire Line
	6450 3850 6450 3250
Connection ~ 6450 3250
Wire Wire Line
	4850 3850 4500 3850
Wire Wire Line
	4500 3850 4500 3100
Wire Wire Line
	4500 3100 6600 3100
Wire Wire Line
	4850 3950 4650 3950
Wire Wire Line
	4650 3950 4650 3500
Wire Wire Line
	4650 3500 6600 3500
Wire Wire Line
	6600 3500 6600 3550
Wire Wire Line
	5850 3750 6100 3750
Wire Wire Line
	6100 3750 6100 4000
Wire Wire Line
	6100 4000 6600 4000
$EndSCHEMATC
