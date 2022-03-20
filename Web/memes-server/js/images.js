'use strict';

/*  
1. images model.
2. render images.
*/

var gNextId = 0;
var gImgs;

function init() {
    gImgs = createImgs(imgNames);
    renderImgs(gImgs);
}
function getRandomInt(max) {
    return Math.floor(Math.random() * Math.floor(max));
}
function randomEdit() {
    var randNo = getRandomInt(gImgs.length);
    initMemeEditor(randNo, gImgs[randNo].name);
}

function orderImgs() {
    gImgs = createImgs(imgNames.sort());
    renderImgs(gImgs);
    if (currentView === "meme"){
        currentView = 'gallery';
        toggleView();
    }
}
function randomImgs() {
    var array = imgNames;
    for (var i = array.length - 1; i > 0; i--) {
        var j = Math.floor(Math.random() * (i + 1));
        var temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
    gImgs = createImgs(array);
    renderImgs(gImgs);
    if (currentView === "meme"){
        currentView = 'gallery';
        toggleView();
    }
}

function createImgs(imgNames) {
    var imgs = [];
    gNextId = 0;
    imgNames.map(function(name) {
        imgs.push(createImage(name));
    });
    return imgs;
}

function createImage(name) {
    return {
        id: gNextId++,
        url: './img/gallery/' + name + '.jpg',
        name: name
    };
}

function renderImgs(imgs) {
    var strHtml = imgs.map(function (img, idx) {
        return `
        <img id='${img.id}' src='${img.url}' onclick="initMemeEditor(${img.id},'${img.name}',this)" alt='meme picture'/>
        `
    })
        .join(' ')
        
    document.querySelector('.gallery').innerHTML = strHtml;
}
