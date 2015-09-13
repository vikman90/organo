<?php
/**
 * Created by PhpStorm.
 * User: Vikman
 * Date: 11/05/2015
 * Time: 21:43
 */

namespace manager;

const DEFAULT_LANGUAGE = 'es';
const LINUX_USERNAME = 'pi';
const WRONG_PASS_DELAY = 2;
const REBOOT_TIME = 60;

const DB_HOST = 'localhost';
const DB_NAME = 'organo';
const DB_USER = 'organo';
const DB_PASS = 'ugr2015';

const FILE_MAXSIZE = 1048576;
const SCORE_DIR = '/home/pi/midis';
const DOWNLOADS_DIR = 'pool';

const BUFFER_MAX = 256;
const SOCKET_PATH = "/run/organd.sock";

const EXEC_SHUTDOWN = '/usr/bin/sudo /sbin/shutdown now 2>&1';
const EXEC_REBOOT = '/usr/bin/sudo /sbin/shutdown -r now 2>&1';
const EXEC_LOGIN = '/usr/local/bin/organ-login';
const EXEC_DURATION = '/usr/local/bin/organ-midinfo --duration';
