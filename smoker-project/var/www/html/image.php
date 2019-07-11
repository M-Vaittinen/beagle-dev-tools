<?php
	header("Content-type: image/png");
        passthru("/bin/bash -c /smoke.plot");
?>

