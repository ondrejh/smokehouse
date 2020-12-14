<?php

include "consts.php";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
	$v = json_decode(stripslashes(file_get_contents("php://input")));
	$key = $v->key;
	$data = $v->data;
	if ($key != KEY) {
		echo "ERROR: INCORRECT CREDENTIALS";
	}
	else {
		if (empty($data)) {
			echo "ERROR: NO DATA";
		} else {
			// Create connection
			$conn = new mysqli(SERVER, USER, PASWD, DBNAME);
			// Check connection
			if ($conn->connect_error) {
			  die("Connection failed: " . $conn->connect_error);
			} 
			//$db = new SQLite3(DB_FILE);
			$query = "CREATE TABLE IF NOT EXISTS ". NODE_TABLE. " (id INTEGER NOT NULL AUTO_INCREMENT PRIMARY KEY, temp REAL, tstmp DATETIME DEFAULT CURRENT_TIMESTAMP);";
			$res = $conn->query($query);
			echo $res;
			$query = "INSERT INTO ". NODE_TABLE. " (temp) VALUES (". $data. ");";
			$res = $conn->query($query);
			$conn->close();
			echo "OK";
		}
	}
}
else echo "ERROR: NO POST";
?>
