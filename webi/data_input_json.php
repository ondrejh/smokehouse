<?php

include "consts.php";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
	$v = json_decode(stripslashes(file_get_contents("php://input")));
	//var_dump($v);
	$key = $v->key;
	$data = $v->data;
	if ($key != KEY) {
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
			$query = "INSERT INTO ". NODE_TABLE. " (temp) VALUES (". $data. ");";
			$res = $db->querySingle($query);
			echo "OK";
		}
	}
}
else echo "ERROR: NO POST";
?>
