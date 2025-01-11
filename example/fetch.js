import { fetch } from 'toyjsruntime:net';
import { setTimeout } from 'toyjsruntime:os';

const before = new Date().getTime();

const resp = fetch('https://jsonplaceholder.typicode.com/todos/1')
console.log(`delay ${new Date().getTime() - before} ms`);
resp.then(value => {
    console.log('promise', value);
});
setTimeout(() => {
    console.log("Set time out");
})

console.log('end!', resp);
