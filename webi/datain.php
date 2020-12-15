<?php

#include "consts.php";
include "utils.php";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
  $v = json_decode(stripslashes(file_get_contents("php://input")));
  $keystr = $v->key;
  $idstr = $v->idstr;
  $data = $v->data;

  $conn = new mysqli(SERVER, USER, PASWD, DBNAME);
  $query = "SELECT devices.id, devices.idstr, types.name FROM devices INNER JOIN types ON devices.typeid=types.id WHERE keystr = '". $keystr. "';";
  $res = $conn->query($query);

  if ($res->num_rows > 0) {
	$row = $res->fetch_row();
	$dev_id = $row[0];
	$dev_idstr = $row[1];
	$dev_type = $row[2];
	  
	if ($dev_type == 't_simple') {
	  $query = "CREATE TABLE IF NOT EXISTS dev_". $keystr. "(id INTEGER NOT NULL AUTO_INCREMENT PRIMARY KEY, temp REAL, tstmp DATETIME DEFAULT CURRENT_TIMESTAMP);";
	  $conn->query($query);
	  $query = "INSERT INTO dev_". $keystr. "(temp) VALUES (". $data. ");";
	  $conn->query($query);
	  if ($dev_idstr == null) {
		$query = "UPDATE devices SET idstr = '". $idstr. "' WHERE id = ". $dev_id. ";";
		$conn->query($query);
	  }
	  $conn->close();
	  echo "OK";
    }
    else if ($dev_type == 'th_simple') {
    }
    }
    echo "ERROR: Device not recognised"; 
}
else echo "ERROR: NO POST";
?>
