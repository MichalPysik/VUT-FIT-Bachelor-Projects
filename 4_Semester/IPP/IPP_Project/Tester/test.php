<?php
// IPP Projekt cast 2.2 - Test.php
// Autor: Michal Pysik (login: xpysik00)

ini_set('display_errors', 'stderr');


// Funkce pro chybovy vystup a ukonceni programu s danou chybovou navratovou hodnotou
function error_exit($retval, $message)
{
	fwrite(STDERR, "Error: " . $message . "\n" . "The testing script has been aborted with error code " . $retval. "\n");
	exit($retval);
}


// Funkce vraci pole obsahujici nazvy vsech souboru ve slozce, parametr $recursive je boolean, znaci ci chceme rekurzivne prochazet podadresare
function getFnames($dir, $recursive)
{
	$file_names = array();

	// otevreni slozky s testama
	$dir_handle = @opendir($dir);
	if(!is_resource($dir_handle))
	{
		error_exit(41, "Could not open selected directory containing test files");
	}

	// cteme jednotlive nazvy souboru
	while($fname = readdir($dir_handle))
	{
		if($fname == '.' || $fname == '..') continue;
		elseif(is_dir($dir . $fname))
		{
			if($recursive) $file_names = array_merge($file_names, getFnames($dir . $fname . '/',true));
		}
		else array_push($file_names, $dir . $fname);
	}

	closedir($dir_handle);

	return $file_names;
}



// defaultne se predpoklada ze se testy nachazi v aktualnim adresari
$testsDir = './';
// prepinac pro rekurzivni prochazeni adresare je defaultne vypnuty
$recursive = false;
// defaultne se predpoklada ze se parse.php skript nachazi v aktualnim adresari
$parseScript = './parse.php';
// defaultne se predpoklada ze se interpret nachazi v aktualnim adresari
$interpretScript = './interpret.py';
// defaultne se ocekavaji testy obou skriptu
$parseOnly = false;
$interpretOnly = false;
// defaultni umisteni JExamXML JAR balicku na Merlinovi
$jexamJAR = '/pub/courses/ipp/jexamxml/jexamxml.jar';
// defaultni umisteni konfigurace JExamXML na Merlinovi
$jexamConfig = '/pub/courses/ipp/jexamxml/options';


// validni parametry co skript prijima
$validOps = array("help", "directory:", "recursive", "parse-script:", "int-script:", "parse-only", "int-only", "jexamxml:", "jexamcfg:");


// nacteni parametru a jejich zpracovani
$usedOps = getopt(null, $validOps);

if(isset( $usedOps["help"] ))
{
	if($argc == 2)
	{
		echo "This script tests the parse.php and interpret.py scripts and generates overview in HTML format to the standard output\n-------------------------\nList of valid parameters:\n-------------------------\n";
		echo "--help <- Prints help (this one)\n";
		echo "--directory=path <- Searches for test files in selected path\n";
		echo "--recursive <- Searches for test files recursively\n";
		echo "--parse-script=file <- Specifies parse.php script location\n";
		echo "--int-script=file <- Specifies interpret.py script location\n";
		echo "--parse-only <- Only parse.php script will be tested\n";
		echo "--int-only <- Only interpret.py script will be tested\n";
		echo "--jexamxml=file <- Specifies JExamXML JAR package location\n";
		echo "--jexamcfg=file <- Specifies JExamXML config location\n";
		exit(0);
	}
	else
	{
		error_exit(10, "Cannot combine parameter --help with other parameters");
	}
}

if(isset( $usedOps["directory"] ))
{
	$testsDir = $usedOps["directory"];
	if (substr($testsDir, -1) != '/') $testsDir = $testsDir . '/'; // uzivatel obcas po nazvu cilove slozky nenapise '/', avsak pro spravnou funkci rekurzivniho prohledavani je potreba
}

if(isset( $usedOps["recursive"] ))
{
	$recursive = true;
}

if(isset( $usedOps["parse-script"] ))
{
	$parseScript = $usedOps["parse-script"];
}

if(isset( $usedOps["int-script"] ))
{
	$interpretScript = $usedOps["int-script"];
}

if(isset( $usedOps["parse-only"] ))
{
	$parseOnly = true;
}

if(isset( $usedOps["int-only"] ))
{
	$interpretOnly = true;
}

