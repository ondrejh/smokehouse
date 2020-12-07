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
