<?php

include "consts.php";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
	$node = $_POST['name'];
	$pswd = $_POST['passwd'];
	$data = $_POST['data'];
	if (($node != NODE) && ($pswd != PSWD)) {
		echo "ERROR: INCORRECT CREDENTIALS";
	}
	else {
		if (empty($data)) {
			echo "ERROR: NO DATA";
		} else {
			// use data
			$db = new SQLite3(DB_FILE);
			$query = "CREATE TABLE IF NOT EXISTS ". NODE_TABLE. " (id INTEGER PRIMARY KEY, temp REAL, tstmp DATETIME DEFAULT CURRENT_TIMESTAMP);";
			$res = $db->querySingle($query);
			$query = "INSERT INTO udirna (temp) VALUES (". $data. ");";
			$res = $db->querySingle($query);
			echo "OK";
		}
	}
}
else echo "ERROR: NO POST";
?>
