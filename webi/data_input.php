<?php
if ($_SERVER["REQUEST_METHOD"] == "POST") {
	$data = $_POST['data'];
	if (empty($data)) {
		echo "ERROR: NO DATA";
	} else {
		// use data
		echo "OK";
	}
}
else echo "ERROR: NO POST";
?>
