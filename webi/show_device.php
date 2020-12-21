<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <title>Výpis zařízení</title>
    <!-- <link rel="stylesheet" href="style.css">
    <script src="script.js"></script> -->
  </head>
  <body>
    <h1>Výpis zařízení</h1>
	<p>Tato stránka poskytuje podrobný výpis daného zařízení.</p>
    <?php
      include "core/utils.php";
    
      if (($_SERVER["REQUEST_METHOD"] == "POST") and ( isset($_POST['user']) ) and ( isset($_POST['password'] )) and ( isset($_POST['dev_id']))) {
        $user = $_POST['user'];
        $pwd = $_POST['password'];
        $devid = $_POST['dev_id'];
          
        $userid = checkUser($user, $pwd, 'id');
          
        if ($userid == null) {
          echo "<p>Neplatné kombinace jména a hesla !!!</p>". PHP_EOL;
        }
        else {
          $conn = new mysqli(SERVER, USER, PASWD, DBNAME);
            
          $query = "SELECT name, keystr FROM devices WHERE id = '". $devid. "';";
          $res = $conn->query($query);
          $row = $res->fetch_row();
          echo "<h2>Zařízení ". $row['0']. "</h2>". PHP_EOL;
        
          $conn->close();
        }
      }
      
      else {
        // user login form
        echo "<p>Pro pokračováni se musíš přihlásit.</p>". PHP_EOL;
        echo "<a href='www.kegator.cz'>Zpět na hlavní stránku</a>". PHP_EOL;
      }
    ?>
  </body>
</html>