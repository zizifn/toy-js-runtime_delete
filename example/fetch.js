import { fetch } from 'toyjsruntime:net';

const resp = fetch('https://jsonplaceholder.typicode.com/todos/1')

resp.then(value=>{
    console.log('promise', value);
})

console.log('resp', resp);
