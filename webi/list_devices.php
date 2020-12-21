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
	<p>Tato stránka poskytuje přehled zařízení daného uživatele.</p>
    <?php
      include "core/utils.php";
    
      if (($_SERVER["REQUEST_METHOD"] == "POST") and ( isset($_POST['user']) ) and ( isset($_POST['password'] ))) {
        $user = $_POST['user'];
        $pwd = $_POST['password'];
          
        $userid = checkUser($user, $pwd, 'id');
          
        if ($userid == null) {
          echo "<p>Neplatné kombinace jména a hesla !!!</p>". PHP_EOL;
        }
        else {
          $conn = new mysqli(SERVER, USER, PASWD, DBNAME);
            
          $query = "SELECT nick FROM users WHERE id = '". $userid. "';";
          $res = $conn->query($query);
          $row = $res->fetch_row();
          $nick = $row[0];
          echo "<h2>Zařízení uživatele ". $nick. "</h2>". PHP_EOL;
            
          if (( isset($_POST['action']) ) and ( $_POST['action'] == 'change_name' )) {
            $query = "UPDATE devices SET name = '". $_POST['dev_name']. "' WHERE id = ". $_POST['dev_id']. ";";
            $conn->query($query);  
          }
            
          $query = "SELECT devices.id, devices.name, devices.idstr, types.description, devices.keystr FROM devices INNER JOIN types ON devices.typeid=types.id WHERE userid = '". $userid. "';";
          #echo PHP_EOL. $query. PHP_EOL;
          $res = $conn->query($query);
          if ($res->num_rows > 0) {
            echo "<table>". PHP_EOL. "<tr><th>Jméno</th><th>Typ</th><th>Klíč</th><th>Aktivní</th></tr>". PHP_EOL;
            while ($row = $res->fetch_assoc()) {
              $dev_id = $row['id'];
              $dev_name = $row['name'];
              $dev_type = $row['description'];
              $dev_key = $row['keystr'];
              $dev_active = ($row['idstr'] == null) ? 'NE' : 'ANO';
              $edit_form = "<form method='post' action='". $_SERVER['PHP_SELF']. "'><input type='hidden' name='user' value='". $user. "'><input type='hidden' name='password' value='". $pwd. "'><input type='hidden' name='dev_id' value='". $dev_id. "'><input type='hidden' name='action' value='change_name'><input type='text' name='dev_name' value = '". $dev_name. "'><input type='submit' value='Upravit'></form>";
              $enter_form = "<form method='post' action='show_device.php'><input type='hidden' name='user' value='". $user. "'><input type='hidden' name='password' value='". $pwd. "'><input type='hidden' name='dev_id' value='". $dev_id. "'><input type='submit' value='Otevřít'></form>";
              echo "<tr><td>". $edit_form. "</td><td>". $dev_type. "</td><td>". $dev_key. "</td><td>". $dev_active. "</td><td>". $enter_form. "</td></tr>". PHP_EOL;
            }
            echo "<table>". PHP_EOL;
          }
          else {
            echo "<p>Nic. Milý ". $nick. " měl by ses polepšit.</p>". PHP_EOL;
          }
          $conn->close();
        }
      }
      
      else {
        // user login form
        echo "<p>Pro pokračováni se musíš přihlásit.</p>". PHP_EOL;
        echo "<form method='post' action='". $_SERVER['PHP_SELF']. "'>". PHP_EOL;
        echo "Přihlašovací jméno: <input type='text' name='user'><br />". PHP_EOL;
        echo "Heslo: <input type='password' name='password'><br />". PHP_EOL;
        echo "<input type='submit'>";
        echo "</form>". PHP_EOL;
      }
    ?>
  </body>
</html>