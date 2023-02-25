<?php
    include "core/utils.php";

    $device_keystr = "QqmxNirN";

    $conn = new mysqli(SERVER, USER, PASWD, DBNAME);
    $query = "SELECT temp, tstmp FROM dev_". $device_keystr. " WHERE tstmp = (SELECT MAX(tstmp) FROM dev_". $device_keystr. ");";
    $res = $conn->query($query);
    $row = $res->fetch_row();
    $last_temp = $row['0'];
    $last_tstmp = $row['1'];
    $conn->close();
    
    header('Content-Type: application/json;charset=utf-8');
    header("Cache-Control: no-store, no-cache, must-revalidate, max-age=0");
    header("Cache-Control: post-check=0, pre-check=0", false);
    header("Pragma: no-cache");

    $tout = strtotime('now') - strtotime($last_tstmp);
    $data = array('temp' => $last_temp, 'tstamp' => $last_tstmp, 'tout' => $tout);
    echo json_encode( $data );
?>