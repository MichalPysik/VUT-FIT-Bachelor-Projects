DROP TABLE IF EXISTS obrazek;
DROP TABLE IF EXISTS nabidka;
DROP TABLE IF EXISTS registrace_do_aukce;
DROP TABLE IF EXISTS aukce;
DROP TABLE IF EXISTS kategorie;
DROP TABLE IF EXISTS licitator;
DROP TABLE IF EXISTS uzivatel;




CREATE TABLE uzivatel (
    id INT NOT NULL PRIMARY KEY AUTO_INCREMENT,
    uzivatelske_jmeno VARCHAR(32) UNIQUE NOT NULL,
    heslo VARCHAR(64) NOT NULL,
    email VARCHAR(64) DEFAULT NULL,
    jmeno VARCHAR(64) DEFAULT NULL,
    prijmeni VARCHAR(64) DEFAULT NULL,
    datum_narozeni DATE DEFAULT NULL,
    telefon VARCHAR(15) DEFAULT NULL,
    adresa VARCHAR(128) DEFAULT NULL,
    prava ENUM('uzivatel', 'licitator', 'admin')
);

CREATE TABLE licitator (
    uzivatel_id INT NOT NULL PRIMARY KEY,
    ico CHAR(8) UNIQUE
        CHECK(REGEXP_LIKE(ico, '[0-9]{8}')),
    CONSTRAINT licitator_je_uzivatelem
        FOREIGN KEY (uzivatel_id) REFERENCES uzivatel (id)
        ON DELETE CASCADE
);

CREATE TABLE kategorie (
    id INT NOT NULL PRIMARY KEY AUTO_INCREMENT,
    nazev VARCHAR(64),
    popis VARCHAR(256)
);

CREATE TABLE aukce (
    id INT NOT NULL PRIMARY KEY AUTO_INCREMENT,
    nazev VARCHAR(64) NOT NULL,
    popis VARCHAR(256) DEFAULT NULL,
    kategorie_id INT NOT NULL,
    typ ENUM('poptavkova', 'nabidkova') NOT NULL,
    pravidla ENUM('otevrena', 'uzavrena') NOT NULL,
    stav ENUM('navrzena', 'zahajena', 'ukoncena', 'vyhodnocena') NOT NULL,
    vyvolavaci_cena DECIMAL(10,2) DEFAULT NULL,
    max_cena DECIMAL(10,2) DEFAULT NULL,
    min_cena DECIMAL(10,2) DEFAULT NULL,
    cas_zahajeni DATETIME NOT NULL,
    cas_ukonceni DATETIME DEFAULT NULL,
    autor_id INT NOT NULL,
    licitator_id INT DEFAULT NULL,
    vyherce_id INT DEFAULT NULL,
    CONSTRAINT uzivatel_je_autorem_aukce
        FOREIGN KEY (autor_id) REFERENCES uzivatel (id)
        ON DELETE CASCADE,
    CONSTRAINT licitator_spravuje_aukci
        FOREIGN KEY (licitator_id) REFERENCES licitator (uzivatel_id)
        ON DELETE CASCADE,
    CONSTRAINT uzivatel_je_vyhercem_aukce
        FOREIGN KEY (vyherce_id) REFERENCES uzivatel (id)
        ON DELETE SET NULL,
    CONSTRAINT kategorie_aukce
        FOREIGN KEY (kategorie_id) REFERENCES kategorie (id)
        ON DELETE CASCADE
);

CREATE TABLE registrace_do_aukce (
    uzivatel_id INT NOT NULL,
    aukce_id INT NOT NULL,
    licitator_id INT DEFAULT NULL,
    schvaleno BOOLEAN,
    PRIMARY KEY(uzivatel_id, aukce_id),
    CONSTRAINT registrace_uzivatel
        FOREIGN KEY (uzivatel_id) REFERENCES uzivatel (id) 
        ON DELETE CASCADE,
    CONSTRAINT registrace_aukce
        FOREIGN KEY (aukce_id) REFERENCES aukce (id)
        ON DELETE CASCADE,
    CONSTRAINT registrace_licitator
        FOREIGN KEY (licitator_id) REFERENCES licitator (uzivatel_id)
        ON DELETE CASCADE
);

CREATE TABLE nabidka (
    id INT NOT NULL PRIMARY KEY AUTO_INCREMENT,
    aukce_id INT NOT NULL,
    uzivatel_id INT NOT NULL,
    cena DECIMAL(10,2),
    cas DATETIME,
    CONSTRAINT nabidka_aukce
        FOREIGN KEY (aukce_id) REFERENCES aukce (id)
        ON DELETE CASCADE,
    CONSTRAINT nabidka_uzivatel
        FOREIGN KEY (uzivatel_id) REFERENCES uzivatel (id)
        ON DELETE CASCADE
);

CREATE TABLE obrazek (
    id INT NOT NULL PRIMARY KEY AUTO_INCREMENT,
    aukce_id INT NOT NULL,
    cesta VARCHAR(256) NOT NULL,
    CONSTRAINT obrazek_aukce
        FOREIGN KEY (aukce_id) REFERENCES aukce (id)
        ON DELETE CASCADE
);
 




INSERT INTO uzivatel (uzivatelske_jmeno, heslo, prava)
VALUES ('bartman', 'ajeje', 'uzivatel');
INSERT INTO uzivatel (uzivatelske_jmeno, heslo, prava)
VALUES ('user20', 'mypass', 'uzivatel');
INSERT INTO uzivatel (uzivatelske_jmeno, heslo, prava)
VALUES ('bozi_licitator', 'licitujubruh', 'licitator');
INSERT INTO uzivatel (uzivatelske_jmeno, heslo, prava)
VALUES ('bozi_admin', '123superpassword', 'admin');

INSERT INTO licitator (uzivatel_id, ico)
VALUES(3, '12345678');
INSERT INTO licitator (uzivatel_id, ico)
VALUES(4, '88445522');

INSERT INTO kategorie (nazev, popis) VALUES ('aktiva', 'Aukce, jejíž předmětem jsou aktiva');
INSERT INTO kategorie (nazev, popis) VALUES ('majetek', 'Aukce, jejíž předmětem je majetek');
INSERT INTO kategorie (nazev, popis) VALUES ('zbozi', 'Aukce, jejíž předmětem je zboží');
INSERT INTO kategorie (nazev, popis) VALUES ('sluzby', 'Aukce, jejíž předmětem jsou služby');

INSERT INTO aukce (nazev, kategorie_id, typ, pravidla, stav, vyvolavaci_cena, cas_zahajeni, autor_id, licitator_id)
VALUES ('prodam auto', 2, 'poptavkova', 'otevrena', 'zahajena', 199.99, NOW(), 1, 3);
INSERT INTO aukce (nazev, kategorie_id, typ, pravidla, stav, vyvolavaci_cena, cas_zahajeni, autor_id, licitator_id)
VALUES ('koupim olej', 3, 'nabidkova', 'uzavrena', 'zahajena', 500.00, NOW(), 2, 4);

INSERT INTO registrace_do_aukce (uzivatel_id, aukce_id, licitator_id, schvaleno)
VALUES (2, 1, 3, TRUE);
INSERT INTO registrace_do_aukce (uzivatel_id, aukce_id, licitator_id, schvaleno)
VALUES (2, 2, 3, TRUE);

INSERT INTO nabidka (aukce_id, uzivatel_id, cena, cas)
VALUES (1, 2, 250.00, NOW());
INSERT INTO nabidka (aukce_id, uzivatel_id, cena, cas)
VALUES (1, 4, 400.00, NOW());