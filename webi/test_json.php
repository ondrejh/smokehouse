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
	<!--<p>This is the page content.</p>
	<form method="post" action="data_input.php">
	  <input type="hidden" name="key" value="<?php include "consts.php"; echo KEY; ?>"><br>
	  Data: <input type="text" name="data"><br>
	  <input type="submit">
	</form>-->
	<p>Test with json.</p>
	<input type="text" id="value">
	<button onclick="sendFunction()">Click to send data</button>
	<button onclick="myAlert()">Test alert</button>

	<script>
	  function myAlert() {
		alert("I am an alert box!");
	  }
	  function sendFunction() {
		// Sending and receiving data in JSON format using POST method
		var xhr = new XMLHttpRequest();
		var url = "data_input_json.php";
		xhr.open("POST", url, true);
		xhr.setRequestHeader("Content-Type", "application/json");
		xhr.onreadystatechange = function () {
		  if (xhr.readyState === 4 && xhr.status === 200) {
            alert(xhr.responseText);
		  }
	    };
	    var data = JSON.stringify({"key": "<?php echo KEY; ?>", "data": document.getElementById("value").value});
	    xhr.send(data);
	  }
</script>
  </body>
</html>
