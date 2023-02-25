<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <title>Test page</title>
    <!--<link rel="stylesheet" href="style.css">
    <script src="script.js"></script> -->
  </head>
  <body>
    <h1>Test page</h1>
	<p>This is the page content. Bla bla.</p>
    <?php
      include "core/utils.php";

      if (($_SERVER["REQUEST_METHOD"] == "POST") and ( isset($_POST['user']) ) and ( isset($_POST['password'] ))) {
        $user = $_POST['user'];
        $pwd = $_POST['password'];

        // new device form
        $nick = checkUser($user, $pwd, 'nick');
        if ($nick != null) {
          echo "<h2>Vítej ". $nick. "!</h2>". PHP_EOL;
            
          echo "<form method='post' action='create_device.php'><input type='hidden' name='user' value='". $user. "'><input type='hidden' name='password' value='". $pwd. "'><input type='submit' value='Nové zařízení'></form><br />". PHP_EOL;
          echo "<form method='post' action='list_devices.php'><input type='hidden' name='user' value='". $user. "'><input type='hidden' name='password' value='". $pwd. "'><input type='submit' value='Seznam zařízení'></form><br />". PHP_EOL;
        }
        else {
          // incorrect login
          echo "<h2>Přístup odepřen</h2><p>Neplatná kombinace jména a hesla.</p>";
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
