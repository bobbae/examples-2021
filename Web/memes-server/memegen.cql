

DROP TABLE IF EXISTS astrakeyspace1.memes_saved;
CREATE TABLE astrakeyspace1.memes_saved (
    key text PRIMARY KEY,
    id text,
    sessionid text, 
    uuid text,
    bottomtext text,
    image_source text,
    url text,
    memeid text,
    comment text,
    memename text,
    metadata text,
    name text,
    width int,
    height int,
    box_count smallint,
    info text,
    description text,
    summary text,
    template text,
    toptext text,
    votes text
);
