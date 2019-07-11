<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transition$
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <title>Savustimen säätö</title>
  </head>
  <body>
  <a href="index.php">Savustimen tila</a><br><br>
  <h1>Savustimen tavoitelämpö</h1>
<table> <tr>
  <td>
  <?php
	if(isset($_POST['target_temp'])){
		$new_target = $_POST['target_temp'];
		if (ctype_digit($new_target))
			file_put_contents("/target_temp.txt", $new_target);
		else
			echo "<h3>Invalid value ($new_target) for temperature</h3>";
	}

	$target_temp = trim(file_get_contents('/target_temp.txt'));
	echo "Nykyinen tavoitelämpö $target_temp";
?>
  </td>
  <td>
  <form action="<?php echo htmlentities($_SERVER['PHP_SELF']);?>" method="post">
    <!-- <input name="target_temp"  value="100"/> -->
   <select name="target_temp">
    <option value="50">50</option>
    <option value="60">60</option>
    <option value="70">70</option>
    <option value="80">80</option>
    <option value="90">90</option>
    <option value="100">100</option>
    <option value="110">110</option>
    <option value="120">120</option>
    <option value="130">130</option>
    <option value="140">140</option>
    <option value="150">150</option>
    <option value="160">160</option>
    <option value="170">170</option>
    <option value="180">180</option>
    <option value="190">190</option>
    <option value="200">200</option>
    <option value="210">210</option>
  </select> 
  <input type="submit" name="my_form_submit_button" 
           value="Aseta"/>

  </form>
  </td>
  </tr>
  <tr>
  <td>
<?php

	if(isset($_POST['warming_state'])){
		$new_state = $_POST['warming_state'];
		if ($new_state == '0' || $new_state == '1')
			file_put_contents("/sys/class/gpio/gpio61/value", $new_state);
	}
	$warming = trim(file_get_contents('/sys/class/gpio/gpio61/value'));
	if ($warming == 0) {
		$text = "Warming...";
		$new_state='1';
		$new_state_text='lopeta lämmitys';
	}
	else
	{
		$text = "not-warming";
		$new_state='0';
		$new_state_text='aloita lämmitys';
	}
	echo "<h3>$text</h3>";
 
echo '</td>';
echo '<td>';
echo '<form action="'.htmlentities($_SERVER['PHP_SELF']).'" method="post">';
echo '<input name="warming_state" type="hidden" value="'.$new_state.'">';
echo '<input name="force_state" type="hidden" value="1">';
echo '<button type="submit" name="action" value="add_to_cart">'.$new_state_text.'</button>';
echo '</form>';
echo '</td>';
?>
  </tr>
  <tr>
  <td>
<?php

	if(isset($_POST['force_state'])){
		$force_state = $_POST['force_state'];
		if ($force_state == '0' || $force_state == '1')
			file_put_contents("/forcevalue", $force_state);
	}
	$forced = trim(file_get_contents('/forcevalue'));
	if ($forced == 0) {
		$text = "Tila automaattinen (tavoitelämpö käytössä)";
		$new_state='1';
		$new_state_text='pakota lämmitys (ohita tavoitelämpö)';
	}
	else
	{
		$text = "Tila pakotettu (tavoitelämpö ei käytössä)";
		$new_state='0';
		$new_state_text='ota tavoitelämpö käyttöön (poista pakotus)';
	}
	echo "<h3>$text</h3>";
 
echo '</td>';
echo '<td>';
echo '<form action="'.htmlentities($_SERVER['PHP_SELF']).'" method="post">';
echo '<input name="force_state" type="hidden" value="'.$new_state.'">';
echo '<button type="submit" name="action" value="add_to_cart">'.$new_state_text.'</button>';
echo '</form>';
echo '</td>';
?>
  </tr>

  </table>
  <p>
  <a href="oven.log">uunilogi</a>
  <p>
  <a href="shutdown.php">Sammuta beagle (savustin jää päälle!)</a>
  <p>
  </body>
  </html>
