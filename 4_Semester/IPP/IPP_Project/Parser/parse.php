<?php

// IPP Projekt cast 1 - soubor parse.php
// Autor: Michal Pysik (login: xpysik00)

// Implementovana rozsireni: STATP

ini_set('display_errors', 'stderr');

$correct_header = '.IPPcode21'; #pozdejsi kontrola hlavicky je case-insensitive (napriklad .IpPCodE21 je v tomto pripade stale validni)

// Funkce pro chybovy vystup a ukonceni programu s danou chybovou navratovou hodnotou
function error_exit($retval, $message)
{
	fwrite(STDERR, "Error: " . $message . "\n" . "The parsing script has been aborted with error code " . $retval. "\n");
	exit($retval);
}

// Funkce pro odstraneni jakychkoli bilych znaku v retezci
function remove_ws($string)
{
	return preg_replace('/\s/', '', $string);
}

// Funkce zkontroluje lexikalni spravnost daneho argumentu, v pripade ze je validni vraci true, v ostatnich pripadech false
// $arg_type <- jaky typ argumentu testujeme (predano funkci jako retezec, napriklad 'bool')
// $argument <- argument jehoz spravnost testujeme
function lex_arg_check($arg_type, $argument)
{
	if($arg_type == 'int')
	{
		return preg_match("/^int@[+-]?[0-9]+$/", $argument);
	}
	elseif($arg_type == 'bool')
	{
		return ($argument == 'bool@true' || $argument == 'bool@false');
	}
	elseif($arg_type == 'string')
	{
		if(substr($argument, 0, 7) == 'string@')
		{
			// tento regularni kontroluje, zdali se zde nevyskytuji zakazane znaky s vyjimkou pro escape sekvence
			return (!preg_match("/(?!\\\\[0-9][0-9][0-9])[[:blank:]\\\\#]/", $argument) || $argument == 'string@'); #take muze byt prazdny retezec
		}
		return false;
	}
	elseif($arg_type == 'nil')
	{
		return ($argument == 'nil@nil');
	}
	elseif($arg_type == 'label')
	{
		return preg_match("/^[[:alpha:]_\-$&%*?!][[:alnum:]_\-$&%*?!]*$/", $argument);
	}
	elseif($arg_type == 'type')
	{
		return ($argument == 'int' || $argument == 'string' || $argument == 'bool' || $argument === 'nil');
	}
	elseif($arg_type == 'var')
	{
		return preg_match("/^[LTG]F@[[:alpha:]_\-$&%*?!][[:alnum:]_\-$&%*?!]*$/", $argument);
	}

	return false;
}

// funkce ktera zjisti zda je dany symbol promenna, integer atd., v pripade ze neni nic z techto moznosti vraci prazdny retezec
function symbol_check($symbol)
{
	$symbol_type = '';

	if(lex_arg_check('var', $symbol)) $symbol_type = 'var';
	elseif(lex_arg_check('int', $symbol)) $symbol_type = 'int';
	elseif(lex_arg_check('bool', $symbol)) $symbol_type = 'bool';
	elseif(lex_arg_check('string', $symbol)) $symbol_type = 'string';
	elseif(lex_arg_check('nil', $symbol)) $symbol_type = 'nil';

	return $symbol_type;
}


// Funkce vraci true/false podle toho, zda je retezec predany jako argument platny kod operace
function check_opcode_dictionary($word)
{
	static $opcode_dictionary = array('MOVE', 'CREATEFRAME', 'PUSHFRAME', 'POPFRAME', 'DEFVAR', 'CALL', 'RETURN', 'PUSHS', 'POPS', 'ADD', 'SUB', 'MUL', 'IDIV', 'LT', 'GT', 'EQ',
	'AND', 'OR', 'NOT', 'INT2CHAR', 'STRI2INT', 'READ', 'WRITE', 'CONCAT', 'STRLEN', 'GETCHAR', 'SETCHAR', 'TYPE', 'LABEL', 'JUMP', 'JUMPIFEQ', 'JUMPIFNEQ', 'EXIT', 'DPRINT', 'BREAK');

	return in_array($word, $opcode_dictionary);
}


