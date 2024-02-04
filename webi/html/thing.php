<?php
require __DIR__ . "/inc/bootstrap.php";

function myValidate($pwd, $hash) {
	if (substr($hash, 0, 1) != "$")
		return ($pwd === $hash);
	else
		return (password_verify($pwd, $hash));
}

function myNeedRehash($hash) {
	if (substr($hash, 0, 1) != "$")
		return true;
	else
		return password_needs_rehash($hash, PASSWORD_DEFAULT);
}

$jsonData = file_get_contents('php://input');
$td = json_decode($jsonData, true);
if (($td !== null) && (isset($td['id'])) && (isset($td['key']))) {
	$name = $td['id'];
	$key = $td['key'];
	$id = null;
	$dresp = array();

	$conn = new mysqli(DB_HOST, DB_USERNAME, DB_PASSWORD, DB_DATABASE_NAME);
	if ($conn) {
		// validate device name and key pair
		$query = "SELECT `id`, `key`, `data` FROM things WHERE `name`=?";
		$stmt = $conn->prepare($query);
		if ($stmt !== false) {
			$stmt->bind_param('s', $name);
			$stmt->execute();
			$res = $stmt->get_result();
			$row = $res->fetch_assoc();
			if ($row) {
				$skey = $row['key'];
				if (myValidate($key, $skey)) {
					$id = $row['id']; // the device is validated

					// key rehash if needed
					if (myNeedRehash($skey)) {
						$newHash = password_hash($key, PASSWORD_DEFAULT);
						$query = "UPDATE things SET `key`=? WHERE `id`=?";
						$stmt = $conn->prepare($query);
						$stmt->bind_param('si', $newHash, $id);
						$stmt->execute();
					}

					// update data if any (and timestamp)
					if (isset($td['data'])) {
						$query = "UPDATE things SET `data`=?, `tstamp`=CURRENT_TIMESTAMP WHERE `id`=?";
						$stmt = $conn->prepare($query);
						$stmt->bind_param('si', json_encode($td['data']), $id);
						$stmt->execute();
					}
					// or reply with last data
					//else {
					//	$dresp = $row['data'];
					//}
				}
			}
		}
	}

	if ($id !== null) {
		http_response_code(200);
		header("Content-Type: application/json");
		$resp = array("result"=>"OK", "id"=>$id);
		if ($dresp) $resp["data"] = $dresp;
		echo json_encode($resp);
		exit();
	}
	else
		http_response_code(400);
}
else {
?>
<!DOCTYPE html>
<html lang="cz">
<head>
  <meta charset="utf-8">
  <title>Kegator - věci</title>
</head>
<body>
  <h1>Přípojný bod našich věcí ...</h1>
  <p>
    <?php echo DB_DATABASE_NAME; ?>
  </p>
  <table>
  <tr><th>ID</th><th>Jméno</th><th>Klíč</th><th>Online</th></tr>
<?php
$conn = new mysqli(DB_HOST, DB_USERNAME, DB_PASSWORD, DB_DATABASE_NAME);
if ($conn) {
	$query = "SELECT `id`, `name`, `key`, `tstamp` FROM things";
	$stmt = $conn->prepare($query);
	if ($stmt !== false) {
		$stmt->execute();
		$res = $stmt->get_result();
		//$row = $res->fetch_assoc();
		while ($row = $res->fetch_assoc()) {
			$key = (substr($row['key'], 0, 1) != "$") ? $row['key'] : '...';
			$online = (strtotime($row['tstamp']) > strtotime("-2 minutes")) ? "Online" : "Offline";
			echo "<tr><td>". $row['id']. "</td><td>". $row['name']. "</td><td>". $key. "</td><td>". $online. "</td></tr>". PHP_EOL;
		}
	}
}
?>
  </table>
</body>
</html>
<?php } ?>
