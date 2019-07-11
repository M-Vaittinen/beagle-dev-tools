<?php
$secondsWait = 10;
header("Refresh:$secondsWait");
echo date('Y-m-d H:i:s');
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transition$
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <title>Savust Mua</title>
  </head>
  <body>
  <a href="control.php">Savustimen ohjaus</a><br><br>
  <h1>Savusta Mua</h1>
<table>
<tr>
<td>
<?php
	$target_temp = trim(file_get_contents('/target_temp'));
	echo "Savustimen tavoitelämpö $target_temp";
?>
</td>
<td>
</td>
</tr> 
<tr><td>
  <h3>savustimen lampo</h3>
  <img src="image.php" alt="temperatures">
<p>
</td>
<td>
<?php
	$warming = trim(file_get_contents('/sys/class/gpio/gpio61/value'));
	if ($warming == 0) {
		$img = "images/red-led.png";
		$text = "Warming...";
	}
	else
	{
		$img = "images/green-led.png";
		$text = "not-warming";
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
?>
<img src="image2.php" alt="temperatures">
</td>
<td>

</td>
</tr>
</table>

</body>
</html>
