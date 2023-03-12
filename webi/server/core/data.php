<?php
	include "utils.php";

	if ( isset($_GET['key'])) {
		$devkey = $_GET['key'];
		$conn = new mysqli(SERVER, USER, PASWD, DBNAME);
		
		$query = "SELECT name, typeid FROM devices WHERE keystr = '". $devkey. "';";
		$res = $conn->query($query);
		$row = $res->fetch_row();
		$device_name = $row['0'];
		$device_typeid = $row['1'];
		if ($device_typeid == 2) {
			$query = "SELECT temp1, temp2, tstmp FROM dev_". $devkey. " WHERE tstmp = (SELECT MAX(tstmp) FROM dev_". $devkey. ");";
			$res = $conn->query($query);
			$row = $res->fetch_row();
			$last_temp1 = $row['0'];
			$last_temp2 = $row['1'];
			$last_tstmp = $row['2'];
			$data = array("caption" => $device_name, "time" => $last_tstmp, "data" => [$last_temp1, $last_temp2]);
			header("Content-Type: application/json");
			echo json_encode($data);
			$conn->close();
			exit();
		}
		elseif ($device_typeid == 3) {
			$query = "SELECT name, whg, brs, vol, tmp, tstmp FROM dev_". $devkey. ";";
			$res = $conn->query($query);
			$row = $res->fetch_row();
			$knam = $row[0];
			$kwhg = $row[1];
			$kbrs = $row[2];
			$kvol = $row[3];
			$ktmp = $row[4];
			$tstmp = $row[5];
			$conn->close();

			$tsql = new DateTime($tstmp);
			$tnow = new DateTime(date("Y-m-d H:i:s"));
			$tts = $tsql->getTimestamp();
			$ttn = $tnow->getTimestamp();
			$intv = $ttn - $tts;
			if ($intv < 30) {
				$data = array("units" => $kwhg, "temp" => $ktmp, "primary_unit" => "kg", "secondary_unit" => "piv");
				if ($kvol > 0)
					$data["keg"] = array("name" => $knam, "volume" => $kvol, "left" => $kbrs);
				header("Content-Type: application/json");
				echo json_encode($data);
			}
			exit();
		}
		$conn->close();
		exit();
	}
	else {
		echo "No key ..";
		exit();
	}
?>