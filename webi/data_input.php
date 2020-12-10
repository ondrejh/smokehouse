<?php

define("NODE", "udirna");
define("PSWD", "1234");

define("DB_FILE", "test.db")
define("NODE_TABLE", "nodes")

if ($_SERVER["REQUEST_METHOD"] == "POST") {
	$node = $_POST['node'];
	$pswd = $_POST['passwd'];
	$data = $_POST['data'];
	if (($node != NODE) && ($pswd != PSWD)) {
		echo "ERROR: INCORRECT CREDENTIALS";
	}
	else {
		if (empty($data)) {
			echo "ERROR: NO DATA";
		} else {
			$db = new SQLite3(DB_FILE);
			
			// use data
			echo "OK";
		}
	}
}
else echo "ERROR: NO POST";
?>
