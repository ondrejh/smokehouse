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
		
	    $db = new SQLite3(DB_FILE);
		$query = "SELECT * FROM udirna;";
		$res = $db->query($query);
		while ($row = $res->fetchArray()) {
		  echo "<tr><td>{$row[0]}</td><td>{$row[1]}</td><td>{$row[2]}</td></tr>\n";
		}
	  ?>
	</table>
  </body>
</html>
