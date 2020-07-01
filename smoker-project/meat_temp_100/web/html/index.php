<?php
$secondsWait = 10;
header("Refresh:$secondsWait");
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transition$
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <title>Savustin</title>
  </head>
  <body>
  <?php
    echo date('Y-m-d H:i:s');
    echo '<br>';
  ?>
  <a href="control.php">Savustimen ohjaus</a><br><br>
<?php
  $forced = trim(file_get_contents('/savustin/tmp/forcevalue'));
  if ($forced == 1) {
	echo "<h1>VAROITUS! Savustimen tila pakotettu!</h1>";	
}
?>
  <h1>Savusta Mua</h1>
  <a href="oven.log">uunilogi</a>
  <p>
  <a href="shutdown.php">Sammuta beagle</a>
  <p>
<table>
<tr>
<td>
<?php
	$target_temp = trim(file_get_contents('/savustin/tmp/target_temp.txt'));
        $lampo_nyt = trim(file_get_contents('/var/www/html/temp'));
	echo "<h3>Savustimen tavoitelämpö $target_temp</h3>";
        echo "<h3>savustimen lampo $lampo_nyt</h3>";
?>
</td>
<td>
</td>
</tr>
<tr><td>
  <img src="image.php" alt="temperatures">
<p>
</td>
<td>
<?php
	$warming = trim(file_get_contents('/sys/class/gpio/gpio61/value'));
	if ($warming == 1) {
		$img = "images/red-led.png";
		$text = "Lämpenee...";
	}
	else
	{
		$img = "images/green-led.png";
		$text = "Ei l&auml;mpene";
	}
	echo "<h3>$text</h3>";
	echo "<img src=\"$img\" alt=\"$text\">";
?> 
</td>
</tr>
<tr>
<td>
<?php
	$number = trim(file_get_contents('meat_temp'));
	echo "<h3>ruoan lampo (viim mitattu $number)</h3>";
        $mtt = trim(file_get_contents('/savustin/tmp/meat_target_temp'));
	if ($mtt <=  $number) {
		echo '<audio controls autoplay>';
		echo ' <source src="Ding-fries-are-done.mp3" type="audio/mpeg">';
		echo '</audio>';
  		echo '<h1>FOOD ready!</h1>';
	}
?>
<img src="image2.php" alt="temperatures">
</td>
<td>

</td>
</tr>
</table>
<p>
<a href="tomppa">tomppa</a>

</body>
</html>
