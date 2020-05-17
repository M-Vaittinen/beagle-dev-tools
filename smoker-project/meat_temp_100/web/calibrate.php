<html>
<head>
<title>calibrate</title>
</head>
<body>
</body>

<h1>Kalibroi lihalämpömittari</h1>
<p>
Kalbroi (uusi) lihalämpömittarin anturi seuraavasti:
<p>
<ul>
<li>Ota joku muu lämpömittari joka kestää kuumaa. Laita se ja blackburnin mittari astiaan.</li>
<li>Kaada asitiaan kuumaa vettä ja katso lämpötila toimivasta mittarista</li>
<li>Syötä lämpötilaarvo sivulla olevaan kentään ja paina "mittaa"</li>
<li>Syötä huomattavasti alhaisempi lämpö seuraavaan kenttään ja veden jäähdyttyä tähän lämpötilaan paina jälleen "mittaa"</li>
<li>Viimeiseksi voit laittaa astiaan kylmää vettä ja syöttää veden lämpötilan viimeiseen kenttään ja painaa "mittaa"</li>
<li>Kun olet mitannut kakki kolme arvoa paina "tallenna kalibrointi"</li>
</ul>
<p>
<h3>Ohjeita kalibrointiin käytettävien lämpöarvojen valintaan</h3>
<ul>
<li>Kahden arvon olisi hyvä olla mitattavan lämpötila-alueen ääripäistä. Esim 10 C - 80 C</li>
<li>Keskimmäisen arvon olisi hyvä olla näiden puolivälistä, mielellään kuitenkin lähempänä kuuminta arvoa kuin viileintä</li>
</ul>
<p>
Savustin tallentaa yhden aiemman kalibroinnin palautusta varten.<br>
Mikäli kalibrointi epäonnistuu voit palauttaa edellisen käytetyn kalibroinnin klikkaamalla tällä
sivulla "palauta"<br>

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
			echo "resistance".$i." was set. Value ".$resistance;
		}
		else {
			echo "resistance".$i." was not set. Reading and writing to ".$res[$i];
			echo 'calling system("/read-resistance.sh /'.$res[$i].'");'; 
			system("/read-resistance.sh /".$res[$i]);
			echo 'reading measured resistance: trim(file_get_contents("/'.$res[$i].'"));';
			$resistance = trim(file_get_contents("/".$res[$i]));
			echo "Just Read resistance ".$i." value ".$resistance;
			echo ' resistance'.$resistance;
		}
		echo $resistance;
		echo '<input type="hidden" name="'.$res[$i].'" value="'.$resistance.'"/></td> </tr>';
	} else {
		echo '<input type="text" name="'.$temps[$i].'"/>';
		echo '<input type="submit" name="foobar" value="mittaa"/></td></tr>';
	}
}
echo '</form></table>';
if ($ctr == 3) {
	system("/prepare_tempres.sh ".$_POST[$temps[0]]." ".$_POST[$temps[1]]." ".$_POST[$temps[2]]." /".$res[0]." /".$res[1]." /".$res[2]);
	die('kalibroitu <a href="index.php"></html>');
}
?>

</html>
