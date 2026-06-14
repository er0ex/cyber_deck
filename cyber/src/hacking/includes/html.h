#pragma once
const char INDEX_HTML[] PROGMEM = R"(<!DOCTYPE html><html>
<head>
<meta charset='UTF-8'>
<meta name='viewport' content='width=device-width,initial-scale=1'>
<title>CYBERDEVICE</title>
<style>
  :root{--bg:#000000;--on:#00ff00;--dim:#00aa44;--line:#004466;--sel:#002200}
  *{box-sizing:border-box;margin:0;padding:0}
  body{background:var(--bg);color:var(--on);font-family:monospace;padding:16px;font-size:13px}
  header{border-bottom:1px solid var(--line);padding-bottom:8px;margin-bottom:16px;display:flex;justify-content:space-between;align-items:center}
  header span{color:var(--dim);font-size:11px}
  .grid{display:grid;grid-template-columns:1fr 1fr;gap:10px}
  .card{border:1px solid var(--line);border-radius:4px;padding:12px;background:var(--sel)}
  .card h3{color:var(--dim);font-size:10px;letter-spacing:2px;margin-bottom:10px;border-bottom:1px solid var(--line);padding-bottom:6px}
  .btn{display:block;width:100%;background:var(--bg);border:1px solid var(--on);color:var(--on);
       padding:8px;margin:4px 0;cursor:pointer;font-family:monospace;font-size:12px;
       border-radius:3px;text-align:left;text-decoration:none}
  .btn:hover{background:var(--sel)}
  .btn::before{content:'> ';color:var(--dim)}
  .status-row{display:flex;justify-content:space-between;padding:4px 0;border-bottom:1px solid var(--line);font-size:11px}
  .status-row:last-child{border:none}
  .ok{color:var(--on)} .err{color:#ff3333}
  #log{background:var(--bg);border:1px solid var(--line);padding:8px;height:100px;overflow-y:auto;font-size:10px;color:var(--dim);border-radius:3px;margin-top:6px}
  .full{grid-column:1/-1}
  footer{border-top:1px solid var(--line);margin-top:16px;padding-top:8px;color:var(--dim);font-size:10px;display:flex;justify-content:space-between}
</style></head>
<body>
<header><div>// CYBERDEVICE v1.0</div><span id='htime'>--:--:--</span></header>
<div class='grid'>
  <div class='card'>
    <h3>// STATUS</h3>
    <div class='status-row'><span>SD</span>  <span id='s-sd'>...</span></div>
    <div class='status-row'><span>HEAP</span><span id='s-heap'>...</span></div>
    <div class='status-row'><span>IP</span>  <span id='s-ip'>...</span></div>
  </div>
  <div class='card'>
    <h3>// SYSTEM</h3>
    <a class='btn' onclick='cmd("/cmd/reboot","REBOOT")'>REBOOT</a>
    <a class='btn' onclick='cmd("/cmd/sd","SD CHECK")'>SD CHECK</a>
  </div>
  <div class='card full'>
    <h3>// FILES</h3>
    <a class='btn' href='/files'>BROWSE FILES</a>
  </div>
  <div class='card full'>
    <h3>// LOG</h3>
    <div id='log'></div>
  </div>
</div>
<footer><span>WE ARE ANONYMOUS</span><span id='heap-bar'>HEAP: ---</span></footer>
<script>
function log(m,c){var d=document.getElementById('log');d.innerHTML+='<span style="color:'+(c||'#00aa44')+'">'+new Date().toLocaleTimeString()+' > '+m+'</span><br>';d.scrollTop=d.scrollHeight;}
function cmd(u,label){log(label+' ...','#004466');fetch(u).then(r=>r.text()).then(t=>{log(label+': '+t,'#00ff00');}).catch(e=>log(label+' ERR','#ff3333'));}
function upd(){
  fetch('/cmd/status').then(r=>r.json()).then(d=>{
    document.getElementById('s-sd').innerHTML=d.sd?"<span class='ok'>MOUNTED</span>":"<span class='err'>NO CARD</span>";
    document.getElementById('s-heap').innerHTML="<span class='ok'>"+d.heap+" B</span>";
    document.getElementById('s-ip').innerHTML="<span class='ok'>"+d.wifi+"</span>";
    document.getElementById('heap-bar').innerText='HEAP: '+d.heap+' B';
  }).catch(()=>{});
  document.getElementById('htime').innerText=new Date().toLocaleTimeString();
}
setInterval(upd,3000);upd();
log('CONNECTED','#00ff00');
</script></body></html>)";


const char FILES_HEADER_HTML[] PROGMEM = R"(<!DOCTYPE html><html>
<head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>
<title>FILES</title>
<style>
  :root{--bg:#000;--on:#00ff00;--dim:#00aa44;--line:#004466;--sel:#002200}
  *{box-sizing:border-box;margin:0;padding:0}
  body{background:var(--bg);color:var(--on);font-family:monospace;padding:16px;font-size:13px}
  header{border-bottom:1px solid var(--line);padding-bottom:8px;margin-bottom:10px;display:flex;justify-content:space-between}
  .path{color:var(--dim);font-size:11px;margin-bottom:10px;word-break:break-all}
  .row{display:flex;justify-content:space-between;align-items:center;
       padding:8px 4px;border-bottom:1px solid var(--line);color:var(--on);text-decoration:none}
  .row:hover{background:var(--sel)}
  .name{flex:1} .size{color:var(--dim);font-size:11px;margin:0 10px}
  .type{color:var(--line);font-size:10px;width:44px;text-align:right}
  a{text-decoration:none}
</style></head>
<body>
<header><div>// FILE BROWSER</div><a href='/' style='color:var(--dim)'>HOME</a></header>)";


const char VIDEO_HTML[] PROGMEM = R"(<!DOCTYPE html><html>
<head>
<meta name='theme-color' content='#000000'>
<link rel='manifest' href='/manifest.json'>
<meta charset='UTF-8'>
<title>CYBERDEVICE MEDIA</title>
<style>
  :root{--bg:#0a0a0a;--on:#00ff00;--dim:#00aa44;--line:#004466;--sel:#001a00;--card:#0d1a0d}
  *{box-sizing:border-box;margin:0;padding:0}
  body{background:var(--bg);color:var(--on);font-family:monospace;min-height:100vh}
  header{background:#000;border-bottom:1px solid var(--line);padding:14px 20px;display:flex;justify-content:space-between;align-items:center;position:sticky;top:0;z-index:10}
  .logo{font-size:14px;letter-spacing:2px}
  .logo span{color:var(--dim)}
  .counter{font-size:10px;color:var(--dim)}
  .grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(260px,1fr));gap:16px;padding:20px}
  .card{background:var(--card);border:1px solid var(--line);border-radius:6px;overflow:hidden;transition:border-color .2s}
  .card:hover{border-color:var(--on)}
  .thumb{width:100%;aspect-ratio:16/9;background:#000;display:flex;align-items:center;justify-content:center;font-size:40px;cursor:pointer;position:relative;overflow:hidden}
  .thumb .play{position:absolute;font-size:48px;transition:opacity .2s;pointer-events:none}
  .thumb:hover .play{opacity:0.6}
  .info{padding:12px}
  .title{font-size:12px;color:var(--on);margin-bottom:6px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
  .meta{font-size:10px;color:var(--dim);margin-bottom:10px}
  .actions{display:flex;gap:8px}
  .btn{flex:1;background:var(--bg);border:1px solid var(--line);color:var(--dim);padding:7px;
       font-family:monospace;font-size:11px;cursor:pointer;border-radius:3px;text-decoration:none;text-align:center}
  .btn-watch{border-color:var(--on);color:var(--on)}
  .btn:hover{background:var(--sel)}
  .empty{text-align:center;padding:60px;color:var(--dim)}
</style>
</head>
<body>
<header>
  <div class='logo'>// <span>CYBER</span>MEDIA</div>
  <div class='counter' id='cnt'>загрузка...</div>
</header>
<div class='grid' id='grid'><div class='empty'>загрузка...</div></div>
<script>
fetch('/api/videos').then(r=>r.json()).then(files=>{
  var grid=document.getElementById('grid');
  document.getElementById('cnt').innerText=files.length+' VIDEO(S)';
  if(!files.length){grid.innerHTML="<div class='empty'>// НЕТ ФАЙЛОВ В /video</div>";return;}
  grid.innerHTML=files.map(f=>`
    <div class='card'>
      <div class='thumb' onclick='location.href="/watch?f="+encodeURIComponent("${f.path}")'><span class='play'>▶</span></div>
      <div class='info'>
        <div class='title'>${f.name}</div>
        <div class='meta'>${f.size}</div>
        <div class='actions'>
          <a class='btn btn-watch' href='/watch?f=${f.path}'>▶ СМОТРЕТЬ</a>
          <a class='btn' href='/download?f=${f.path}'>↓ СКАЧАТЬ</a>
        </div>
      </div>
    </div>`).join('');
}).catch(()=>{document.getElementById('grid').innerHTML="<div class='empty'>// ОШИБКА ЗАГРУЗКИ</div>";});
</script>
</body></html>)";


const char WATCH_HTML[] PROGMEM = R"(<!DOCTYPE html><html>
<head>
<meta charset='UTF-8'>
<meta name='viewport' content='width=device-width,initial-scale=1'>
<title>CYBERMEDIA PLAYER</title>
<style>
  *{margin:0;padding:0;box-sizing:border-box}
  :root{--on:#00ff00;--dim:#00aa44;--line:#004466}
  body{background:#000;color:var(--on);font-family:monospace;height:100vh;display:flex;flex-direction:column}
  .bar{background:#0a0a0a;border-bottom:1px solid var(--line);padding:10px 16px;display:flex;align-items:center;gap:12px;flex-shrink:0}
  .back{border:1px solid var(--line);color:var(--dim);padding:6px 14px;font-family:monospace;font-size:11px;cursor:pointer;border-radius:3px;text-decoration:none;background:none}
  .back:hover{border-color:var(--on);color:var(--on)}
  .vtitle{font-size:12px;color:var(--on);flex:1;overflow:hidden;text-overflow:ellipsis;white-space:nowrap}
  .vwrap{flex:1;display:flex;align-items:center;justify-content:center;background:#000;position:relative}
  video{width:100%;height:100%;object-fit:contain;display:block}
  .controls{position:absolute;bottom:0;left:0;right:0;padding:10px 14px 14px;
            background:linear-gradient(transparent,rgba(0,0,0,.9));
            display:flex;flex-direction:column;gap:8px;
            opacity:0;transition:opacity .3s;pointer-events:none}
  .vwrap:hover .controls,.vwrap.show .controls{opacity:1;pointer-events:all}
  .prog{position:relative;height:5px;background:rgba(0,255,0,.15);border-radius:3px;cursor:pointer}
  .prog-buf{position:absolute;top:0;left:0;height:100%;background:rgba(0,255,0,.2);border-radius:3px;pointer-events:none}
  .prog-fill{position:absolute;top:0;left:0;height:100%;background:var(--on);border-radius:3px;pointer-events:none}
  .prog-dot{position:absolute;top:50%;width:11px;height:11px;background:var(--on);border-radius:50%;transform:translate(-50%,-50%);pointer-events:none}
  .row{display:flex;align-items:center;gap:8px}
  .cbtn{background:none;border:none;color:var(--on);font-size:16px;cursor:pointer;padding:3px 5px;font-family:monospace;opacity:.85}
  .cbtn:hover{opacity:1}
  .cbtn.sm{font-size:12px}
  .time{font-size:11px;color:var(--dim);min-width:88px}
  .sp{flex:1}
  input[type=range]{width:65px;accent-color:var(--on)}
  .spdbtn{font-size:11px;border:1px solid var(--line);color:var(--dim);padding:2px 6px;border-radius:3px;cursor:pointer;background:none;font-family:monospace}
  .spdbtn:hover{border-color:var(--on);color:var(--on)}
</style>
</head>
<body>
<div class='bar'>
  <a class='back' href='/'>← НАЗАД</a>
  <div class='vtitle' id='vtitle'></div>
</div>
<div class='vwrap' id='wrap'>
  <video id='v' preload='auto' playsinline webkit-playsinline></video>
  <div class='controls'>
    <div class='prog' id='prog'>
      <div class='prog-buf' id='buf'></div>
      <div class='prog-fill' id='fill'></div>
      <div class='prog-dot' id='dot'></div>
    </div>
    <div class='row'>
      <button class='cbtn' id='btnP' onclick='togglePlay()'>▶</button>
      <button class='cbtn sm' onclick='skip(-10)'>«10</button>
      <button class='cbtn sm' onclick='skip(10)'>10»</button>
      <span class='time' id='tlbl'>0:00 / 0:00</span>
      <span class='sp'></span>
      <button class='cbtn sm' id='btnM' onclick='toggleMute()'>🔊</button>
      <input type='range' min=0 max=1 step=.05 value=1 oninput='setVol(this.value)'>
      <button class='spdbtn' onclick='cycleSpeed()' id='spdBtn'>1×</button>
      <button class='cbtn sm' onclick='toggleFS()'>⛶</button>
    </div>
  </div>
</div>
<script>
var v=document.getElementById('v'),wrap=document.getElementById('wrap');
var p=new URLSearchParams(location.search),fp=p.get('f')||'';
document.getElementById('vtitle').textContent=decodeURIComponent(fp).split('/').pop();
v.src='/stream?f='+fp;
v.play().catch(function(){});
var fill=document.getElementById('fill'),buf=document.getElementById('buf'),dot=document.getElementById('dot'),prog=document.getElementById('prog');
function fmt(s){var m=Math.floor(s/60),sc=Math.floor(s%60);return m+':'+(sc<10?'0':'')+sc;}
v.addEventListener('timeupdate',function(){
  if(!v.duration)return;
  var pct=(v.currentTime/v.duration*100).toFixed(2)+'%';
  fill.style.width=pct;dot.style.left=pct;
  document.getElementById('tlbl').textContent=fmt(v.currentTime)+' / '+fmt(v.duration);
});
v.addEventListener('progress',function(){
  if(v.buffered.length&&v.duration)buf.style.width=(v.buffered.end(v.buffered.length-1)/v.duration*100)+'%';
});
v.addEventListener('play',function(){document.getElementById('btnP').textContent='⏸';});
v.addEventListener('pause',function(){document.getElementById('btnP').textContent='▶';});
var seeking=false;
function seekTo(e){var r=prog.getBoundingClientRect();var ratio=Math.max(0,Math.min(1,(e.clientX-r.left)/r.width));if(v.duration)v.currentTime=ratio*v.duration;}
prog.addEventListener('mousedown',function(e){seeking=true;seekTo(e);});
document.addEventListener('mousemove',function(e){if(seeking)seekTo(e);});
document.addEventListener('mouseup',function(){seeking=false;});
prog.addEventListener('touchstart',function(e){seeking=true;seekTo(e.touches[0]);e.preventDefault();},{passive:false});
document.addEventListener('touchmove',function(e){if(seeking)seekTo(e.touches[0]);},{passive:false});
document.addEventListener('touchend',function(){seeking=false;});
function togglePlay(){v.paused?v.play():v.pause();}
function skip(s){v.currentTime=Math.max(0,Math.min(v.duration||1e9,v.currentTime+s));}
function setVol(val){v.volume=val;}
function toggleMute(){v.muted=!v.muted;document.getElementById('btnM').textContent=v.muted?'🔇':'🔊';}
var speeds=[0.5,1,1.25,1.5,2],si=1;
function cycleSpeed(){si=(si+1)%speeds.length;v.playbackRate=speeds[si];document.getElementById('spdBtn').textContent=speeds[si]+'×';}
function toggleFS(){
  var inFS=document.fullscreenElement||document.webkitFullscreenElement||document.mozFullScreenElement;
  if(!inFS){
    if(v.requestFullscreen)v.requestFullscreen();
    else if(v.webkitRequestFullscreen)v.webkitRequestFullscreen();
    else if(v.webkitEnterFullscreen)v.webkitEnterFullscreen();
    else if(wrap.requestFullscreen)wrap.requestFullscreen();
    else if(wrap.webkitRequestFullscreen)wrap.webkitRequestFullscreen();
  }else{
    if(document.exitFullscreen)document.exitFullscreen();
    else if(document.webkitExitFullscreen)document.webkitExitFullscreen();
    else if(document.mozCancelFullScreen)document.mozCancelFullScreen();
  }
}
wrap.addEventListener('dblclick',toggleFS);
var hideT;
function showCtrl(){wrap.classList.add('show');clearTimeout(hideT);hideT=setTimeout(function(){if(!v.paused)wrap.classList.remove('show');},3000);}
wrap.addEventListener('mousemove',showCtrl);
wrap.addEventListener('touchstart',showCtrl,{passive:true});
document.addEventListener('keydown',function(e){
  if(e.key===' '||e.key==='k'){togglePlay();e.preventDefault();}
  if(e.key==='f'||e.key==='F')toggleFS();
  if(e.key==='ArrowRight')skip(10);
  if(e.key==='ArrowLeft')skip(-10);
  if(e.key==='m'||e.key==='M')toggleMute();
});
</script>
</body></html>)";