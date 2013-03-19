.load "./sqlite3_lz4.so"
CREATE TABLE test(name TEXT);
INSERT INTO test VALUES(lz4compress('I am Sam Sam I am That Sam-I-am! That Sam-I-am! I do not like that Sam-I-am! Do you like green eggs and ham? I do not like them, Sam-I-am. I do not like green eggs and ham.'));
SELECT length(name) FROM test;
SELECT length(lz4uncompress(name)), lz4uncompress(name) FROM test;
