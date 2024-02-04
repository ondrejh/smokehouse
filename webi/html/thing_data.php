<?php
require __DIR__ . "/inc/bootstrap.php";

if (isset($_GET['id'])) {
	$conn = new mysqli(DB_HOST, DB_USERNAME, DB_PASSWORD, DB_DATABASE_NAME);

	if ($conn) {
		$query = "SELECT `id`, `data`, `tstamp` FROM things WHERE `name`=?";
		$stmt = $conn->prepare($query);
		if ($stmt !== false) {
			$stmt->bind_param('s', $_GET['id']);
			$stmt->execute();
			$res = $stmt->get_result();
			$row = $res->fetch_assoc();
			if ($row) {
				http_response_code(200);
				header("Content-Type: application/json");
				$resp = array("data"=>$row['data'], "tstamp"=>$row['tstamp']);
				echo json_encode($resp);
				exit();
			}
		}
	}
	http_response_code(400);
}
?>
