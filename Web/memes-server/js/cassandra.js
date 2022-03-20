// calls nodejs backend REST API 
// at ../routes/api.js to access cassandra DB 

//var meme_server = 'http://localhost:3001';
var meme_server = '';

async function saveDB() {
    try{
        let url = meme_server + "/api/save";
        let uuid = sha256(gMeme.name + gMeme.txts[0].line + gMeme.txts[1].line);
        let body = {
            name: gMeme.name,
            toptext: gMeme.txts[0].line,
            bottomtext: gMeme.txts[1].line,
            votes: gMeme.votes,
            uuid: uuid,
        };
        const response = await fetch(url, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(body)
        })
        console.log('saveDB', body)
    } catch(e) {
        console.error(e);
    }
}

async function likedImgs() {
    try{
        let url = meme_server + "/api/list";
        const response = await fetch(url, {method: 'GET'});
        var res = await response.json();
        console.log('allDB', res);
        var imgListDB = [];
        for (var i = 0; i < res.rows.rows.length; i++){
            var row = res.rows.rows[i];
            imgListDB.push(row.name);
        }
        gImgs = createImgs(imgListDB);
        renderImgs(gImgs); 
        if (currentView === "meme") {
            currentView = "gallery";
            console.log("view",currentView);
            toggleView();
        }
        return res;
    } catch(e) {
        console.error(e);
    }
}
async function getDB(name) {
    try{
        let url = meme_server + "/api/get";
        const response = await fetch(url, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                name: gMeme.name,
            })
        })
        var res = await response.json();
        console.log('getDB', res);
        if (res.rows.length === 0 || res.status !== 'ok') {
            return null;
        }
        var highVote = 0;
        var pickedItem = res.rows[0];
        for (var i = 0; i < res.rows.length; i++) {
            var item = res.rows[i];
            console.log("item votes", item.votes);
            if (item.votes !== null) {
                if (item.votes > highVote) {
                    highVote = item.votes;
                    pickedItem = item;
                }
            }
        }
        gMeme.votes = highVote;
        gMeme.txts[0].line = pickedItem.toptext;
        gMeme.txts[1].line = pickedItem.bottomtext;
        console.log('picked gMeme',gMeme)
        drawCanvas();
        return res;
    } catch(e) {
        console.error(e);
    }
}
