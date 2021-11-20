-- MySQL Script generated by MySQL Workbench
-- Sat Nov 20 17:30:19 2021
-- Model: New Model    Version: 1.0
-- MySQL Workbench Forward Engineering

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION';

-- -----------------------------------------------------
-- Schema bank_simul
-- -----------------------------------------------------

-- -----------------------------------------------------
-- Schema bank_simul
-- -----------------------------------------------------
CREATE SCHEMA IF NOT EXISTS `bank_simul` DEFAULT CHARACTER SET utf8 ;
USE `bank_simul` ;

-- -----------------------------------------------------
-- Table `bank_simul`.`customers`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `bank_simul`.`customers` (
  `idcustomer` INT NOT NULL AUTO_INCREMENT,
  `firstname` VARCHAR(45) NULL,
  `lastname` VARCHAR(45) NULL,
  `address` VARCHAR(45) NULL,
  `phone` INT NULL,
  PRIMARY KEY (`idcustomer`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `bank_simul`.`accounts`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `bank_simul`.`accounts` (
  `idaccount` INT NOT NULL AUTO_INCREMENT,
  `idOwner` INT NOT NULL,
  `balance` BIGINT NULL,
  PRIMARY KEY (`idaccount`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `bank_simul`.`cards`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `bank_simul`.`cards` (
  `idcard` INT NOT NULL AUTO_INCREMENT,
  `pin` SMALLINT NULL,
  `failed_attempts` TINYINT NULL,
  `has_credit` TINYINT NULL,
  `idaccount` INT NOT NULL,
  `idcustomer` INT NOT NULL,
  PRIMARY KEY (`idcard`),
  INDEX `fk_cards_accounts1_idx` (`idaccount` ASC) VISIBLE,
  INDEX `fk_cards_customers1_idx` (`idcustomer` ASC) VISIBLE,
  CONSTRAINT `fk_cards_accounts1`
    FOREIGN KEY (`idaccount`)
    REFERENCES `bank_simul`.`accounts` (`idaccount`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_cards_customers1`
    FOREIGN KEY (`idcustomer`)
    REFERENCES `bank_simul`.`customers` (`idcustomer`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `bank_simul`.`transactions`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `bank_simul`.`transactions` (
  `idtransaction` INT NOT NULL AUTO_INCREMENT,
  `transaction_type` TINYINT NULL,
  `transaction_amount` INT NULL,
  `transaction_time` DATETIME NULL,
  `idaccount` INT NOT NULL,
  `idcard` INT NULL,
  PRIMARY KEY (`idtransaction`),
  INDEX `fk_transactions_accounts1_idx` (`idaccount` ASC) VISIBLE,
  CONSTRAINT `fk_transactions_accounts1`
    FOREIGN KEY (`idaccount`)
    REFERENCES `bank_simul`.`accounts` (`idaccount`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `bank_simul`.`credit`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `bank_simul`.`credit` (
  `idcredit` INT NOT NULL AUTO_INCREMENT,
  `credit_limit` INT NULL,
  `balance` INT NULL,
  `idaccount` INT NOT NULL,
  PRIMARY KEY (`idcredit`),
  INDEX `fk_credit_accounts1_idx` (`idaccount` ASC) VISIBLE,
  CONSTRAINT `fk_credit_accounts1`
    FOREIGN KEY (`idaccount`)
    REFERENCES `bank_simul`.`accounts` (`idaccount`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `bank_simul`.`accounts_has_customers`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `bank_simul`.`accounts_has_customers` (
  `idAccount` INT NOT NULL,
  `idCustomer` INT NOT NULL,
  INDEX `fk_accounts_has_customers_customers1_idx` (`idCustomer` ASC) VISIBLE,
  INDEX `fk_accounts_has_customers_accounts_idx` (`idAccount` ASC) VISIBLE,
  CONSTRAINT `fk_accounts_has_customers_accounts`
    FOREIGN KEY (`idAccount`)
    REFERENCES `bank_simul`.`accounts` (`idaccount`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_accounts_has_customers_customers1`
    FOREIGN KEY (`idCustomer`)
    REFERENCES `bank_simul`.`customers` (`idcustomer`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

USE `bank_simul` ;

-- -----------------------------------------------------
-- procedure kirjaudu_sisaan
-- -----------------------------------------------------

DELIMITER $$
USE `bank_simul`$$
CREATE PROCEDURE `kirjaudu_sisaan` (
IN korttiID INT,
IN korttiPIN INT)
Aliohjelma:
BEGIN
	DECLARE tiliID INT DEFAULT 0;
    DECLARE failCount INT DEFAULT 0;
    DECLARE credit BOOL DEFAULT false;
    SELECT failed_attempts INTO failCount FROM cards WHERE idcard=korttiID;
    IF failCount >= 3 THEN
		SELECT "Kortti on lukittu";
        LEAVE Aliohjelma;
	END IF;
    SELECT idaccount INTO tiliID FROM cards WHERE idcard=korttiID AND pin=korttiPIN;
    IF tiliID=0 THEN
		SELECT 'Kirjautuminen epäonnistui.';
        UPDATE cards
        SET failed_attempts=failed_attempts+1
        WHERE idcard=korttiID;
		SELECT failed_attempts INTO failCount FROM cards WHERE idcard=korttiID;
		IF failCount >= 3 THEN
			SELECT "Kortti on lukittu.";
		END IF;
        LEAVE Aliohjelma;
	ELSE
		SELECT "Kirjautuminen onnistui";
        UPDATE cards
        SET failed_attempts=0
        WHERE idcard=korttiID;
        SELECT has_credit INTO credit FROM cards WHERE idCard=korttiID;
        SELECT @tiliID, @credit;
	END IF;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure tarkista_saldo
-- -----------------------------------------------------

DELIMITER $$
USE `bank_simul`$$
CREATE PROCEDURE `tarkista_saldo` (
IN tiliID INT,
IN luotto BOOL,
OUT saldo INT,
OUT luottoraja INT)
Aliohjelma:
BEGIN
	IF luotto = false THEN
		SELECT balance INTO saldo FROM accounts WHERE idaccount=tiliID;
        SELECT @saldo;
	ELSE
		SELECT balance INTO saldo FROM credit WHERE idaccount=tiliID;
        SELECT credit_limit INTO luottoraja FROM credit WHERE idaccount=tiliID;
        SELECT @saldo;
        SELECT @luottoraja;
	END IF;
    SELECT * FROM transactions ORDER BY transaction_time DESC LIMIT 5;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure nosta_rahaa
-- -----------------------------------------------------

DELIMITER $$
USE `bank_simul`$$
CREATE PROCEDURE `nosta_rahaa` (
IN tiliID INT,
IN luotto BOOL,
IN summa INT)
BEGIN
	DECLARE saldo INT DEFAULT 0;
    DECLARE luottoraja INT DEFAULT 0;
	IF luotto=false THEN
		SELECT balance INTO saldo FROM accounts WHERE idaccount=tiliID;
        IF summa <= saldo THEN
			UPDATE accounts SET balance=balance-summa WHERE idaccount=tiliID;
            INSERT INTO transactions VALUES(0, summa, CURRENT_TIMESTAMP());
        ELSE
			SELECT 'Tililäsi ei ole tarpeeksi rahaa';
        END IF;
	ELSE
		SELECT balance INTO saldo FROM credit WHERE idaccount=tiliID;
        SELECT credit_limit INTO luottoraja FROM credit WHERE idaccount=tiliID;
        IF summa+saldo <= luottoraja THEN
			UPDATE credit SET balance=balance+summa WHERE idaccount=tiliID;
            INSERT INTO transactions VALUES(1, summa, CURRENT_TIMESTAMP());
        ELSE
			SELECT 'Luottoraja ylitetty, nosto evätty';
        END IF;        
    END IF;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure selaa_tilitapahtumia
-- -----------------------------------------------------

DELIMITER $$
USE `bank_simul`$$
CREATE PROCEDURE `selaa_tilitapahtumia` (
IN tiliID INT,
IN alkaen_tapahtumasta INT)
BEGIN
	SELECT * FROM transactions WHERE idaccount=tiliID ORDER BY transaction_time DESC LIMIT alkaen_tapahtumasta,10;
END$$

DELIMITER ;
CREATE USER 'bank_simul_user' IDENTIFIED BY '1234';

GRANT ALL ON `bank_simul`.* TO 'bank_simul_user';

SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;