DROP TABLE IF EXISTS astrakeyspace1.memes_saved;
CREATE TABLE astrakeyspace1.memes_saved (
    name text,
    toptext text,
    bottomtext text,
    uuid text,
    votes int,
    PRIMARY KEY (name, votes)
)
WITH CLUSTERING ORDER BY (votes DESC);