if(isset( $usedOps["jexamxml"] ))
{
	$jexamJAR = $usedOps["jexamxml"];
}

if(isset( $usedOps["jexamcfg"] ))
{
	$jexamConfig = $usedOps["jexamcfg"];
}



// osetreni nekterych zakazanych kombinaci parametru
if($parseOnly && ($interpretOnly || $interpretScript != './interpret.py'))
{
	error_exit(10, "Cannot combine --parse-only parameter with --int-only or --int-script parameters");
}
elseif($interpretOnly && $parseScript != './parse.php')
{
	error_exit(10, "Cannot combine --int-only parameter with --parse-only or --parse-script parameters");
}

// kontrola ze soubory vybrane v parametrech existuji a jsou readable
if(!file_exists($parseScript) && !$interpretOnly) error_exit(41, "The selected parse.php file does not exist");
elseif(!is_readable($parseScript) && !$interpretOnly) error_exit(41, "Insufficient permission to access selected parse.php file");

if(!file_exists($interpretScript) && !$parseOnly) error_exit(41, "The selected interpret.py file does not exist");
elseif(!is_readable($interpretScript) && !$parseOnly) error_exit(41, "Insufficient permission to access selected interpret.py file");

if(!file_exists($jexamJAR) && $parseOnly) error_exit(41, "The selected JexamXML JAR package does not exist");
elseif(!is_readable($jexamJAR) && $parseOnly) error_exit(41, "Insufficient permission to access selected JexamXML JAR package");

if(!file_exists($jexamConfig) && $parseOnly) error_exit(41, "The selected JexamXML configuration file does not exist");
elseif(!is_readable($jexamConfig) && $parseOnly) error_exit(41, "Insufficient permission to access selected JexamXML configuration file");



// nazvy (i s cestou) vsech souboru
$file_names = getFnames($testsDir, $recursive);


// nazvy pouze souboru src bez koncovky
$src_names = array();
foreach($file_names as $fname)
{
	if (substr($fname, -4) == '.src') array_push($src_names, substr($fname, 0, -4)); //pokud je to src soubor, ulozime jeho nazev bez koncovky
}

$results = array(); // asociativni pole kde je klicem nazev testu, hodnota udava zda byl uspesny
$totalTests = 0;
$passedTests = 0;


// Testujeme pouze parse.php
if($parseOnly)
{
	foreach($src_names as $test_name)
	{
		$totalTests++;
		$success = false;

		// $trash bude vzdy prazdny (z nejakeho duvodu nefunguje parametr null), v $retval je nav. hodnota, v docasnem tmp souboru je output
		exec('php7.4 ' . $parseScript . ' < ' . $test_name . '.src > ' . $test_name . '.tmp 2> /dev/null', $trash, $retval);

		if(!in_array($test_name . '.rc', $file_names)) //pokud neexistuje soubor s navratovou hodnotou tak vytvorime novy s textem "0" uvnitr
		{
			$fptr = fopen($test_name . '.rc', "w");
			if(!$fptr) error_exit(12, "Could not create file with default return value 0: " . $test_name . '.rc');
			fwrite($fptr, "0");
			fclose($fptr);
			$correct_retval = 0;
		}
		else //pokud existuje tak zjistime jakou hodnotu obsahuje
		{
			$fptr = fopen($test_name . '.rc', "r");
			if(!$fptr) error_exit(11, "Could not open existing .rc file: " . $test_name . '.rc');
			$correct_retval = intval(fread($fptr, 8));
			fclose($fptr);
		}

		if(!in_array($test_name . '.out', $file_names))
		{
			$fptr = fopen($test_name . '.out', "w");
			if(!$fptr) error_exit(12, "Could not create empty .out file: " . $test_name . '.out');
			fclose($fptr);
		}

		if(!in_array($test_name . '.in', $file_names)) //NEVIM JESTLI MAM U PARSE-ONLY VYTVARET I .in SOUBOR, ALE RADSI HO UDELAM
		{
			$fptr = fopen($test_name . '.in', "w");
			if(!$fptr) error_exit(12, "Could not create empty .in file: " . $test_name . '.in');
			fclose($fptr);
		}

		if($retval == $correct_retval)
		{
			$success = true; //pokud maji stejnou navratovou hodnotu tak to byl uspech, avsak pokud je zaroven 0 musime zkontrolovat i XML soubory
			if($retval == 0)
			{
				exec('java -jar ' . $jexamJAR . ' ' . $test_name . '.tmp ' . $test_name . '.out ' . $test_name . ".deltaxml " . $jexamConfig, $trash, $xmlDiff);
				if($xmlDiff) $success = false; // xml soubory jsou odlisne
				exec('rm -f ' . $test_name . '.deltaxml');
			}
		}

		$results[$test_name] = $success;
		if($success) $passedTests++;

		exec('rm -f ' . $test_name . '.tmp'); //odstraneni docasneho souboru s vystupem (xml vystup parse.php)
	}
}

