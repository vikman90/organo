-- MySQL Script generated by MySQL Workbench
-- 08/07/15 16:47:55
-- Model: New Model    Version: 1.0
-- MySQL Workbench Forward Engineering

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL,ALLOW_INVALID_DATES';

-- -----------------------------------------------------
-- Schema organo
-- -----------------------------------------------------
DROP SCHEMA IF EXISTS `organo` ;

-- -----------------------------------------------------
-- Schema organo
-- -----------------------------------------------------
CREATE SCHEMA IF NOT EXISTS `organo` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci ;
USE `organo` ;

-- -----------------------------------------------------
-- Table `playlist`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `playlist` (
  `idplaylist` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `name` VARCHAR(255) NULL,
  PRIMARY KEY (`idplaylist`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `score`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `score` (
  `idscore` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `playlist` INT UNSIGNED NOT NULL,
  `sorting` INT UNSIGNED NULL,
  `source` VARCHAR(255) NULL,
  `name` VARCHAR(255) NULL,
  `duration` INT NULL,
  PRIMARY KEY (`idscore`),
  INDEX `score_playlist_idx` (`playlist` ASC),
  CONSTRAINT `score_playlist`
    FOREIGN KEY (`playlist`)
    REFERENCES `playlist` (`idplaylist`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `shortcut`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `shortcut` (
  `idshortcut` INT UNSIGNED NOT NULL,
  `playlist` INT UNSIGNED NULL,
  PRIMARY KEY (`idshortcut`),
  INDEX `shortcut_playlist_idx` (`playlist` ASC),
  CONSTRAINT `shortcut_playlist`
    FOREIGN KEY (`playlist`)
    REFERENCES `playlist` (`idplaylist`)
    ON DELETE SET NULL
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;

-- -----------------------------------------------------
-- Data for table `shortcut`
-- -----------------------------------------------------
START TRANSACTION;
USE `organo`;
INSERT INTO `shortcut` (`idshortcut`, `playlist`) VALUES (0, NULL);
INSERT INTO `shortcut` (`idshortcut`, `playlist`) VALUES (1, NULL);

COMMIT;

