# SmokeHouse

The smokehouse thermometer (posibly IOT?)

## Install DB ##

```
sudo apt install mariadb-server
mysql_secure_installation

sudo mysql
```

```sql
CREATE DATABASE uzeniTest;
SHOW DATABASES;
CREATE USER udirna@localhost IDENTIFIED BY 'udirna123';
SELECT User FROM mysql.user;
GRANT ALL PRIVILEGES ON uzeniTest.* to udirna@localhost;
FLUSH PRIVILEGES;
SHOW GRANTS FOR udirna@localhost;
SELECT user,authentication_string,plugin,host FROM mysql.user;
```

```
sudo apt install php libapache2-mod-php
sudo apt install php-mysql php-gd
sudo systemctl restart apache2.service
```

## Next steps (HW / FW) ##

- [X] update sketch !!!
- [ ] add the second thremocoupler
- [ ] draw the schematic

## Next steps (WEBI) ##

- [X] post some data to server (test)
- [X] receive and decode (json) data
- [X] save it to db (sqlite)
- [X] mysql
- [X] public server
- [ ] better scheme (user, node, data)
