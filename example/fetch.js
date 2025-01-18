// import { old_fetch as fetch } from 'toyjsruntime:net';
import { fetch } from 'toyjsruntime:net';

// import { setTimeout } from 'toyjsruntime:os';

const before = new Date().getTime();

/** @type Promise */
const resp = fetch('https://jsonplaceholder.typicode.com/todos/1')
console.log(`delay ${new Date().getTime() - before} ms`);
resp.then(value => {
    console.log('promise', value);
}).catch(err =>{
    console.log('promise error', err);
});

const result = setTimeout(() => {
    console.log("setTimeout is 1000---dd--------");
    return "string";
})

console.log('setTimeout timeid',  result);
// setTimeout("xxxxx", 1000)

console.log('end!!!!', resp);