// trida pro jednotlive statistiky (STATP)
class Stat
{
	public int $cnt = 0; #pocitadlo vyskytu dane statistiky
	public string $fname = ''; #nazev souboru, kam se dana statistika vypise
	public $rows = array(); #na jake radeky vybraneho souboru se vypise, indexovano od 1
}

// tvorba jejich instanci a umisteni do asociativniho pole (klic je jejich nazev, instance tridy je samotna statistika)
$statsArr = array(
	'loc' => $loc = new Stat(),
	'comments' => $comments = new Stat(),
	'labels' => $labels = new Stat(),
	'jumps' => $jumps = new Stat(),
	'fwjumps' => $fwjumps = new Stat(),
	'backjumps' => $backjumps = new Stat(),
	'badjumps' => $badjumps = new Stat()
);

// knihovna moznych argumentu (krome help, ten je osetren zvlast)
$statOpLibrary = array("--loc", "--comments", "--labels", "--jumps", "--fwjumps", "--backjumps", "--badjumps",
						"-loc", "-comments", "-labels", "-jumps", "-fwjumps", "-backjumps", "-badjumps");

// pripadne pomocne extra pole pro stats, jelikoz jsem puvodne spatne pochopil zadani
$extra_stats = array(); #format: nazev_statu/nazev_souboru/cislo_radku

// Kontrola parametru prikazove radky
if($argc == 2)
{
	if($argv[1] == "--help" || $argv[1] == "-help")
	{
		echo "----------------------HELP----------------------\nThis script parses code written in IPPcode21 read from the standard input, into it's XML representation written to the standard output.\n";
		echo "\nList of valid parameters:\n--help <-- Displays help\n";
		echo "--stats=\"file\" (--arg_1 ... --arg_n) <-- collects selected statistics to selected file, read the IPP21 specification for more information\n";
		echo "------------------------------------------------\n";
		exit(0);
	}
	elseif( substr($argv[1], 0, 8) == '--stats=' || substr($argv[1], 0, 7) == '-stats=' )
	{
		if( $file_name = substr($argv[1], strpos($argv[1], "=") + 1) ) #stats jen vytvori prazdny soubor, musi vsak mit nazev
		{
			$empty_file = fopen($file_name, "w"); #specialni case kdy se ma podle zadani jen otevrit a zavrit soubor
			fclose($empty_file);
		}
		else error_exit(10, "Filename for STATP extension not specified");
	}
	else
	{
		error_exit(10, "Invalid option or missing option parameter(s), run the script with only \"--help\" option to see the correct usage");
	}
}
elseif($argc > 2)
{
	$offset = 0; #pomuze pri pouziti vice souboru pro statistiky

	if( substr($argv[1], 0, 8) == '--stats=' || substr($argv[1], 0, 7) == '-stats=' )
	{
		if( $file_name = substr($argv[1], strpos($argv[1], "=") + 1) ) #prvni vyskyt stats musi obsahovat nazev souboru
		{
			for($i = 2; $i < $argc; $i++)
			{
				if( in_array($argv[$i], $statOpLibrary) ) #je platny parametr
				{
					$tmp = str_replace('-', '', $argv[$i]);

					if( $statsArr[$tmp]->fname && $statsArr[$tmp]->fname != $file_name )
					{
						$extra_stats[] = $tmp . '/' . $file_name . '/' . ($i-$offset-1);
					}
					else
					{
						$statsArr[$tmp]->fname = $file_name;
						$statsArr[$tmp]->rows[] = $i-$offset-1;
					}
				}
				elseif( substr($argv[$i], 0, 8) == '--stats=' || substr($argv[$i], 0, 7) == '-stats=' ) #narazil na dalsi --stats=
				{
					if( substr($argv[$i], strpos($argv[$i], "=") + 1) && $file_name != substr($argv[$i], strpos($argv[$i], "=") + 1) )
					{
						$empty_file = fopen($file_name, "w"); #specialni case znovu, tentokrat za stats nasleduje dalsi stats
						fclose($empty_file);

						$file_name = substr($argv[$i], strpos($argv[$i], "=") + 1);
						$offset = $i-1; #abychom nemuseli tvorit novy for cyklus, pouze nastavime spravnou odchylku
						if($i == $argc-1)
						{
							$empty_file = fopen($file_name, "w"); #specialni case znovu, tentokrat je to posledni z vicera --stats za kterym nic neni
							fclose($empty_file);
						}
					}
					else if($file_name == substr($argv[$i], strpos($argv[$i], "=") + 1)) error_exit(12, "Filename(s) for STATP extension cannot be the same");
					else error_exit(10, "Filename(s) for STATP extension not specified");
				}
				else error_exit(10, "Invalid arguments for stats");
			}
		}
		else error_exit(10, "Filename(s) for STATP extension not specified");
	}
	else error_exit(10, "Invalid options combination, run the script with only \"--help\" option to see the correct usage");
}



