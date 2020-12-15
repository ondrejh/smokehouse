<?php

# database
include "dbauth.php";

#const SERVER = "localhost";
#const USER = "user";
#const PASWD = "user123";
#const DBNAME = "testdb";

function checkUser($user, $password, $retval) {
  $conn = new mysqli(SERVER, USER, PASWD, DBNAME);
  $query = "SELECT id, nick, password FROM users WHERE name = '". $user. "';";
  $res = $conn->query($query);
  $conn->close();
    
  if ( !$res ) {
    return null;
  }
    
  if ( $res->num_rows > 0 ) {
    $row = $res->fetch_row();
      
    if ($row[2] != $password)
      return null;
    
    if ($retval == 'nick')
      return $row[1];
    if ($retval == 'id')
      return $row[0];

    return true;
  }

  return null;
}

function generateRandomString($length = 10) {
    $characters = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
    $charactersLength = strlen($characters);
    $randomString = '';
    for ($i = 0; $i < $length; $i++) {
        $randomString .= $characters[rand(0, $charactersLength - 1)];
    }
    return $randomString;
}

function createDevice($name, $type_id, $user_id) {
  // create connection
  $conn = new mysqli(SERVER, USER, PASWD, DBNAME);
  // check connection
  if ($conn->connect_error) {
      return null;
  }
    
  // create table (if not exist)
  $query = "CREATE TABLE IF NOT EXISTS devices (id INTEGER NOT NULL AUTO_INCREMENT PRIMARY KEY, name VARCHAR(16), idstr VARCHAR(16), keystr VARCHAR(16), userid INTEGER, typeid INTEGER);";
  $res = $conn->query($query);
  #var_dump($res);
  
  // generate key (and test if not exists)
  $key = null;
  do {
    $key = generateRandomString(8);
    $query = "SELECT * FROM devices WHERE keystr = '". $key. "';";
    $res = $conn->query($query);
    if ($res->num_rows > 0)
        $key = null;
  } while ($key == null);
    
  // add device
  $query = "INSERT INTO devices (name, typeid, keystr, userid) VALUES ('". $name. "', '". $type_id. "', '". $key. "', '". $user_id. "');";
  $res = $conn->query($query);
  $conn->close();
  return $key;
}

?>