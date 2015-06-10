<?php
/**
 * Created by PhpStorm.
 * User: Vikman
 * Date: 10/06/2015
 * Time: 11:30
 */

namespace manager;

require_once 'templates.php';

$db = new \mysqli(DB_HOST, DB_USER, DB_PASS, DB_NAME);

if ($db->connect_error)
    html_error('database');

function db_get_playlists() {
    global $db;

    $sql = "SELECT idplaylist, playlist.name, COUNT(idscore) FROM playlist LEFT JOIN score ON score.playlist = idplaylist GROUP BY idplaylist";
    $result = $db->query($sql);
    $playlists = [];

    foreach ($result->fetch_all() as $row)
        $playlists[] = ['id' => $row[0], 'name' => $row[1], 'scores' => $row[2]];

    return $playlists;
}

function db_insert_playlist($name) {
    global $db;

    $sql = "INSERT INTO playlist(name) VALUES (?)";
    $stmt = $db->prepare($sql);
    $stmt->bind_param('s', $name);
    $stmt->execute();
}