// priprava vystupniho XML souboru
$xml = new XMLWriter();
$xml->openMemory();
$xml->setIndent(1);
$xml->setIndentString(' ');

$xml->startDocument('1.0', 'UTF-8');
$xml->startElement('program');
$xml->startAttribute('language');
$xml->text('IPPcode21');
$xml->endAttribute();



// Test povinne IPPcode hlavicky
$line = " "; #abychom nemuseli samostatne osetrovat pro prazdny soubor

while( ctype_space($line) || substr(trim($line), 0, 1) == '#' ) #ignorujeme radky pouze s whitespace a komentarema
{
	if( $statsArr['comments']->fname && substr(trim($line), 0, 1) == '#' ) $statsArr['comments']->cnt++; #pokud je radek komentar a sbirame o nich statistiku
	$line = fgets(STDIN);
	if(!$line)
	{
		error_exit(21, "Input file must contain \".IPPcode21\" header on the first line (excluding whitespace and comment lines)");
	}
}

$cut_header = substr(remove_ws($line), 0, strlen($correct_header)+1); #narazili jsme na radek kde ocekavame hlavicku, pripadne nasledovanou komentarem, whitespace ignorujeme

if(strcasecmp($cut_header, $correct_header) && strcasecmp($cut_header ,$correct_header.'#')) #velikost pismen zanedbame
{
	error_exit(21, "Input file must contain \".IPPcode21\" header on the first line (with exception for whitespace/comment only lines)");
}

if( $statsArr['comments']->fname && substr($cut_header, -1) == '#' ) $statsArr['comments']->cnt++; #pocitame mozny komentar za hlavickou



$labelList = array(); #seznam vsech nalezenych unikatnich labelu (STATP - labels)
$labelStrictList = array(); #seznam pouze navesti vytvorenych instrukci label (STATP - pro fw/back/badjumps)
$bbjList = array(); #seznam skoku co muzou byt bad nebo forward


$arg1type = $arg2type = $arg3type = 'none'; #prvni inicializace

$order_counter = 0; #pocitadlo instrukci

