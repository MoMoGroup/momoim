CREATE TABLE users(
id INTEGER PRIMARY KEY AUTOINCREMENT,
name varchar UNIQUE,
key varchar
);
INSERT INTO sqlite_sequence VALUES('users',9999);
