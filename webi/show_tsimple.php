<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <title>Detail zařízení</title>
    <script src="http://cdnjs.cloudflare.com/ajax/libs/moment.js/2.13.0/moment.min.js"></script>
    <script src="http://cdnjs.cloudflare.com/ajax/libs/jquery/2.1.3/jquery.min.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.4.0/Chart.bundle.js"></script>
  </head>
  <body>
    <?php
    include "core/utils.php";
    
    if (($_SERVER["REQUEST_METHOD"] == "POST") and ( isset($_POST['dev_id']))) {
      $devid = $_POST['dev_id'];
          
      $conn = new mysqli(SERVER, USER, PASWD, DBNAME);
            
      $query = "SELECT name, keystr, userid, typeid FROM devices WHERE id = '". $devid. "';";
      $res = $conn->query($query);
      $row = $res->fetch_row();
      $device_name = $row['0'];
      $device_keystr = $row['1'];
      $device_userid = $row['2'];
      $device_typeid = $row['3'];
      $query = "SELECT name, description FROM types WHERE id = '". $device_typeid. "'";
      $res = $conn->query($query);
      $row = $res->fetch_row();
      $device_typename = $row['0'];
      $device_typedesc = $row['1'];
      //echo "<p>Typ zařízení: ". $device_typedesc. " (". $device_typeid. ")</p>". PHP_EOL;
      $query = "SELECT temp, tstmp FROM dev_". $device_keystr. " WHERE tstmp = (SELECT MAX(tstmp) FROM dev_". $device_keystr. ");";
      $res = $conn->query($query);
      $row = $res->fetch_row();
      $last_temp = $row['0'];
      $last_tstmp = $row['1'];
      #echo "<p>Poslední měření ". $last_tstmp. " ". $last_temp. "&deg;C</p>". PHP_EOL;
      echo "<h1>". $device_name. " ". $last_temp. "&deg;C</h1>". PHP_EOL;
      echo "<canvas id='myChart'></canvas>". PHP_EOL;
      $time_to = strtotime($last_tstmp);
      $time_from = strtotime('-1 day', $time_to);
      //if ( isset($_POST['from']) )
      //  $time_from = strtotime($_POST['from']);
      //if ( isset($_POST['to']) )
      //  $time_to = strtotime($_POST['to']);
      $query = "SELECT temp, tstmp FROM dev_". $device_keystr. " WHERE tstmp >= '". date('Y-m-d H:i:s', $time_from). "' AND tstmp <= '". date('Y-m-d H:i:s', $time_to). "';";
      $res = $conn->query($query);
      if ($res->num_rows > 0) {
        $data = [];
        //echo "<table>". PHP_EOL. "<tr><th>Čas</th><th>Teplota</th></tr>". PHP_EOL;
        while ($row = $res->fetch_assoc()) {
          $dev_time = $row['tstmp'];
          $dev_temp = $row['temp'];
          array_push($data, [$dev_time, $dev_temp]);
          //echo "<tr><td>". $dev_time. "</td><td>". $dev_temp. " &deg;C</td></tr>". PHP_EOL;
        }
        //echo "</table>". PHP_EOL;
        echo "<script>". PHP_EOL. "var ctx = document.getElementById('myChart').getContext('2d');". PHP_EOL;
        echo "var timeFormat = 'YYYY-MM-DD HH:mm:SS'". PHP_EOL. "var chart = new Chart(ctx, {". PHP_EOL;
        echo "type: 'line',". PHP_EOL. "data: {". PHP_EOL. "  datasets: [{". PHP_EOL. " label: 'Teplota', /*backgroundColor: 'rgb(128, 0, 0)',*/ borderColor: 'rgb(128, 0, 0)'," .PHP_EOL;
        echo "data: ["; //"{x: '2021-01-21 15:30:20', y: 1}, {x: '2021-01-22 08:55:11', y: 3}]
        $first = true;
        for ($i = 0; $i < count($data); $i++) {
          if ($first) $first = false; else echo", ";
          echo "{x: '". $data[$i][0]. "', y: ". $data[$i][1]. "}";
        }
        echo "]". PHP_EOL. "}]},". PHP_EOL;
        echo "options: {legend: {display: false}, elements: { point:{ radius: 0 }}, scales: { xAxes: [{ type: 'time', time: { format: timeFormat, tooltipFormat: 'll'}, scaleLabel: { display: true, labelString: 'Čas'}}], yAxes: [{ scaleLabel: { display: true, labelString: 'Teplota °C' }}]}}});</script>". PHP_EOL;
        //echo "<p>". $query. "</p>". PHP_EOL;
        $conn->close();
      }
    }
    ?>
    <form method="post"><input type='hidden' name='dev_id' value='<?php echo $devid; ?>'><input type='submit' value='Obnovit'></form>
  </body>
  <script>setTimeout(function() {document.forms[0].submit();}, 30000);</script>
</html>