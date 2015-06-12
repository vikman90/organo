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

$db->set_charset('utf8');

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

function db_get_playlist($idplaylist) {
    global $db;

    $sql = "SELECT name FROM playlist WHERE idplaylist = ?";
    $stmt = $db->prepare($sql);
    $stmt->bind_param('i', $idplaylist);
    $stmt->execute();
    $row = $stmt->get_result()->fetch_row();

    if (!$row)
        return null;

    $name = $row[0];
    $sql = "SELECT idscore, source, name FROM score WHERE playlist = ? ORDER BY sorting";
    $stmt = $db->prepare($sql);
    $stmt->bind_param('i', $idplaylist);
    $stmt->execute();
    $result = $stmt->get_result();
    $scores = [];

    while ($row = $result->fetch_row())
        $scores[] = ['id' => $row[0], 'source' => $row[1], 'name' => $row[2]];

    return ['id' => $idplaylist, 'name' => $name, 'scores' => $scores];
}

function db_rename_playlist($idplaylist, $name) {
    global $db;

    $sql = "UPDATE playlist SET name = ? WHERE idplaylist = ?";
    $stmt = $db->prepare($sql);
    $stmt->bind_param('si', $name, $idplaylist);
    $stmt->execute();
}

function db_delete_playlist($idplaylist) {
    global $db;

    $sql = "DELETE FROM playlist WHERE idplaylist = ?";
    $stmt = $db->prepare($sql);
    $stmt->bind_param('i', $idplaylist);
    $stmt->execute();
}

function db_insert_score($idplaylist, $name) {
    global $db;

    $sql = "INSERT INTO score (playlist, name) VALUES (?, ?)";
    $stmt = $db->prepare($sql);
    $stmt->bind_param('is', $idplaylist, $name);
    $stmt->execute();
    $idscore = $stmt->insert_id;
    $source = $idscore . '.mid';
    $sql = "UPDATE score SET sorting = idscore, source = ? WHERE idscore = ?";
    $stmt = $db->prepare($sql);
    $stmt->bind_param('si', $source, $idscore);
    $stmt->execute();

    return ['id' => $idscore, 'playlist' => $idplaylist, 'source' => $source, 'name' => $name];
}

function db_delete_score($idscore) {
    global $db;

    $sql = "DELETE FROM score WHERE idscore = ?";
    $stmt = $db->prepare($sql);
    $stmt->bind_param('i', $idscore);
    $stmt->execute();
}
