BEGIN TRANSACTION;
CREATE TABLE templet(f1, f2, f3, f4, indication, comments);
CREATE TABLE templet_copy(f1, f2, f3, f4, indication, comments);
INSERT INTO "templet" VALUES('','','','','','');
COMMIT;
