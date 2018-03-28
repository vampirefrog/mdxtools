<?php

$sjis = array();
$unicode = array();

$files = $argc < 2 ? array('sjis.txt') : array_slice($argv, 1);
foreach($files as $file) {
	$f = fopen($file, "r");
	if(!$f) die("Could not open $sjisTxt");

	while(!feof($f)) {
		$line = fgets($f);
		if(preg_match('#^0x(?<sjis>[0-9a-f]{2,4})\s+(0x|U\+)(?<unicode>[0-9a-f]{4,})#i', $line, $match)) {
			$sjis[intval($match['sjis'], 16)] = intval($match['unicode'], 16);
			$unicode[intval($match['unicode'], 16)] = intval($match['sjis'], 16);
		}
	}

	fclose($f);
}

echo "#include <stdint.h>\n\n";

generate_function($sjis, 'sjis', 'unicode');
generate_function($unicode, 'unicode', 'sjis');

function generate_function($data, $prefix, $to) {
	ksort($data);

	$ranges = array();

	// fill small gaps
	$prev = 0;
	foreach($data as $code => $unicode) {
		if($code - $prev > 1 && $code - $prev < 15) {
			for($i = $prev+1; $i < $code; $i++)
				$data[$i] = 0x55;
		}
		$prev = $code;
	}

	ksort($data);

	$prev = 0;
	$start = 0;
	$data[99999999] = null; // force end condition for last range
	$total = 0;
	foreach($data as $code => $unicode) {
		if($prev != $code-1) {
			if($code > 0 && $prev > 0) {
				$s = array_search(intval($start), array_keys($data));
				$e = array_search(intval($prev), array_keys($data));
				$l = $e - $s + 1;
				$ranges[] = array(
					'start' => $start,
					'end' => $prev,
					's' => $s,
					'e' => $e,
					'data' => array_slice($data, $s, $l),
					'table_start' => $total,
					'table_end' => $total + $l,
				);
				$total += $l;
			}
			$start = $code;
		}
		$prev = $code;
	}

	$src_width = $prefix == 'unicode' ? 32 : 16;
	$dst_width = $prefix == 'unicode' ? 16 : 32;

	echo "static uint{$dst_width}_t {$prefix}_table[] = {\n";

	foreach($ranges as $range) {
		printf("\t /* 0x%04x - 0x%04x */", $range['start'], $range['end']);
		foreach($range['data'] as $k => $d) {
			if($k % 8 == 0)
				echo "\n\t";
			else echo ' ';
			printf('0x%04x,', $d);
		}
		echo "\n";
	}
	echo "};\n";

	echo "uint{$dst_width}_t {$prefix}_char_to_$to(uint{$src_width}_t $prefix) {\n";
	foreach($ranges as $k => $range) {
		printf("\tif($prefix >= 0x%04x && $prefix <= 0x%04x)", $range['start'], $range['end']);
		printf(" return {$prefix}_table[0x%04x + $prefix - 0x%04x];\n", $range['table_start'], $range['start']);
	}
	if($to == 'sjis')
		echo "\treturn 0x81a6; // ※\n}\n";
	else
		echo "\treturn 0x203b; // ※\n}\n";
}
