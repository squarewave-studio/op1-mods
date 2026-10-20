import { scene, line, text, header, parameter } from './components.mjs';

// Original teaching drawings. No stock paths, coordinates or glyphs are used.
const names = ['blue', 'green', 'white', 'orange'];
function frame(title) { const s = scene(); header(s, { title }); return s; }
function box(s, id, x, y, w, h, tone = 'secondary') {
  line(s, id, [[x,y],[x+w,y],[x+w,y+h],[x,y+h],[x,y]], tone, 1.5);
}
function ring(s, id, x, y, r, tone) {
  line(s, id, Array.from({length:25}, (_, i) => [x+r*Math.cos(i*Math.PI/12),y+r*Math.sin(i*Math.PI/12)]), tone, 1.5);
}
function strip(s, labels, values, units = []) {
  labels.forEach((label, i) => parameter(s, { id:`control-${i}`, label, value:values[i], unit:units[i] || '', lane:i, x:9+i*80, y:102, width:70 }));
  return s;
}
function wave(s, id, x, y, w, h, tone, phase = 0) {
  line(s, id, Array.from({length:49}, (_,i) => [x+w*i/48,y+Math.sin(i*Math.PI/12+phase)*h]), tone, 1.5);
}
export function envelopePattern() {
  const s=frame('AMPLITUDE ENVELOPE');
  line(s,'axis',[[24,86],[296,86]],'decorative');
  const points=[[24,86],[72,37],[128,62],[224,62],[296,86]];
  names.forEach((tone,i)=>line(s,`stage-${i}`,points.slice(i,i+2),tone,1.5));
  return strip(s,['ATTACK','DECAY','SUSTAIN','RELEASE'],['12','240','65','1.2'],['MS','MS','%','S']);
}
export function samplerPattern() {
  const s=frame('SAMPLE REGION');
  for(let i=0;i<64;i++) { const h=3+Math.abs(Math.sin(i*1.7)*Math.cos(i*.09))*20; line(s,`peak-${i}`,[[25+i*4.2,62-h],[25+i*4.2,62+h]],'secondary',1); }
  [44,96,220,280].forEach((x,i)=>line(s,`marker-${i}`,[[x,34],[x,89]],names[i],1.5));
  return strip(s,['START','LOOP IN','LOOP OUT','END'],['0.1','0.4','1.8','2.0'],['S','S','S','S']);
}
export function synthPattern() {
  const s=frame('TWO OSCILLATORS');
  wave(s,'osc-a',26,52,110,13,'blue'); wave(s,'osc-b',184,52,110,13,'green',1);
  line(s,'mix-left',[[136,52],[160,78]],'blue',1.5); line(s,'mix-right',[[184,52],[160,78]],'green',1.5);
  line(s,'output',[[160,78],[266,78]],'orange',1.5);
  return strip(s,['SHAPE A','SHAPE B','DETUNE','BALANCE'],['25','60','7','50'],['%','%','CT','%']);
}
export function drumPattern() {
  const s=frame('DRUM VOICES');
  for(let i=0;i<8;i++) { const x=24+i*35; box(s,`voice-${i}`,x,42,25,35,i===2?'blue':'decorative'); line(s,`hit-${i}`,[[x+5,70],[x+9,49+i%3*5],[x+13,70],[x+20,70]],i===2?'blue':'secondary',1.5); }
  return strip(s,['PITCH','DECAY','TONE','LEVEL'],['0','180','45','80'],['ST','MS','%','%']);
}
export function delayPattern() {
  const s=frame('DELAY REPEATS');
  [44,100,156,212,268].forEach((x,i)=>{line(s,`echo-${i}`,[[x,84],[x,38+i*8]],i===0?'blue':i<3?'green':'decorative',2);line(s,`tick-${i}`,[[x-4,84],[x+4,84]],'secondary',1);});
  line(s,'time-axis',[[28,88],[290,88]],'secondary');
  return strip(s,['TIME','FEEDBACK','FILTER','MIX'],['125','35','2.4','25'],['MS','%','KHZ','%']);
}
export function lfoPattern() {
  const s=frame('MODULATION ROUTE');
  wave(s,'source',24,60,94,17,'blue');
  line(s,'route',[[126,60],[188,60],[180,55],[188,60],[180,65]],'green',1.5);
  box(s,'target',204,41,88,38,'white');
  text(s,{id:'destination',value:'PITCH',x:248,y:54,align:'center'});
  return strip(s,['SPEED','AMOUNT','DEST','PHASE'],['1.2','20','1','90'],['HZ','%','','DEG']);
}
export function sequencerPattern() {
  const s=frame('EIGHT STEP PATTERN');
  for(let i=0;i<8;i++) { const x=24+i*35; box(s,`step-${i}`,x,38,25,48,i===3?'white':'decorative'); if(i%3!==2)line(s,`gate-${i}`,[[x+6,74],[x+6,49+i%3*7],[x+19,49+i%3*7],[x+19,74]],'blue',2); }
  return strip(s,['LENGTH','SWING','GATE','OCTAVE'],['8','55','70','0'],['','%','%','']);
}
export function tapePattern() {
  const s=frame('TAPE OVERVIEW');
  for(let i=0;i<4;i++) { const y=35+i*13;line(s,`track-${i}`,[[24,y+8],[296,y+8]],'decorative');box(s,`clip-${i}`,32+i*18,y,100+i*22,7,names[i]); }
  line(s,'playhead',[[182,31],[182,91]],'white',1.5);
  return strip(s,['TRACK','LOOP IN','LOOP OUT','SPEED'],['2','1','9','100'],['','','','%']);
}
export function mixerPattern() {
  const s=frame('FOUR TRACK LEVELS');
  for(let i=0;i<4;i++) { const x=44+i*77;line(s,`rail-${i}`,[[x,34],[x,88]],'decorative',1.5);line(s,`level-${i}`,[[x,88],[x,44+i*7]],names[i],2);box(s,`handle-${i}`,x-9,42+i*7,18,6,names[i]); }
  return strip(s,['TRACK 1','TRACK 2','TRACK 3','TRACK 4'],['80','65','50','35'],['%','%','%','%']);
}
export function eqPattern() {
  const s=frame('THREE BAND EQ');
  line(s,'zero',[[24,69],[296,69]],'decorative');
  line(s,'low',[[24,49],[65,49],[100,69]],'blue',1.5);
  line(s,'mid',[[100,69],[151,80],[198,69]],'green',1.5);
  line(s,'high',[[198,69],[244,53],[296,53]],'white',1.5);
  return strip(s,['LOW','MID','HIGH','OUTPUT'],['3','-2','2','0'],['DB','DB','DB','DB']);
}
export function dynamicsPattern() {
  const s=frame('DYNAMICS');
  line(s,'reference',[[34,85],[96,85],[190,36]],'decorative');
  line(s,'transfer',[[34,85],[96,85],[190,62],[276,54]],'blue',1.5);
  for(let i=0;i<10;i++)line(s,`meter-${i}`,[[248+i*4,84],[248+i*4,75]],i<6?'green':'decorative',2);
  return strip(s,['THRESH','RATIO','RELEASE','OUTPUT'],['-12','4','120','2'],['DB','','MS','DB']);
}
export function tempoPattern() {
  const s=frame('TEMPO AND SYNC');
  text(s,{id:'tempo-value',value:'120',x:160,y:41,size:'large',align:'center',tone:'blue'});
  text(s,{id:'tempo-unit',value:'BPM',x:160,y:78,align:'center',tone:'blue'});
  return strip(s,['TEMPO','CLICK','SYNC','SWING'],['120','30','1','50'],['','%','','%']);
}
export function inputPattern() {
  const s=frame('INPUT MONITOR');
  box(s,'input',24,43,72,36,'blue');text(s,{id:'source',value:'LINE',x:60,y:55,align:'center',tone:'blue'});
  line(s,'route',[[96,61],[136,61]],'secondary',1.5);
  for(let i=0;i<22;i++)line(s,`meter-${i}`,[[144+i*7,73],[144+i*7,49]],i<15?'green':'decorative',2);
  return strip(s,['SOURCE','GAIN','THRESH','MONITOR'],['1','6','-18','75'],['','DB','DB','%']);
}
export function albumPattern() {
  const s=frame('RECORDING SIDE A');
  ring(s,'record',84,61,28,'secondary');ring(s,'hub',84,61,8,'blue');
  text(s,{id:'elapsed',value:'02:18',x:184,y:47,size:'medium',tone:'white',maxWidth:120});
  text(s,{id:'recording',value:'RECORDING',x:184,y:76,tone:'orange',maxWidth:120});
  return s;
}
export const patterns = [
  ['synth','Synth Relationships',synthPattern,'Two sources meet at one output. Colour identifies ownership without copying an engine illustration.'],
  ['envelope','Envelope Stages',envelopePattern,'Four coloured stages and four stable readout columns. Units share the value baseline.'],
  ['sampler','Sample Boundaries',samplerPattern,'Cached peaks, four distinct boundaries and a small live cursor layer. Recompute peaks when sample data changes.'],
  ['drum','Drum Selection',drumPattern,'Selection uses an outline and position as well as colour. Keep pad geometry fixed when selecting a voice.'],
  ['delay','Effect Relationships',delayPattern,'Repeat height explains feedback. Store the stable guide and update only the changing repeat representation.'],
  ['lfo','Modulation Routing',lfoPattern,'Source, depth and destination form a readable route. The host example is a static snapshot.'],
  ['sequencer','Step And Play Position',sequencerPattern,'Notes use Blue; the white frame identifies the play position separately. Update only affected steps.'],
  ['tape','Tape Position',tapePattern,'Four lanes and a playhead communicate time without reel animation. The ruler and clips can be retained geometry.'],
  ['mixer','Four Control Lanes',mixerPattern,'The physical encoder order is repeated in colour, position and track labels. Meter and fader state have separate jobs.'],
  ['eq','Frequency Regions',eqPattern,'Three bands meet at a shared neutral line. Rebuild the response only when relevant controls change.'],
  ['dynamics','Dynamics Feedback',dynamicsPattern,'A stable transfer diagram and a small meter layer keep configuration and live activity distinct.'],
  ['tempo','One Dominant Value',tempoPattern,'Large tempo with Small units and Medium supporting values. The duplicate control value demonstrates its encoder mapping.'],
  ['input','Input And Level',inputPattern,'A labelled source feeds a segmented meter. Dim segments are decoration; the source and gain stay readable.'],
  ['album','Recording State',albumPattern,'Elapsed time and the word RECORDING carry the state. Colour alone is insufficient. No decorative rotation is needed.']
].map(([id,title,build,lesson])=>({id,title,build,lesson}));
