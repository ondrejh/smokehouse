# SmokeHouse

The IOT smokehouse thermometer using standard web server as the interface.

## DB scheme ##

![Database scheme](/doc/db_scheme.png)

## Next steps (HW / FW) ##

- [X] update sketch !!!
- [X] use device registration (unique id, key)
- [X] connect to webi as t_simple
- [ ] add the second thermocoupler
- [ ] draw the schematic

## Next steps (WEBI) ##

- [X] post some data to server (test)
- [X] receive and decode (json) data
- [X] save it to db (sqlite)
- [X] mysql
- [X] public server
- [X] better scheme (users, devices, data)
- [X] register new device
- [X] list devices
- [ ] smoker device scheme
- [ ] dump t_simple data

## Install LAMP ##

Install Apache
--------------

```
sudo apt update
sudo apt install apache2
```

Install MySQL
-------------

```
sudo apt install mariadb-server
mysql_secure_installation

sudo mysql
```

Create database
---------------

```sql
CREATE DATABASE devices;
SHOW DATABASES;
CREATE USER devAdmin@localhost IDENTIFIED BY 'devAdm123';
SELECT User FROM mysql.user;
GRANT ALL PRIVILEGES ON devices.* to devAdmin@localhost;
FLUSH PRIVILEGES;
SHOW GRANTS FOR devAdmin@localhost;
SELECT user,authentication_string,plugin,host FROM mysql.user;
```

Install PHP
-----------

```
sudo apt install php libapache2-mod-php
sudo apt install php-mysql php-gd
sudo systemctl restart apache2.service
```

Set privileges
--------------

```
cd /var/www
sudo chown -R www-data:www-data html
sudo chmod 775 html
cd html
sudo chmod 664 *
```

Add user into www-data group
----------------------------

```
sudo usermod -aG www-data <username>
```