// Cteni jednotlivych instrukci
while($line = fgets(STDIN))
{
	if(ctype_space($line)) continue; #opet radky pouze s whitespace ignorujeme
	elseif(substr(trim($line), 0, 1) == '#')
	{
		if($statsArr['comments']->fname) $statsArr['comments']->cnt++;
		continue;
	}
	else
	{
		$arg1type = $arg2type = $arg3type = 'none';

		if($statsArr['comments']->fname) #pripadne pocitani komentare za instrukci
		{
			if(strpos($line, '#')) $statsArr['comments']->cnt++;
		}

		$line = explode('#', $line)[0]; #odstranime pripadny komentar za instrukcemi

		$tmpwrds = preg_split('/\s+/', $line); #rozdelime radek na slova
		$words = array_values(array_filter($tmpwrds)); #odfiltrujeme prazdna slova, vnejsi funkce rekalibruje indexovani

		$wc = count($words);

		$instruction = strtoupper($words[0]);

		$xml->startElement('instruction');
		$xml->startAttribute('order');
		$xml->text(++$order_counter);
		$xml->endAttribute();
		$xml->startAttribute('opcode');
		$xml->text($instruction);
		$xml->endAttribute();

		// Switch na zaklade poctu jednotlivych slov na radku (povoleno 1 az 4, komentare se nepocitaji)
		switch($wc)
		{
			case 1:
				switch($instruction)
				{
					case 'CREATEFRAME':
					case 'PUSHFRAME':
					case 'POPFRAME':
					case 'RETURN':
					case 'BREAK':
						break;

					default: #kontrola zda uzivatel zadal validni instrukci se spatnym poctem argumentu, nebo kompletne neplatny kod instrukce
						if(check_opcode_dictionary($instruction)) error_exit(23, "Instruction expects different number of arguments " . "(instruction " . $order_counter .": " . $instruction . ")");
						else error_exit(22, "Invalid instruction operation code (instruction " . $order_counter .": \"" . $instruction . "\")");
						break;
				}
				break;

			case 2:
				switch($instruction)
				{
					case 'DEFVAR': #ocekavaji <var>
					case 'POPS':
						if(lex_arg_check('var', $words[1])) $arg1type = 'var';
						else error_exit(23, "Invalid variable name passed as argument " . "(instruction " . $order_counter .": " . $instruction . ")");
						break;

					case 'CALL': #ocekavaji <label>
					case 'JUMP':
						if(lex_arg_check('label', $words[1])) $arg1type = 'label';
						else error_exit(23, "Invalid label name passed as argument " . "(instruction " . $order_counter .": " . $instruction . ")");
						if( $statsArr['fwjumps']->fname || $statsArr['backjumps']->fname || $statsArr['badjumps']->fname )
						{
							if( in_array($words[1], $labelStrictList) ) $statsArr['backjumps']->cnt++; #pokud label uz existuje prida se do backjumps
							else $bbjList[] = $words[1]; #jinak se daji do listu bad a fw jumpu
						}
						break;

					case 'LABEL': #ocekava <label>
						if(lex_arg_check('label', $words[1])) $arg1type = 'label';
						else error_exit(23, "Invalid label name passed as argument " . "(instruction " . $order_counter .": " . $instruction . ")");
						if($statsArr['fwjumps']->fname || $statsArr['backjumps']->fname || $statsArr['badjumps']->fname) $labelStrictList[] = $words[1];
						break;

					case 'PUSHS': #ocekavaji <symb>
					case 'WRITE':
					case 'EXIT':
					case 'DPRINT':
						$arg1type = symbol_check($words[1]);
						if(!$arg1type) error_exit(23, "Invalid first argument, must be either constant or variable " . "(instruction " . $order_counter .": " . $instruction . ")");
						break;

					default: #kontrola zda uzivatel zadal validni instrukci se spatnym poctem argumentu, nebo kompletne neplatny kod instrukce
						if(check_opcode_dictionary($instruction)) error_exit(23, "Instruction expects different number of arguments " . "(instruction " . $order_counter .": " . $instruction . ")");
						else error_exit(22, "Invalid instruction operation code (instruction " . $order_counter .": \"" . $instruction . "\")");
						break;
				}
				break;

			case 3:
				switch($instruction)
				{
					case 'MOVE': #ocekavaji <var> <symb>
					case 'TYPE':
					case 'INT2CHAR':
					case 'STRLEN':
					case 'NOT':
						if(lex_arg_check('var', $words[1])) $arg1type = 'var';
						else error_exit(23, "Invalid variable name passed as first argument " . "(instruction " . $order_counter .": " . $instruction . ")");
						$arg2type = symbol_check($words[2]);
						if(!$arg2type) error_exit(23, "Invalid second argument, must be either constant or variable " . "(instruction " . $order_counter .": " . $instruction . ")");
						break;

					case 'READ': #ocekava <var> <type>
						if(lex_arg_check('var', $words[1])) $arg1type = 'var';
						else error_exit(23, "Invalid variable name passed as first argument " . "(instruction " . $order_counter .": " . $instruction . ")");
						if(lex_arg_check('type', $words[2])) $arg2type = 'type';
						else error_exit(23, "Invalid type name passed as second argument " . "(instruction " . $order_counter .": " . $instruction . ")");
						break;

					default:
						if(check_opcode_dictionary($instruction)) error_exit(23, "Instruction expects different number of arguments " . "(instruction " . $order_counter .": " . $instruction . ")");
						else error_exit(22, "Invalid instruction operation code (instruction " . $order_counter .": \"" . $instruction . "\")");
						break;
				}
				break;

			case 4:
				switch($instruction)
				{
					case 'ADD': #ocekavaji <var> <symb> <symb>
					case 'SUB':
					case 'MUL':
					case 'IDIV':
					case 'LT':
					case 'GT':
					case 'EQ':
					case 'AND':
					case 'OR':
					case 'STRI2INT':
					case 'GETCHAR':
					case 'CONCAT':
					case 'SETCHAR':
						if(lex_arg_check('var', $words[1])) $arg1type = 'var';
						else error_exit(23, "Invalid variable name passed as first argument " . "(instruction " . $order_counter .": " . $instruction . ")");
						$arg2type = symbol_check($words[2]);
						if( !$arg2type ) error_exit(23, "Invalid second argument, must be either constant or variable " . "(instruction " . $order_counter .": " . $instruction . ")");
						$arg3type = symbol_check($words[3]);
						if( !$arg3type ) error_exit(23, "Invalid third argument, must be either constant or variable " . "(instruction " . $order_counter .": " . $instruction . ")");
						break;

					case 'JUMPIFEQ': #ocekavaji <label> <symb1> <symb2>, symboly jsou stejneho typu nebo je alespon jeden z nich nil
					case 'JUMPIFNEQ':
						if(lex_arg_check('label', $words[1])) $arg1type = 'label';
						else error_exit(23, "Invalid label name passed as first argument " . "(instruction " . $order_counter .": " . $instruction . ")");
						$arg2type = symbol_check($words[2]);
						if( !$arg2type ) error_exit(23, "Invalid second argument, must be either constant or variable " . "(instruction " . $order_counter .": " . $instruction . ")");
						$arg3type = symbol_check($words[3]);
						if( !$arg3type ) error_exit(23, "Invalid third argument, must be either constant or variable " . "(instruction " . $order_counter .": " . $instruction . ")");
						break;

					default:
						if(check_opcode_dictionary($instruction)) error_exit(23, "Instruction expects different number of arguments " . "(instruction " . $order_counter .": " . $instruction . ")");
						else error_exit(22, "Invalid instruction operation code (instruction " . $order_counter .": \"" . $instruction . "\")");
						break;
				} // konec switche pro nadrazeny case 4
				break;


			// pri neuspechu zjistime zda je instrukce platna jen se spatnym poctem argumentu, nebo je kompletne neplatna
			default:
				if(check_opcode_dictionary($instruction)) error_exit(23, "Instruction expects less arguments " . "(instruction " . $order_counter .": " . $instruction . ")");
				else error_exit(22, "Invalid instruction operation code (instruction " . $order_counter .": \"" . $instruction . "\")");
				break;
		} // konec hlavniho switche podle poctu slov



		$index = 0;
		// vsechny argumenty jsou jiz zkontrolovany, nyni se pouze vypisou
		foreach(array($arg1type, $arg2type, $arg3type) as $argtype)
		{
			if($argtype == 'none') break;

			++$index;

			if($argtype == 'int' || $argtype == 'bool' || $argtype == 'string' || $argtype == 'nil') $words[$index] = explode('@', $words[$index], 2)[1]; #odstraneni casti pred zavinacem (vcetne) u literalu
			$xml->startElement('arg' . $index);
			$xml->startAttribute('type');
			$xml->text($argtype);
			$xml->endAttribute();
			$xml->text($words[$index]);
			$xml->endElement();

			if( $statsArr['labels']->fname && $argtype == 'label' ) #pokud sbirame statistiky o navestich adany argument je navesti, zjistime zda je nove a pokud ano inkrementujeme cnt
			{
				if(!in_array($words[$index], $labelList))
				{
					$labelList[] = $words[$index];
					$statsArr['labels']->cnt++;
				}
			}
		}

		$xml->endElement(); #konec vypisu instrukce vcetne jejich atributu


		// Osetreni pripadnych statistik pro obecne skoky
		if($statsArr['jumps']->fname)
		{
			if($instruction == 'JUMP' || $instruction == 'JUMPIFEQ' || $instruction == 'JUMPIFNEQ' || $instruction == 'CALL' || $instruction == 'RETURN') $statsArr['jumps']->cnt++;
		}



	} // konec else (radek obsahujici instrukci)
} // konec while



