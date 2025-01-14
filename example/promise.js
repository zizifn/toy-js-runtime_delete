
let resolve1 = null;
const p = new Promise((resolve, reject)=>{
    console.log("promise111");
    resolve1 = resolve;
});

p.then(value=>{
    console.log("promise->then", value);
});

console.log(p);

setTimeout(()=>{
    console.log("settime out");
}, 1000);

// resolve1('neddd')
console.log("end!!!");