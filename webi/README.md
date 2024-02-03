# Files

## Tree

|
|--- inc
|    |--- bootstrap.php
|    |--- config.php
|
|--- thing.php

## Description

inc/config.php: configuration, mainly DB credentials
inc/bootstrap.php: include files

thing.php: the endpoint for data exchange of iot devices

# Database

create database restapi;
use restapi;
create table `things` (`id` bigint(20) unsigned NOT NULL AUTO_INCREMENT, `name` varchar(60) collate utf8mb4_unicode_ci NOT NULL DEFAULT '', `key` varchar(100) collate utf8mb4_unicode_ci NOT NULL DEFAULT '', `data` JSON, PRIMARY KEY(`id`));
insert into things(`name`, `key`) values('test1234', '123456');

# Sources

[How to Build a Simple REST API in PHP](code.tutsplus.com/how-to-build-a-simple-rest-api-in-php--cms-37000t)
[How to Create Simple Rest API in PHP Framework: A Foundation Blueprint](boltic.io/blog/rest-api-in-php-framework)
