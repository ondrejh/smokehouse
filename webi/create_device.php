<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <title>Vytvoření nového zařízení</title>
    <!-- <link rel="stylesheet" href="style.css">
    <script src="script.js"></script> -->
  </head>
  <body>
    <h1>Registrace nového zařízení</h1>
	<p>Na této stránce je možné zaregistrovat nové zařízení. Výstupem procesu je pak klíč, který bude zařízení používat k přístupu k databázi. Tento klíč je třeba do zařízení zadat pro jeho aktivaci.</p>
    <?php
      include "core/utils.php";
    
      if (($_SERVER["REQUEST_METHOD"] == "POST") and ( isset($_POST['user']) ) and ( isset($_POST['password'] ))) {
        $user = $_POST['user'];
        $pwd = $_POST['password'];

        if (( isset($_POST['action'] )) and ( $_POST['action'] == 'new_device' )) {
          // new device action
          echo "<h2>Vytvářím nové zařízení</h2>". PHP_EOL;
          $user_id = checkUser($user, $pwd, 'id');
          $dev_name = $_POST['device_name'];
          $dev_type_id = $_POST['device_type'];
          echo "<p>Device name: ". $dev_name. ", Device type: ". $dev_type_id. "</p>". PHP_EOL;
          if ($user_id == null) {
            echo "<p>Chyba přihlášení !!!</p>". PHP_EOL;
          }
          else {
            $key = createDevice($dev_name, $dev_type_id, $user_id);  
            if ($key == null) {
              echo "<p>Chyba při vytváření zařízení !!!</p>". PHP_EOL;
            }
            else {
              echo "<p>Zařízení ". $dev_name. " vytvořeno. Klíč je <b>". $key. "</b>.</p>". PHP_EOL;
            }
          }
        }

        else {
          // new device form
          $nick = checkUser($user, $pwd, 'nick');
          if ($nick != null) {
            echo "<h2>Vítej ". $nick. "!</h2>". PHP_EOL. "<p>Tak jaké zařízení chceš přidat?</p>". PHP_EOL;
            echo "<h3>Nové zařízení</h3>". PHP_EOL;
            echo "<form method='post' action='". $_SERVER['PHP_SELF']. "'>". PHP_EOL;
            echo "<input type='hidden' name='user' value='". $user. "'><input type='hidden' name='password' value='". $pwd. "'>". PHP_EOL;
            echo "<input type='hidden' name='action' value='new_device'>". PHP_EOL;
            echo "Jméno: <input type='text' name='device_name'><br />". PHP_EOL;
            echo "Typ: <select name='device_type'>". PHP_EOL;
            $conn = new mysqli(SERVER, USER, PASWD, DBNAME);
            $query = "SELECT id, name, description FROM types;";
            $res = $conn->query($query);
            $conn->close();
            while ( $row = $res->fetch_assoc() ) {
              echo "  <option value='". $row['id']. "'>". $row['name']. ": ". $row['description']. "</option>". PHP_EOL;
            }
            echo "</select>". PHP_EOL;
            echo "<input type='submit'>". PHP_EOL;
            echo "</form>". PHP_EOL;
          }
          else {
            // incorrect login
            echo "<h2>Přístup odepřen</h2><p>Neplatná kombinace jména a hesla.</p>";
          }
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