// Testujeme pouze interpret.py
elseif($interpretOnly)
{
	foreach($src_names as $test_name)
	{
		$totalTests++;
		$success = false;

		if(!in_array($test_name . '.in', $file_names))
		{
			$fptr = fopen($test_name . '.in', "w");
			if(!$fptr) error_exit(12, "Could not create empty .in file: " . $test_name . '.in');
			fclose($fptr);
		}

		// spusteni interpretu
		exec('python3.8 ' . $interpretScript . ' --source=' . $test_name . '.src --input=' . $test_name . '.in > ' . $test_name . '.tmp 2> /dev/null', $trash, $retval);

		if(!in_array($test_name . '.rc', $file_names)) //pokud neexistuje soubor s navratovou hodnotou tak vytvorime novy s textem "0" uvnitr
		{
			$fptr = fopen($test_name . '.rc', "w");
			if(!$fptr) error_exit(12, "Could not create file with default return value 0: " . $test_name . '.rc');
			fwrite($fptr, "0");
			fclose($fptr);
			$correct_retval = 0;
		}
		else
		{
			$fptr = fopen($test_name . '.rc', "r");
			if(!$fptr) error_exit(11, "Could not open existing .rc file: " . $test_name . '.rc');
			$correct_retval = intval(fread($fptr, 8));
			fclose($fptr);
		}

		if(!in_array($test_name . '.out', $file_names))
		{
			$fptr = fopen($test_name . '.out', "w");
			if(!$fptr) error_exit(12, "Could not create empty .out file: " . $test_name . '.out');
			fclose($fptr);
		}

		if($retval == $correct_retval)
		{
			$success = true; //pokud maji stejnou navratovou hodnotu tak to byl uspech, avsak pokud je zaroven 0 musime zkontrolovat vystupy interpretu
			if($retval == 0)
			{
				exec('diff ' . $test_name . '.out ' . $test_name . '.tmp', $trash, $difference);
				if($difference) $success = false;
			}
		}

		$results[$test_name] = $success;
		if($success) $passedTests++;

		exec('rm -f ' . $test_name . '.tmp'); //odstraneni docasneho souboru s vystupem (xml vystup parse.php)
	}
}

