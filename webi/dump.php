<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <title>Test dump</title>
    <!-- <link rel="stylesheet" href="style.css">
    <script src="script.js"></script> -->
  </head>
  <body>
    <h1>Test dump</h1>
	<table>
	  <?php
		include "consts.php";
		
		$conn = new mysqli(SERVER, USER, PASWD, DBNAME);
	    //$db = new SQLite3(DB_FILE);
		$query = "SELECT * FROM ". NODE_TABLE. ";";
		$res = $conn->query($query);
		if ($res->num_rows > 0) {
		  // output data of each row
		  while($row = $res->fetch_assoc()) {
		    echo "id: " . $row["id"]. ", temp: " . $row["temp"]. ", time: " . $row["tstmp"]. "<br>";
		  }
		} else {
		  echo "0 results";
		}
		//while ($row = $res->fetchArray()) {
		//  echo "<tr><td>{$row[0]}</td><td>{$row[1]}</td><td>{$row[2]}</td></tr>\n";
		//}
		$conn->close();
	  ?>
	</table>
  </body>
</html>
