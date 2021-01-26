<html>
<head>
<title>calibrate</title>
</head>
<body>
</body>

<h1>Meat thermometer probe calibration</h1>
<p>
<ul>
<li>Get working thermometer which can measure temperatures at calibration range</li>
<li>Pour hot water in a bowl. Put both probes in water and check the water temperature from working thermometer</li>
<li>Fill the measured temperature in form and select 'measure'</li>
<li>Cool down the water to significantly lower temperature, fill in the new temperature and press 'measure' again</li>
<li>Fill bowl with cold water, fill in last temperature and press 'measure'</li>
<li>When you have measured all three temperature/resistance pairs the calibration is saved</li>
</ul>
<p>
<h3>Guide to select calibration temperatures</h3>
<ul>
<li>Two of the values should be from the extremes of temperature range you think food will be. Good values could be 10 C and 80 C</li>
<li>The mid value could be somewhere in between. Preferably closer the high than low-end of range. Mid value should work Ok.</li>
</ul>
<p>
<!-- Smoker saves one previous calibration for restoring.<br>
TODL: If the calbration fails you should be able to restore previuously used calibration by clicking 'restore'<br>
-->
<table border="1">

<?php

$temps = array('temp0', 'temp1', 'temp2');
$res = array('resistance0', 'resistance1', 'resistance2');

$ctr = 0;
$SubmitShown=0;

echo '<form action="'.htmlentities($_SERVER['PHP_SELF']).'" method="post">';
for ($i = 0; $i < 3; $i++) {

echo '<tr> <td>';

	if(isset($_POST[$temps[$i]]) && ctype_digit($_POST[$temps[$i]])){
		$temp = $_POST[$temps[$i]];
		$ctr++;
		echo $temp;
		echo '<input type="hidden" name="'.$temps[$i].'" value="'.$temp.'"/></td><td>';
		if (isset($_POST[$res[$i]]) && ctype_digit($_POST[$res[$i]]))
		{
			$resistance = $_POST[$res[$i]];
		}
		else {
			system("/savustin/bin/read-resistance.sh /savustin/tmp/".$res[$i]);
			$resistance = trim(file_get_contents("/savustin/tmp/".$res[$i]));
		}
		echo ' resistance '.$resistance;
		echo '<input type="hidden" name="'.$res[$i].'" value="'.$resistance.'"/></td> </tr>';
	} else {
		echo '<input type="text" name="'.$temps[$i].'"/>';
		echo '<input type="submit" name="foobar" value="measure"/></td></tr>';
	}
}
echo '</form></table>';
if ($ctr == 3) {
	system("/savustin/bin/prepare_tempres.sh ".$_POST[$temps[0]]." ".$_POST[$temps[1]]." ".$_POST[$temps[2]]." /savustin/tmp/".$res[0]." /savustin/tmp/".$res[1]." /savustin/tmp/".$res[2]);
	die('Calibration done <a href="index.php">MAIN PAGE</a></html>');
}
?>

</html>
