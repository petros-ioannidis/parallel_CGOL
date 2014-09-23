#!/usr/bin/perl
use warnings;
use Benchmark qw(:all);
foreach (@ARGV){
	if( $_ eq "-h"){
		print("Usage: $0 -d [density] -n [test cases] -o [prefix of output files] -g [number of generations] --silent
		       density: black/(white+black)
		       test cases: the number of test cases produced
		       silent:no output\n");
		exit 0;
	}
}

if($#ARGV+1 < 8 || $ARGV[0] ne "-d" || $ARGV[2] ne "-n" || $ARGV[4] ne "-o" || $ARGV[6] ne "-g"){
	die("Usage: $0 -d [density] -n [test cases] -o [prefix of output files] -g [number of generations]
	    density: black/(white+black)
	    test cases: the number of test cases produced\n");
}

my $silent = "";

unless($ARGV[$#ARGV] ne "--silent"){
	$silent = " > /dev/null";
}

my $density = $ARGV[1];
my $test_cases = $ARGV[3];
my $current_test = $ARGV[5]."_current_test";
my $gen = $ARGV[7];
my $plot = "_time.plot";
my %dimensions = (
	4 => [4],
	8 => [4],
	9 => [9],
	12 => [4],
	16 => [4, 16],
	18 => [9],
	20 => [4],
	24 => [4],
	25 => [25],
	27 => [9],
	28 => [4],
	32 => [4, 16],
	36 => [4, 9, 36],
	40 => [4],
	44 => [4],
	45 => [9],
	48 => [4, 16],
	49 => [49],
	50 => [25],
	52 => [4],
	54 => [9],
	56 => [4],
	60 => [4],
	63 => [9],
	64 => [4, 16],
	68 => [4],
	72 => [4, 9, 36],
	75 => [25],
	76 => [4],
	80 => [4, 16],
	81 => [9],
	84 => [4],
	88 => [4],
	90 => [9],
	92 => [4],
	96 => [4, 16],
	98 => [49],
	99 => [9],
	100 => [4, 25],
	104 => [4],
	108 => [4, 9, 36],
	112 => [4, 16],
	116 => [4],
	117 => [9],
	120 => [4],
	124 => [4],
	125 => [25],
	126 => [9],
	128 => [4, 16],
	132 => [4],
	135 => [9],
	136 => [4],
	140 => [4], 144 => [4, 9, 16, 36],
	147 => [49],
	148 => [4],
	150 => [25],
	152 => [4],
	153 => [9],
	156 => [4],
	160 => [4, 16],
	162 => [9],
	164 => [4],
	168 => [4],
	171 => [9],
	172 => [4],
	175 => [25],
	176 => [4, 16],
	180 => [4, 9, 36],
	184 => [4],
	188 => [4],
	189 => [9],
	192 => [4, 16],
	196 => [4, 49],
	198 => [9],
	200 => [4, 25],
	204 => [4],
	207 => [9],
	208 => [4, 16],
	212 => [4],
	216 => [4, 9, 36],
	220 => [4],
	224 => [4, 16],
	225 => [9, 25],
	228 => [4],
	232 => [4],
	234 => [9],
	236 => [4],
	240 => [4, 16],
	243 => [9],
	244 => [4],
	245 => [49],
	248 => [4],
	250 => [25],
	252 => [4, 9, 36],
	256 => [4, 16],
	260 => [4],
	261 => [9],
	264 => [4],
	268 => [4],
	270 => [9],
	272 => [4, 16],
	275 => [25],
	276 => [4],
	279 => [9],
	280 => [4],
	284 => [4],
	288 => [4, 9, 16, 36],
	292 => [4],
	294 => [49],
	296 => [4],
	297 => [9],
	300 => [4, 25],
	304 => [4, 16],
	306 => [9],
	308 => [4],
	312 => [4],
	315 => [9],
	316 => [4],
	320 => [4, 16],
	324 => [4, 9, 36],
	325 => [25],
	328 => [4],
	332 => [4],
	333 => [9],
	336 => [4, 16],
	340 => [4],
	342 => [9],
	343 => [49],
	344 => [4],
	348 => [4],
	350 => [25],
	351 => [9],
	352 => [4, 16],
	356 => [4],
	360 => [4, 9, 36],
	364 => [4],
	368 => [4, 16],
	369 => [9],
	372 => [4],
	375 => [25],
	376 => [4],
	378 => [9],
	380 => [4],
	384 => [4, 16],
	387 => [9],
	388 => [4],
	392 => [4, 49],
	396 => [4, 9, 36],
	400 => [4, 16, 25],
	404 => [4],
	405 => [9],
	408 => [4],
	412 => [4],
	414 => [9],
	416 => [4, 16],
	420 => [4],
	423 => [9],
	424 => [4],
	425 => [25],
	428 => [4],
	432 => [4, 9, 16, 36],
	436 => [4],
	440 => [4],
	441 => [9, 49],
	444 => [4],
	448 => [4, 16],
	450 => [9, 25],
	452 => [4],
	456 => [4],
	459 => [9],
	460 => [4],
	464 => [4, 16],
	468 => [4, 9, 36],
	472 => [4],
	475 => [25],
	476 => [4],
	477 => [9],
	480 => [4, 16],
	484 => [4],
	486 => [9],
	488 => [4],
	490 => [49],
	492 => [4],
	495 => [9],
	496 => [4, 16],
	500 => [4, 25],
	504 => [4, 9, 36],
	508 => [4],
	512 => [4, 16],
	513 => [9],
	516 => [4],
	520 => [4],
	522 => [9],
	524 => [4],
	525 => [25],
	528 => [4, 16],
	531 => [9],
	532 => [4],
	536 => [4],
	539 => [49],
	540 => [4, 9, 36],
	544 => [4, 16],
	548 => [4],
	549 => [9],
	550 => [25],
	552 => [4],
	556 => [4],
	558 => [9],
	560 => [4, 16],
	564 => [4],
	567 => [9],
	568 => [4],
	572 => [4],
	575 => [25],
	576 => [4, 9, 16, 36],
	580 => [4],
	584 => [4],
	585 => [9],
	588 => [4, 49],
	592 => [4, 16],
	594 => [9],
	596 => [4],
	600 => [4, 25],
	603 => [9],
	604 => [4],
	608 => [4, 16],
	612 => [4, 9, 36],
	616 => [4],
	620 => [4],
	621 => [9],
	624 => [4, 16],
	625 => [25],
	628 => [4],
	630 => [9],
	632 => [4],
	636 => [4],
	637 => [49],
	639 => [9],
	640 => [4, 16],
	644 => [4],
	648 => [4, 9, 36],
	650 => [25],
	652 => [4],
	656 => [4, 16],
	657 => [9],
	660 => [4],
	664 => [4],
	666 => [9],
	668 => [4],
	672 => [4, 16],
	675 => [9, 25],
	676 => [4],
	680 => [4],
	684 => [4, 9, 36],
	686 => [49],
	688 => [4, 16],
	692 => [4],
	693 => [9],
	696 => [4],
	700 => [4, 25],
	702 => [9],
	704 => [4, 16],
	708 => [4],
	711 => [9],
	712 => [4],
	716 => [4],
	720 => [4, 9, 16, 36],
	724 => [4],
	725 => [25],
	728 => [4],
	729 => [9],
	732 => [4],
	735 => [49],
	736 => [4, 16],
	738 => [9],
	740 => [4],
	744 => [4],
	747 => [9],
	748 => [4],
	750 => [25],
	752 => [4, 16],
	756 => [4, 9, 36],
	760 => [4],
	764 => [4],
	765 => [9],
	768 => [4, 16],
	772 => [4],
	774 => [9],
	775 => [25],
	776 => [4],
	780 => [4],
	783 => [9],
	784 => [4, 16, 49],
	788 => [4],
	792 => [4, 9, 36],
	796 => [4],
	800 => [4, 16, 25],
	801 => [9],
	804 => [4],
	808 => [4],
	810 => [9],
	812 => [4],
	816 => [4, 16],
	819 => [9],
	820 => [4],
	824 => [4],
	825 => [25],
	828 => [4, 9, 36],
	832 => [4, 16],
	833 => [49],
	836 => [4],
	837 => [9],
	840 => [4],
	844 => [4],
	846 => [9],
	848 => [4, 16],
	850 => [25],
	852 => [4],
	855 => [9],
	856 => [4],
	860 => [4],
	864 => [4, 9, 16, 36],
	868 => [4],
	872 => [4],
	873 => [9],
	875 => [25],
	876 => [4],
	880 => [4, 16],
	882 => [9, 49],
	884 => [4],
	888 => [4],
	891 => [9],
	892 => [4],
	896 => [4, 16],
	900 => [4, 9, 25, 36],
	904 => [4],
	908 => [4],
	909 => [9],
	912 => [4, 16],
	916 => [4],
	918 => [9],
	920 => [4],
	924 => [4],
	925 => [25],
	927 => [9],
	928 => [4, 16],
	931 => [49],
	932 => [4],
	936 => [4, 9, 36],
	940 => [4],
	944 => [4, 16],
	945 => [9],
	948 => [4],
	950 => [25],
	952 => [4],
	954 => [9],
	956 => [4],
	960 => [4, 16],
	963 => [9],
	964 => [4],
	968 => [4],
	972 => [4, 9, 36],
	975 => [25],
	976 => [4, 16],
	980 => [4, 49],
	981 => [9],
	984 => [4],
	988 => [4],
	990 => [9],
	992 => [4, 16],
	996 => [4],
	999 => [9],
	1000 => [4, 25],
	1004 => [4],
	1008 => [4, 9, 16, 36],
	1012 => [4],
	1016 => [4],
	1017 => [9],
	1020 => [4],
	1024 => [4, 16],
	1025 => [25],
	1026 => [9],
	1028 => [4],
	1029 => [49],
	1032 => [4],
	1035 => [9],
	1036 => [4],
	1040 => [4, 16],
	1044 => [4, 9, 36],
	1048 => [4],
	1050 => [25],
	1052 => [4],
	1053 => [9],
	1056 => [4, 16],
	1060 => [4],
	1062 => [9],
	1064 => [4],
	1068 => [4],
	1071 => [9],
	1072 => [4, 16],
	1075 => [25],
	1076 => [4],
	1078 => [49],
	1080 => [4, 9, 36],
	1084 => [4],
	1088 => [4, 16],
	1089 => [9],
	1092 => [4],
	1096 => [4],
	1098 => [9],
	1100 => [4, 25],
	1104 => [4, 16],
	1107 => [9],
	1108 => [4],
	1112 => [4],
	1116 => [4, 9, 36],
	1120 => [4, 16],
	1124 => [4],
	1125 => [9, 25],
	1127 => [49],
	1128 => [4],
	1132 => [4],
	1134 => [9],
	1136 => [4, 16],
	1140 => [4],
	1143 => [9],
	1144 => [4],
	1148 => [4],
	1150 => [25],
	1152 => [4, 9, 16, 36],
	1156 => [4],
	1160 => [4],
	1161 => [9],
	1164 => [4],
	1168 => [4, 16],
	1170 => [9],
	1172 => [4],
	1175 => [25],
	1176 => [4, 49],
	1179 => [9],
	1180 => [4],
	1184 => [4, 16],
	1188 => [4, 9, 36],
	1192 => [4],
	1196 => [4],
	1197 => [9],
	1200 => [4, 16, 25],
	1204 => [4],
	1206 => [9],
	1208 => [4],
	1212 => [4],
	1215 => [9],
	1216 => [4, 16],
	1220 => [4],
	1224 => [4, 9, 36],
	1225 => [25, 49],
	1228 => [4],
	1232 => [4, 16],
	1233 => [9],
	1236 => [4],
	1240 => [4],
	1242 => [9],
	1244 => [4],
	1248 => [4, 16],
	1250 => [25],
	1251 => [9],
	1252 => [4],
	1256 => [4],
	1260 => [4, 9, 36],
	1264 => [4, 16],
	1268 => [4],
	1269 => [9],
	1272 => [4],
	1274 => [49],
	1275 => [25],
	1276 => [4],
	1278 => [9],
	1280 => [4, 16],
	1284 => [4],
	1287 => [9],
	1288 => [4],
	1292 => [4],
	1296 => [4, 9, 16, 36],
	1300 => [4, 25],
	1304 => [4],
	1305 => [9],
	1308 => [4],
	1312 => [4, 16],
	1314 => [9],
	1316 => [4],
	1320 => [4],
	1323 => [9, 49],
	1324 => [4],
	1325 => [25],
	1328 => [4, 16],
	1332 => [4, 9, 36],
	1336 => [4],
	1340 => [4],
	1341 => [9],
	1344 => [4, 16],
	1348 => [4],
	1350 => [9, 25],
	1352 => [4],
	1356 => [4],
	1359 => [9],
	1360 => [4, 16],
	1364 => [4],
	1368 => [4, 9, 36],
	1372 => [4, 49],
	1375 => [25],
	1376 => [4, 16],
	1377 => [9],
	1380 => [4],
	1384 => [4],
	1386 => [9],
	1388 => [4],
	1392 => [4, 16],
	1395 => [9],
	1396 => [4],
	1400 => [4, 25],
	1404 => [4, 9, 36],
	1408 => [4, 16],
	1412 => [4],
	1413 => [9],
	1416 => [4],
	1420 => [4],
	1421 => [49],
	1422 => [9],
	1424 => [4, 16],
	1425 => [25],
	1428 => [4],
	1431 => [9],
	1432 => [4],
	1436 => [4],
	1440 => [4, 9, 16, 36],
	1444 => [4],
	1448 => [4],
	1449 => [9],
	1450 => [25],
	1452 => [4],
	1456 => [4, 16],
	1458 => [9],
	1460 => [4],
	1464 => [4],
	1467 => [9],
	1468 => [4],
	1470 => [49],
	1472 => [4, 16],
	1475 => [25],
	1476 => [4, 9, 36],
	1480 => [4],
	1484 => [4],
	1485 => [9],
	1488 => [4, 16],
	1492 => [4],
	1494 => [9],
	1496 => [4],
	1500 => [4, 25],
	1503 => [9],
	1504 => [4, 16],
	1508 => [4],
	1512 => [4, 9, 36],
	1516 => [4],
	1519 => [49],
	1520 => [4, 16],
	1521 => [9],
	1524 => [4],
	1525 => [25],
	1528 => [4],
	1530 => [9],
	1532 => [4],
	1536 => [4, 16],
	1539 => [9],
	1540 => [4],
	1544 => [4],
	1548 => [4, 9, 36],
	1550 => [25],
	1552 => [4, 16],
	1556 => [4],
	1557 => [9],
	1560 => [4],
	1564 => [4],
	1566 => [9],
	1568 => [4, 16, 49],
	1572 => [4],
	1575 => [9, 25],
	1576 => [4],
	1580 => [4],
	1584 => [4, 9, 16, 36],
	1588 => [4],
	1592 => [4],
	1593 => [9],
	1596 => [4],
	1600 => [4, 16, 25],
	1602 => [9],
	1604 => [4],
	1608 => [4],
	1611 => [9],
	1612 => [4],
	1616 => [4, 16],
	1617 => [49],
	1620 => [4, 9, 36],
	1624 => [4],
	1625 => [25],
	1628 => [4],
	1629 => [9],
	1632 => [4, 16],
	1636 => [4],
	1638 => [9],
	1640 => [4],
	1644 => [4],
	1647 => [9],
	1648 => [4, 16],
	1650 => [25],
	1652 => [4],
	1656 => [4, 9, 36],
	1660 => [4],
	1664 => [4, 16],
	1665 => [9],
	1666 => [49],
	1668 => [4],
	1672 => [4],
	1674 => [9],
	1675 => [25],
	1676 => [4],
	1680 => [4, 16],
	1683 => [9],
	1684 => [4],
	1688 => [4],
	1692 => [4, 9, 36],
	1696 => [4, 16],
	1700 => [4, 25],
	1701 => [9],
	1704 => [4],
	1708 => [4],
	1710 => [9],
	1712 => [4, 16],
	1715 => [49],
	1716 => [4],
	1719 => [9],
	1720 => [4],
	1724 => [4],
	1725 => [25],
	1728 => [4, 9, 16, 36],
	1732 => [4],
	1736 => [4],
	1737 => [9],
	1740 => [4],
	1744 => [4, 16],
	1746 => [9],
	1748 => [4],
	1750 => [25],
	1752 => [4],
	1755 => [9],
	1756 => [4],
	1760 => [4, 16],
	1764 => [4, 9, 36, 49],
	1768 => [4],
	1772 => [4],
	1773 => [9],
	1775 => [25],
	1776 => [4, 16],
	1780 => [4],
	1782 => [9],
	1784 => [4],
	1788 => [4],
	1791 => [9],
	1792 => [4, 16],
	1796 => [4],
	1800 => [4, 9, 25, 36],
	1804 => [4],
	1808 => [4, 16],
	1809 => [9],
	1812 => [4],
	1813 => [49],
	1816 => [4],
	1818 => [9],
	1820 => [4],
	1824 => [4, 16],
	1825 => [25],
	1827 => [9],
	1828 => [4],
	1832 => [4],
	1836 => [4, 9, 36],
	1840 => [4, 16],
	1844 => [4],
	1845 => [9],
	1848 => [4],
	1850 => [25],
	1852 => [4],
	1854 => [9],
	1856 => [4, 16],
	1860 => [4],
	1862 => [49],
	1863 => [9],
	1864 => [4],
	1868 => [4],
	1872 => [4, 9, 16, 36],
	1875 => [25],
	1876 => [4],
	1880 => [4],
	1881 => [9],
	1884 => [4],
	1888 => [4, 16],
	1890 => [9],
	1892 => [4],
	1896 => [4],
	1899 => [9],
	1900 => [4, 25],
	1904 => [4, 16],
	1908 => [4, 9, 36],
	1911 => [49],
	1912 => [4],
	1916 => [4],
	1917 => [9],
	1920 => [4, 16],
	1924 => [4],
	1925 => [25],
	1926 => [9],
	1928 => [4],
	1932 => [4],
	1935 => [9],
	1936 => [4, 16],
	1940 => [4],
	1944 => [4, 9, 36],
	1948 => [4],
	1950 => [25],
	1952 => [4, 16],
	1953 => [9],
	1956 => [4],
	1960 => [4, 49],
	1962 => [9],
	1964 => [4],
	1968 => [4, 16],
	1971 => [9],
	1972 => [4],
	1975 => [25],
	1976 => [4],
	1980 => [4, 9, 36],
	1984 => [4, 16],
	1988 => [4],
	1989 => [9],
	1992 => [4],
	1996 => [4],
	1998 => [9],
	2000 => [4, 16, 25]
);

foreach my $dim (sort {$a<=>$b} keys %dimensions){
	foreach (@{$dimensions{$dim}}){
		$t = 0;
		while($t < $test_cases){
			
			unless(open $in_file, ">$current_test"){
				die "\nCannot create $current_test\n";
			}
			 $i = 0;
			while($i < $dim){
				$j = 0;
				while($j < $dim){
					$random = rand(100);
					if($random < 100*$density){
						print $in_file "1";
					}
					else{
						print $in_file "0";
					}
					$j = $j + 1;
				
				}
				$i = $i + 1;
				print $in_file "\n";
			}
			print ("-d $dim $dim -g $gen -i $current_test $silent\n");
			system("mpiexec -n $_ ./MPI_CGOL/main -d $dim $dim -g $gen -i $current_test $silent");
			system("mpiexec -n $_ ./OPENMP_CGOL/main -d $dim $dim -g $gen -i $current_test $silent");
			system("./SERIAL_CGOL/main -d $dim $dim -g $gen -i $current_test $silent");
			$t = $t + 1;
			close($in_file);
		}
	}
}
		