// Testujeme oba skripty
else
{
	foreach($src_names as $test_name)
	{
		$totalTests++;
		$success = false;

		// $trash bude vzdy prazdny (z nejakeho duvodu nefunguje parametr null), v $retval je nav. hodnota, v docasnem tmp1 souboru je output parse.php
		exec('php7.4 ' . $parseScript . ' < ' . $test_name . '.src > ' . $test_name . '.tmp1 2> /dev/null', $trash, $retvalParse);

		if(!in_array($test_name . '.rc', $file_names)) //pokud neexistuje soubor s navratovou hodnotou tak vytvorime novy s textem "0" uvnitr
		{
			$fptr = fopen($test_name . '.rc', "w");
			if(!$fptr) error_exit(12, "Could not create file with default return value 0: " . $test_name . '.rc');
			fwrite($fptr, "0");
			fclose($fptr);
			$correct_retval = 0;
		}
		else //pokud existuje tak zjistime jakou hodnotu obsahuje
		{
			$fptr = fopen($test_name . '.rc', "r");
			if(!$fptr) error_exit(11, "Could not open existing .rc file: " . $test_name . '.rc');
			$correct_retval = intval(fread($fptr, 8));
			fclose($fptr);
		}

		if(!in_array($test_name . '.out', $file_names))
		{
			$fptr = fopen($test_name . '.out', "w");
			if(!$fptr) error_exit(12, "Could not create empty .out file: " . $test_name . '.out');
			fclose($fptr);
		}

		if(!in_array($test_name . '.in', $file_names)) //NEVIM JESTLI MAM U PARSE-ONLY VYTVARET I .in SOUBOR, ALE RADSI HO UDELAM
		{
			$fptr = fopen($test_name . '.in', "w");
			if(!$fptr) error_exit(12, "Could not create empty .in file: " . $test_name . '.in');
			fclose($fptr);
		}

		if($retvalParse == 0) // parse.php probehl uspesne
		{
			exec('python3.8 ' . $interpretScript . ' --source=' . $test_name . '.tmp1 --input=' . $test_name . '.in > ' . $test_name . '.tmp2 2> /dev/null', $trash, $retval);

			if($retval == $correct_retval) // navratova hodnota interpret.py je spravna
			{
				$success = true;
				if($retval == 0) // navratova hodnota interpretu je spravna a zaroven 0
				{
					exec('diff ' . $test_name . '.out ' . $test_name . '.tmp2', $trash, $difference);
					if($difference) $success = false;
				}
			}
		}
		elseif($retvalParse == $correct_retval) $success = true; // pokud parse.php vratil chybu, zkontroluju zda opovida ocekavane return value

		$results[$test_name] = $success;
		if($success) $passedTests++;

		exec('rm -f ' . $test_name . '.tmp1'); //odstraneni docasnych souboru s vystupy obou skriptu
		exec('rm -f ' . $test_name . '.tmp2');
	}
}

$failedTests = $totalTests - $passedTests;

// Ulozeni informaci o tom kolik testu proslo v jednotlivych slozkach
$folder_total = array();
$folder_passed = array();

foreach($results as $test => $passed)
{
	$test_folder = pathinfo($test)['dirname'];

	if( !array_key_exists($test_folder, $folder_total) )
	{
		$folder_total[$test_folder] = 1;
		$folder_passed[$test_folder] = 0;
	}
	else ++$folder_total[$test_folder];

	if($passed) ++$folder_passed[$test_folder];
}

ksort($folder_total, SORT_NATURAL); # serazeni slozek

// TVORBA HTML SOUBORU NA STDOUT
echo "<!doctype html>\n<html>\n<head>\n<title>Test Results</title>\n";
echo "<style>\nbody {max-width:1000px;margin:auto;}\nh1 {padding-bottom:30px;}\nh2 {padding-bottom:20px;}\nh3 {padding-top:30px;}\np {font-size:15pt;padding-left:15px}\n</style>\n</head>\n";
echo "<body>\n";
if($parseOnly) echo "<h1>Results of parse.php script tests</h1>\n";
elseif($interpretOnly) echo "<h1>Results of interpret.py script tests</h1>\n";
else echo "<h1>Results of parse.php and interpret.py combined scripts tests</h1>\n";
echo "<h2>Total number of tests: {$totalTests}<br><br>Number of tests passed: <em style=\"color:green;\">{$passedTests}</em><br><br>Number of tests failed: <em style=\"color:red;\">{$failedTests}</em></h2>\n";

foreach($folder_total as $folder => $folder_total_tests)
{
	$passed_fold = $folder_passed[$folder];
	$failed_fold = $folder_total_tests - $passed_fold;

	echo "<h3>Folder: {$folder} - {$folder_total_tests} tests - <em style=\"color:green\">{$passed_fold}</em> passed, <em style=\"color:red\">{$failed_fold}</em> failed</h3>";

	$tmp_arr = array();

	//zjistit jake soubory do dane slozky patri, ulozeni jich do docasneho pole
	foreach($results as $test => $passed)
	{
		if( pathinfo($test)['dirname'] == $folder)
		{
			$tname = basename($test);
			$tmp_arr[$tname] = $passed;
		}
	}

	ksort($tmp_arr, SORT_NATURAL); //serazeni souboru v dane slozce

	foreach($tmp_arr as $test_name => $test_passed)
	{
		if($test_passed) echo "<p>{$test_name} : <em style=\"color:green;\">Passed</em></p>\n";
		else echo "<p>{$test_name} : <em style=\"color:red;\">Failed</em></p>\n";
	}
}

echo "</body>\n</html>\n";



exit(0);

?>
