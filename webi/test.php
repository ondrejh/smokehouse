<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <title>Test page</title>
    <!-- <link rel="stylesheet" href="style.css">
    <script src="script.js"></script> -->
  </head>
  <body>
    <h1>Test page</h1>
	<p>This is the page content.</p>
	<form method="post" action="data_input.php">
	  <input type="hidden" name="key" value="<?php include "consts.php"; echo KEY; ?>"><br>
	  Data: <input type="text" name="data"><br>
	  <input type="submit">
	</form>
  </body>
</html>
