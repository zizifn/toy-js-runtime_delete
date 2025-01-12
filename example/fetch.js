// import { old_fetch as fetch } from 'toyjsruntime:net';
import { fetch } from 'toyjsruntime:net';

import { setTimeout } from 'toyjsruntime:os';

const before = new Date().getTime();

/** @type Promise */
const resp = fetch('https://jsonplaceholder.typicode.com/todos/1')
console.log(`delay ${new Date().getTime() - before} ms`);
resp.then(value => {
    console.log('promise', value);
}).catch(err =>{
    console.log('promise error', err);
});
setTimeout(() => {
    console.log("Set time out");
}, 10)

console.log('end!', resp);
