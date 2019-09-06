#include<iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <vector>
#include "cstdio"
#include "iostream"
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <algorithm>


using namespace std;
#define timestep 288  //frequency
// class Pancreas
// {
// public:                             //ignore this part if you have the real data
    
	// Initialization//
	std::vector<double> dataArray;
	double ts=0.08333333;
	double gb = double(291.0);
	double p1 = double(3.17e-2);
	double p2 = double(1.23e-2);
	double si = double(2.9e-2);
	double ke = double(9.0e-2);
	double kabs = double(1.2e-2);
	double kemp = double(1.8e-1);
	double f = double(8.00e-1);
	double vi = double(12.0);
	double vg = double(12.0);
	double g[289]={double(76.22)};//glucouse
	double x[289]={double(33.33)};
	double I[289]={double(33.33)};
	double q1[289]={double(16.67)};
	double q2[289]={double(16.67)};
	double g_gut[289]={double(250.0)};
	double u[timestep + 1]={double(3.0)};

	//meal
	double d[289] = { 942, 948, 948, 954, 960, 966, 978, 1008, 1026, 1044, 1050, 1278, 1278, 1278, 1278, 1278, 1272,//effect  of meal
		1260, 1248, 1236, 1212, 1188, 1164, 1146, 1128, 1110, 1092, 1080, 1068, 1056, 1056, 1056, 1038, 1014, 990,
		966, 942, 918, 894, 876, 858, 840, 822, 810, 798, 774, 762, 774, 780, 756, 714, 678, 642, 612, 588, 570, 558,
		546, 546, 552, 564, 564, 564, 558, 558, 564, 564, 564, 558, 558, 552, 552, 564, 570, 582, 594, 606, 618, 630,
		642, 654, 672, 690, 696, 702, 702, 708, 708, 708, 708, 714, 708, 702, 702, 702, 708, 714, 714, 702, 696, 696, 714,
		738, 762, 780, 798, 714, 708, 702, 714, 732, 744, 762, 780, 792, 804, 804, 804, 816, 834, 876, 924, 966, 1026, 1092,
		1158, 1206, 1242, 1284, 1320, 1326, 1314, 1290, 1266, 1236, 1188, 1152, 1128, 1098, 1074, 1050, 1038, 1056, 1062, 1062,
		1044, 1026, 1032, 1032, 1008, 984, 948, 924, 888, 846, 810, 780, 762, 750, 738, 726, 720, 708, 708, 714, 684, 660, 636, 612,
		576, 552, 540, 540, 540, 534, 534, 522, 516, 510, 522, 546, 564, 588, 600, 624, 672, 720, 762, 810, 846, 858, 870, 888, 906,
		924, 948, 960, 978, 996, 1008, 1014, 1026, 1038, 1062, 1086, 1116, 1140, 1146, 1146, 1152, 1146, 1116, 1086, 1062, 1038, 1020,
		1002, 984, 978, 960, 936, 912, 876, 834, 774, 714, 654, 600, 570, 564, 570, 582, 600, 612, 630, 660, 696, 744, 786, 816, 828, 828, 912, 948, 1000, 1100, 1200, 1400, 1482, 1512, 1524, 1500, 1464, 1476, 1512, 1542, 1536, 1512, 1482, 1434, 1386, 1332, 1290, 1260, 1230, 1206, 1176, 1146, 1116, 1086, 1062, 1044, 1032, 1014, 1002, 990, 978, 972, 954, 948, 954, 966, 966, 954, 942, 942, 942, 942, 942 };

	
	

	double pump_output(int i)//amount of insulin in pump
	{
		return u[i];
	}
	// void sensor_initialiaztion()//ignore this part if you have the real data
	// {

	// }
// // };
