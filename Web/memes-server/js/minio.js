// access minio REST API, assuming minio cluster running

async function backup() {
    try{
        var canvas = document.querySelector('.memeCanvas');

        var dt = canvas.toDataURL('image/png');
        dt = dt.replace(/^data:image\/[^;]*/, 'data:application/octet-stream');
        dt = dt.replace(/^data:application\/octet-stream/, 'data:application/octet-stream;headers=Content-Disposition%3A%20attachment%3B%20filename=canvas.png');
        //console.log('backup image', dt.length)
        let url = meme_server + "/api/backup";
        let uuid = sha256(gMeme.name + gMeme.txts[0].line + gMeme.txts[1].line);

        let body = {
            name: uuid+'.jpg',
            image: dt,
        };
        console.log('saving to minio', uuid);
        const response = await fetch(url, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(body)
        });
    }catch(e){ console.error('backup',e)};
}