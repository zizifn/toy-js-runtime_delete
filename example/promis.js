console.log("start!");

const p1 = new Promise(
    (resolve, reject)=>{
resolve("p1 resolved");
    }
)

console.log(p1);

p1.then(value=>{
    console.log(value);
})

console.log("end!");