if($statsArr['loc']->fname) $statsArr['loc']->cnt = $order_counter; #pokud sbirame statistiku o poctu instrukci, jen zde zapiseme konecny order_counter

if( $statsArr['fwjumps']->fname || $statsArr['backjumps']->fname || $statsArr['badjumps']->fname ) #pokud sbirame fw/bad jumpy, az nyni je odlisujeme
{
	foreach($bbjList as $suspect)
	{
		if(in_array($suspect, $labelStrictList)) $statsArr['fwjumps']->cnt++;
		else $statsArr['badjumps']->cnt++;
	}
}



// zavreni dokumentu a jeho tisk na standardni vystup
$xml->endElement();
$xml->endDocument();
fwrite(STDOUT, $xml->outputMemory());

$opened_files = array();

// zjistime zda uzivatel chce vypis statistik a otevreme si soubory
$stats = false;
foreach($statsArr as $key => $stat)
{
	if($stat->fname)
	{
		$stats = true;

		if(!in_array($stat->fname, $opened_files)) #pokud dany soubor jeste nebyl otevren
		{
			$tmp_ptr = fopen($stat->fname, "w");
			$opened_files[$stat->fname] = $tmp_ptr;
		}
	}
}

if($extra_stats)
{
	for($i = 0; $i < count($extra_stats); $i++)
	{
		$extra_fname = explode('/', $extra_stats[$i])[1];
		if(!in_array($extra_fname, $opened_files)) #pokud dany soubor jeste nebyl otevren
		{
			$tmp_ptr = fopen($extra_fname, "w");
			$opened_files[$extra_fname] = $tmp_ptr;
		}
	}
}

// Pokud uzivatel pouziva statistiky, ma tudiz otevrene soubory kam se maji vypsat
if($stats)
{
	for($i = 1; $i <= count($statOpLibrary)/2; $i++) #vsechny mozne stat argumenty jsou v teto "knihovne" ve dvou variantach (- nebo --)
	{
		foreach($statsArr as $key => $stat)
		{
			if(in_array($i, $stat->rows))
			{
				fwrite($opened_files[$stat->fname], $stat->cnt . "\n");
			}
		}

		if($extra_stats) #v pripade stejne statistiky ve vice souborech je to slozite osetrene
		{
			foreach($extra_stats as $extra_line)
			{
				if( explode('/', $extra_line)[2] == $i )
				{
					fwrite($opened_files[ explode('/', $extra_line)[1] ], $statsArr[ explode('/', $extra_line)[0] ]->cnt . "\n");
				}
			}
		}

	}

	foreach($opened_files as $fkey => $fptr) fclose($fptr); #zavreni vsech otevrenych souboru
}



exit(0);

?>
